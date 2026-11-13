#include "platform/pc/PcDiscord.hpp"

#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <thread>
#include <vector>

#include "platform/pc/Log.hpp"
#include "platform/pc/PcDiscordPipe.hpp"
#include "platform/pc/PcSystemInfo.hpp"
#include "platform/pc/PcUtf8.hpp"


namespace
{
const Logger sLog("Discord");
const unsigned int kOpHandshake = 0;
const unsigned int kOpFrame = 1;
const size_t kMaxDetailsLength = 127;
const int kRetrySeconds = 15;

struct DiscordState
{
    std::mutex mutex;
    std::condition_variable changed;
    std::string applicationId;
    std::string details;
    unsigned int revision = 0;
    long long startTime = 0;
};

DiscordState* sState = NULL;

bool Send(PcDiscordPipe& pipe, unsigned int opcode, const std::string& json)
{
    std::vector<unsigned char> message(8 + json.size());
    const uint32_t header[2] = {opcode, static_cast<uint32_t>(json.size())};
    for (int word = 0; word < 2; ++word)
        for (int byte = 0; byte < 4; ++byte)
            message[word * 4 + byte] = static_cast<unsigned char>(header[word] >> (byte * 8));
    std::memcpy(message.data() + 8, json.data(), json.size());
    return pipe.Write(message);
}

std::string JsonString(const std::string& text)
{
    std::string result = "\"";
    for (unsigned char character : text)
    {
        if (character == '"' || character == '\\')
        {
            result += '\\';
            result += static_cast<char>(character);
        }
        else if (character < 0x20)
        {
            char escaped[8];
            std::snprintf(escaped, sizeof(escaped), "\\u%04x", character);
            result += escaped;
        }
        else
            result += static_cast<char>(character);
    }
    return result + "\"";
}

std::string ActivityJson(const std::string& details, long long startTime, unsigned int nonce)
{
    std::string activity = "{\"timestamps\":{\"start\":" + std::to_string(startTime) + "}";
    if (!details.empty())
        activity += ",\"details\":" + JsonString(details);
    activity += "}";
    return "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + std::to_string(PcProcessId()) + ",\"activity\":" + activity
           + "},\"nonce\":\"" + std::to_string(nonce) + "\"}";
}

void Run(DiscordState* state)
{
    PcDiscordPipe pipe;
    bool connected = false;
    bool announced = false;
    unsigned int sentRevision = 0;
    unsigned int nonce = 0;
    bool first = true;
    for (;;)
    {
        std::string details;
        unsigned int revision;
        {
            std::unique_lock<std::mutex> lock(state->mutex);
            if (!first)
                state->changed.wait_for(lock, std::chrono::seconds(connected ? 2 : kRetrySeconds),
                                        [&] { return connected && state->revision != sentRevision; });
            first = false;
            details = state->details;
            revision = state->revision;
        }

        if (!connected)
        {
            if (!pipe.Open())
                continue;
            if (!Send(pipe, kOpHandshake, "{\"v\":1,\"client_id\":" + JsonString(state->applicationId) + "}"))
            {
                pipe.Close();
                continue;
            }
            connected = true;
            sentRevision = revision - 1;
            if (!announced)
                LOG_INFO(sLog, "Connected to Discord");
            announced = true;
        }

        if (!pipe.Drain())
        {
            pipe.Close();
            connected = false;
            continue;
        }
        if (revision != sentRevision)
        {
            if (!Send(pipe, kOpFrame, ActivityJson(details, state->startTime, ++nonce)))
            {
                pipe.Close();
                connected = false;
                continue;
            }
            sentRevision = revision;
        }
    }
}
}

void PcDiscordStart(const std::string& applicationId)
{
    if (sState != NULL || applicationId.empty())
        return;
    sState = new DiscordState;
    sState->applicationId = applicationId;
    sState->startTime = static_cast<long long>(std::time(NULL));
    std::thread(Run, sState).detach();
}

void PcDiscordSetPresence(const std::wstring& details)
{
    if (sState == NULL)
        return;
    std::string text = PcWideToUtf8(details);
    if (text.size() > kMaxDetailsLength)
    {
        size_t length = kMaxDetailsLength;
        while (length > 0 && (static_cast<unsigned char>(text[length]) & 0xc0) == 0x80)
            --length;
        text.resize(length);
    }
    std::lock_guard<std::mutex> lock(sState->mutex);
    if (text == sState->details)
        return;
    sState->details = text;
    ++sState->revision;
    sState->changed.notify_one();
}
