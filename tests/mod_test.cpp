#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <utility>

#include "zlib.h"

#include "archive/ArchiveFiles.hpp"
#include "mod/ModApi.hpp"
#include "mod/ModManager.hpp"
#include "mod/ModManifest.hpp"
#include "mod/ModValue.hpp"

using namespace mod;

namespace
{
int failures = 0;


void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}

const uint8_t kHookWasm[] = {
    0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x1b, 0x05, 0x60,
    0x02, 0x7f, 0x7f, 0x00, 0x60, 0x03, 0x7f, 0x7f, 0x7f, 0x00, 0x60, 0x01,
    0x7f, 0x01, 0x7f, 0x60, 0x00, 0x00, 0x60, 0x03, 0x7f, 0x7f, 0x7f, 0x01,
    0x7f, 0x02, 0x24, 0x02, 0x03, 0x6c, 0x63, 0x65, 0x0d, 0x68, 0x6f, 0x6f,
    0x6b, 0x5f, 0x72, 0x65, 0x67, 0x69, 0x73, 0x74, 0x65, 0x72, 0x00, 0x00,
    0x03, 0x6c, 0x63, 0x65, 0x08, 0x68, 0x6f, 0x6f, 0x6b, 0x5f, 0x73, 0x65,
    0x74, 0x00, 0x01, 0x03, 0x04, 0x03, 0x02, 0x03, 0x04, 0x05, 0x03, 0x01,
    0x00, 0x01, 0x07, 0x2c, 0x04, 0x06, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79,
    0x02, 0x00, 0x09, 0x6c, 0x63, 0x65, 0x5f, 0x61, 0x6c, 0x6c, 0x6f, 0x63,
    0x00, 0x02, 0x08, 0x6c, 0x63, 0x65, 0x5f, 0x69, 0x6e, 0x69, 0x74, 0x00,
    0x03, 0x08, 0x6c, 0x63, 0x65, 0x5f, 0x68, 0x6f, 0x6f, 0x6b, 0x00, 0x04,
    0x0a, 0x1e, 0x03, 0x05, 0x00, 0x41, 0x80, 0x10, 0x0b, 0x08, 0x00, 0x41,
    0x02, 0x41, 0x00, 0x10, 0x00, 0x0b, 0x0d, 0x00, 0x41, 0x01, 0x41, 0xc0,
    0x00, 0x41, 0x0a, 0x10, 0x01, 0x41, 0x01, 0x0b, 0x0b, 0x11, 0x01, 0x00,
    0x41, 0xc0, 0x00, 0x0b, 0x0a, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x58, 0x40
};

// The module registers its hook by number (lce_init passes it to hook_register) and the number of a hook moves whenever the API gains
// one, so the number is written into a copy of the module, with the sizes of the function and the code section kept right.
std::string HookWasmFor(int hook)
{
    std::vector<uint8_t> bytes(kHookWasm, kHookWasm + sizeof(kHookWasm));
    // the constant is `i32.const 2`, then the priority `i32.const 0`, then `call hook_register`
    static const uint8_t pattern[] = {0x41, 0x02, 0x41, 0x00, 0x10, 0x00};
    size_t at = 0;
    for (; at + sizeof(pattern) <= bytes.size(); ++at)
    {
        if (memcmp(&bytes[at], pattern, sizeof(pattern)) == 0)
            break;
    }
    std::vector<uint8_t> leb;
    int value = hook;
    for (;;)
    {
        const uint8_t low = value & 0x7f;
        value >>= 7;
        const bool done = (value == 0 && (low & 0x40) == 0);
        leb.push_back(done ? low : (low | 0x80));
        if (done)
            break;
    }
    const size_t extra = leb.size() - 1;
    bytes.erase(bytes.begin() + at + 1);
    bytes.insert(bytes.begin() + at + 1, leb.begin(), leb.end());
    bytes[at - 2] = static_cast<uint8_t>(bytes[at - 2] + extra);
    for (size_t index = 0; index + 2 < bytes.size(); ++index)
    {
        if (bytes[index] == 0x0a && bytes[index + 1] == 0x1e && bytes[index + 2] == 0x03)
        {
            bytes[index + 1] = static_cast<uint8_t>(bytes[index + 1] + extra);
            break;
        }
    }
    // lce_hook sets argument 1, the amount; the damage of the hurt hook is argument 2
    static const uint8_t setter[] = {0x41, 0x01, 0x41, 0xc0, 0x00, 0x41, 0x0a, 0x10, 0x01};
    for (size_t index = 0; index + sizeof(setter) <= bytes.size(); ++index)
    {
        if (memcmp(&bytes[index], setter, sizeof(setter)) == 0)
        {
            bytes[index + 1] = 0x02;
            break;
        }
    }
    return std::string(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
}

const uint8_t kWasm[] = {
    0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x14, 0x04, 0x60,
    0x02, 0x7f, 0x7f, 0x00, 0x60, 0x01, 0x7f, 0x01, 0x7f, 0x60, 0x00, 0x00,
    0x60, 0x02, 0x7f, 0x7f, 0x01, 0x7f, 0x02, 0x25, 0x03, 0x03, 0x6c, 0x63,
    0x65, 0x03, 0x6c, 0x6f, 0x67, 0x00, 0x00, 0x03, 0x6c, 0x63, 0x65, 0x05,
    0x72, 0x65, 0x70, 0x6c, 0x79, 0x00, 0x00, 0x03, 0x6c, 0x63, 0x65, 0x07,
    0x63, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64, 0x00, 0x00, 0x03, 0x04, 0x03,
    0x01, 0x02, 0x03, 0x05, 0x03, 0x01, 0x00, 0x01, 0x07, 0x2f, 0x04, 0x06,
    0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x02, 0x00, 0x09, 0x6c, 0x63, 0x65,
    0x5f, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x00, 0x03, 0x08, 0x6c, 0x63, 0x65,
    0x5f, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x04, 0x0b, 0x6c, 0x63, 0x65, 0x5f,
    0x63, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64, 0x00, 0x05, 0x0a, 0x21, 0x03,
    0x05, 0x00, 0x41, 0x80, 0x10, 0x0b, 0x0e, 0x00, 0x41, 0x10, 0x41, 0x0a,
    0x10, 0x00, 0x41, 0x20, 0x41, 0x04, 0x10, 0x02, 0x0b, 0x0a, 0x00, 0x41,
    0x30, 0x41, 0x04, 0x10, 0x01, 0x41, 0x00, 0x0b, 0x0b, 0x22, 0x03, 0x00,
    0x41, 0x10, 0x0b, 0x0a, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x61,
    0x73, 0x6d, 0x00, 0x41, 0x20, 0x0b, 0x04, 0x70, 0x69, 0x6e, 0x67, 0x00,
    0x41, 0x30, 0x0b, 0x04, 0x70, 0x6f, 0x6e, 0x67
};

class FakeServices : public Services
{
public:
    FakeServices() : clock(0) {}
    virtual void Log(const std::string& mod, const std::string& text) { logs.push_back(mod + ": " + text); }
    virtual void Say(const std::string& text) { said.push_back(text); }
    virtual bool Run(const std::string& line, std::string* reply)
    {
        ran.push_back(line);
        *reply = "ran " + line;
        return true;
    }
    virtual std::vector<std::string> Players() { return players; }
    virtual int64_t TimeMs() { return clock; }
    virtual bool ReadGameFile(const std::string& relative, std::string* data)
    {
        if (relative != "Common/res/original.txt")
            return false;
        *data = "original";
        return true;
    }
    virtual void Unloaded(const std::string&) {}
    virtual bool Call(const std::string&, int function, const std::vector<Value>& arguments, Value* result)
    {
        calls.push_back(function);
        if (function == api::eFunction_entity_getName && arguments.size() == 1)
        {
            *result = Value::FromString(arguments[0].Id() == 7 ? "Steve" : "Alex");
            return true;
        }
        return false;
    }

    bool Logged(const std::string& part) const
    {
        for (size_t index = 0; index < logs.size(); ++index)
            if (logs[index].find(part) != std::string::npos)
                return true;
        return false;
    }

    std::vector<std::string> logs, said, ran, players;
    std::vector<int> calls;
    int64_t clock;
};

// the examples call the game's API as they load; here a call is answered with nothing, which is enough to know the call exists and takes its arguments
class AnsweringServices : public FakeServices
{
public:
    virtual bool Call(const std::string&, int, const std::vector<Value>&, Value*) { return true; }
};

std::string StoredZip(const std::vector<std::pair<std::string, std::string> >& files)
{
    std::string data, directory;
    auto put16 = [](std::string& out, unsigned v) { out += char(v & 255); out += char(v >> 8); };
    auto put32 = [&](std::string& out, unsigned v) { put16(out, v & 0xffff); put16(out, v >> 16); };
    for (size_t i = 0; i < files.size(); ++i)
    {
        const std::string& name = files[i].first;
        const std::string& body = files[i].second;
        const unsigned crc = static_cast<unsigned>(crc32(0L, reinterpret_cast<const Bytef*>(body.data()), static_cast<uInt>(body.size())));
        const unsigned offset = static_cast<unsigned>(data.size());
        put32(data, 0x04034b50); put16(data, 20); put16(data, 0); put16(data, 0); put16(data, 0); put16(data, 0);
        put32(data, crc); put32(data, body.size()); put32(data, body.size()); put16(data, name.size()); put16(data, 0);
        data += name + body;
        put32(directory, 0x02014b50); put16(directory, 20); put16(directory, 20); put16(directory, 0); put16(directory, 0);
        put16(directory, 0); put16(directory, 0); put32(directory, crc); put32(directory, body.size()); put32(directory, body.size());
        put16(directory, name.size()); put16(directory, 0); put16(directory, 0); put16(directory, 0); put16(directory, 0);
        put32(directory, 0); put32(directory, offset);
        directory += name;
    }
    const unsigned start = static_cast<unsigned>(data.size());
    data += directory;
    put32(data, 0x06054b50); put16(data, 0); put16(data, 0); put16(data, files.size()); put16(data, files.size());
    put32(data, directory.size()); put32(data, start); put16(data, 0);
    return data;
}

void Write(const std::string& path, const std::string& text)
{
    FILE* file = fopen(path.c_str(), "wb");
    fwrite(text.data(), 1, text.size(), file);
    fclose(file);
}

void Touch(const std::string& path)
{
    struct timespec times[2];
    times[0].tv_sec = times[1].tv_sec = 2000000000;
    times[0].tv_nsec = times[1].tv_nsec = 0;
    utimensat(0, path.c_str(), times, 0);
}

std::string MakeMod(const std::string& root, const std::string& folder, const std::string& id, const std::string& entry,
                    const std::string& source)
{
    const std::string directory = root + "/" + folder;
    mkdir(directory.c_str(), 0700);
    Write(directory + "/mod.txt", "id=" + id + "\nname=" + id + " mod\nversion=1.0\napi=1\nentry=" + entry + "\n");
    Write(directory + "/" + entry, source);
    return directory;
}
}

int main()
{
    char pattern[] = "/tmp/lce-mod-test-XXXXXX";
    const std::string root = mkdtemp(pattern);

    std::vector<Value> items;
    items.push_back(Value::FromHandle(3, 70000));
    items.push_back(Value());
    std::vector<Value> inner(1, Value::FromString("deep"));
    items.push_back(Value::FromList(inner));
    std::vector<Value> values(1, Value::FromList(items));
    values.push_back(Value::FromInt(5));
    std::vector<uint8_t> packed;
    Value::Encode(values, packed);
    std::vector<Value> back;
    const bool decoded = Value::Decode(&packed[0], packed.size(), back);
    const std::vector<Value> backItems = decoded && back.size() == 2 ? back[0].Items() : std::vector<Value>();
    Check(decoded && back[0].Type() == Value::eType_List && backItems.size() == 3 && backItems[0].Kind() == 3
              && backItems[0].Id() == 70000 && backItems[1].Type() == Value::eType_Nil
              && backItems[2].Items().size() == 1 && backItems[2].Items()[0].String() == "deep" && back[1].Int() == 5,
          "a list of a handle, nothing and a list round trips, and the value after it is read in its place");
    std::vector<Value> cutShort;
    Check(!Value::Decode(&packed[0], packed.size() - 9, cutShort), "a list cut short is refused");

    Manifest manifest;
    std::string problem;
    Check(Manifest::Parse("id=hello\napi=1\nentry=main.lua\n", manifest, &problem) && manifest.name == "hello",
          "a manifest parses and the name defaults to the id");
    Check(!Manifest::Parse("id=Hello\napi=1\nentry=main.lua\n", manifest, &problem), "an id with a capital is refused");
    Check(!Manifest::Parse("id=hello\napi=2\nentry=main.lua\n", manifest, &problem), "another api version is refused");
    Check(!Manifest::Parse("id=hello\napi=1\nentry=../main.lua\n", manifest, &problem), "an entry outside the folder is refused");
    Check(!Manifest::Parse("id=hello\napi=1\nentry=main.exe\n", manifest, &problem), "an entry that is not lua or wasm is refused");

    FakeServices services;
    services.players.push_back("Alex");
    Manager manager(&services);
    manager.SetDirectory(root);

    MakeMod(root, "greeter", "greeter", "main.lua", R"(
local count = 0
mod.hook("MinecraftServer_tick", function() count = count + 1 end)
mod.hook("PlayerList_add", function(call) game.say("welcome " .. call.player:getName()) end)
mod.command("count", function(sender, args) return sender .. " ticks=" .. count .. " args=" .. args end)
mod.command("players", function() return table.concat(game.players(), ",") end)
mod.command("run", function(sender, args) local ok, reply = game.run(args); return tostring(ok) .. ":" .. reply end)
game.log("started " .. mod.id)
)");
    manager.LoadAll();
    std::vector<Manager::Status> list = manager.List();
    Check(list.size() == 1 && list[0].state == "running" && list[0].commands.size() == 3, "a lua mod loads and registers commands");
    Check(services.Logged("greeter: started greeter"), "the mod can log");

    std::vector<Value> tick;
    tick.push_back(Value::FromHandle(api::eHandle_MinecraftServer, 1));
    manager.RunHook(api::eHook_MinecraftServer_tick, tick);
    manager.RunHook(api::eHook_MinecraftServer_tick, tick);
    std::string reply;
    Check(manager.Command("count", "Alex", "x y", &reply) && reply == "Alex ticks=2 args=x y", "a command sees the mod's state");
    Check(manager.Command("players", "Alex", "", &reply) && reply == "Alex", "a mod can list the players");
    Check(manager.Command("run", "Alex", "time set day", &reply) && reply == "true:ran time set day" && services.ran.size() == 1,
          "a mod can run a command");
    Check(!manager.Command("nothing", "Alex", "", &reply), "an unknown command is not handled");

    std::vector<Value> joined;
    joined.push_back(Value::FromHandle(api::eHandle_PlayerList, 1));
    joined.push_back(Value::FromHandle(api::eHandle_entity, 7));
    manager.RunHook(api::eHook_PlayerList_add, joined);
    Check(services.said.back() == "welcome Steve", "a join event reaches the mod");

    MakeMod(root, "sandbox", "sandbox", "main.lua", R"(
local missing = {}
for _, name in ipairs({"io", "debug", "package", "dofile", "loadfile", "load", "collectgarbage"}) do
  if _G[name] ~= nil then missing[#missing + 1] = name end
end
if os.execute ~= nil or os.remove ~= nil or os.getenv ~= nil then missing[#missing + 1] = "os"end
if #missing > 0 then game.log("escape " .. table.concat(missing, ",")) end
local ok = pcall(string.rep, "x", 100 * 1024 * 1024)
game.log("rep " .. tostring(ok))
mod.command("spin", function() while true do end end)
mod.command("boom", function() error("on purpose") end)
mod.command("hoard", function() local t = {} for i = 1, 100000000 do t[i] = {i} end return "no" end)
)");
    manager.Poll(5000);
    Check(!services.Logged("escape"), "the sandbox hides io, debug, package, load and the dangerous os functions");
    Check(services.Logged("rep false"), "a huge allocation fails instead of taking the machine's memory");
    Check(manager.Command("spin", "Alex", "", &reply) && reply.find("time") != std::string::npos, "an endless loop is stopped");
    Check(manager.Command("hoard", "Alex", "", &reply) && reply.find("memory") != std::string::npos, "a mod that hoards memory is stopped");
    Check(manager.Command("boom", "Alex", "", &reply) && reply.find("on purpose") != std::string::npos, "an error is reported to the caller");
    bool failed = false;
    for (int index = 0; index < 12; ++index)
        manager.Command("boom", "Alex", "", &reply);
    list = manager.List();
    for (size_t index = 0; index < list.size(); ++index)
        failed = failed || (list[index].id == "sandbox" && list[index].state == "failed");
    Check(failed, "a mod that keeps failing is switched off");
    Check(manager.Command("count", "Alex", "", &reply), "the other mods keep working");

    MakeMod(root, "broken", "broken", "main.lua", "this is not lua (");
    MakeMod(root, "twin", "greeter", "main.lua", "game.log('twin')");
    manager.Poll(7000);
    bool broken = false, twin = false;
    list = manager.List();
    for (size_t index = 0; index < list.size(); ++index)
    {
        broken = broken || (list[index].folder == "broken" && list[index].state == "error");
        twin = twin || (list[index].folder == "twin" && list[index].state == "error" && list[index].error.find("already") != std::string::npos);
    }
    Check(broken, "a mod that does not compile is reported, not loaded");
    Check(twin, "two mods with one id are refused");

    const std::string greeter = root + "/greeter";
    Write(greeter + "/main.lua", "mod.command('count', function() return 'version two' end)");
    Touch(greeter + "/main.lua");
    Check(manager.Poll(9000), "a changed script is noticed");
    Check(manager.Command("count", "Alex", "", &reply) && reply == "version two", "the new script is running");
    Check(!manager.Poll(9100), "nothing is reloaded when nothing changed");

    Write(root + "/greeter/helper.lua", "return {value = 41}");
    Write(greeter + "/main.lua", "local h = require('helper') mod.command('count', function() return tostring(h.value + 1) end)");
    Touch(greeter + "/main.lua");
    manager.Poll(11000);
    Check(manager.Command("count", "Alex", "", &reply) && reply == "42", "a mod can require a file from its own folder");

    const std::string wasm = root + "/native";
    mkdir(wasm.c_str(), 0700);
    Write(wasm + "/mod.txt", "id=native\nname=native\nversion=1\napi=1\nentry=main.wasm\n");
    Write(wasm + "/main.wasm", std::string(reinterpret_cast<const char*>(kWasm), sizeof(kWasm)));
    manager.Poll(13000);
    list = manager.List();
    bool running = false;
    for (size_t index = 0; index < list.size(); ++index)
        running = running || (list[index].id == "native" && list[index].state == "running" && list[index].commands.size() == 1);
    Check(running, "a WebAssembly mod loads, runs lce_init and registers a command");
    Check(services.Logged("native: hello wasm"), "the WebAssembly mod can log");
    Check(manager.Command("ping", "Alex", "", &reply) && reply == "pong", "a WebAssembly command answers");

    // a module whose lce_init loops for ever: the call traps when its budget is spent instead of holding the thread
    static const unsigned char kForever[] = {0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x04, 0x01, 0x60, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00,
                                             0x07, 0x0c, 0x01, 0x08, 0x6c, 0x63, 0x65, 0x5f, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x00, 0x0a, 0x09, 0x01,
                                             0x07, 0x00, 0x03, 0x40, 0x0c, 0x00, 0x0b, 0x0b};
    const std::string endless = root + "/endless";
    mkdir(endless.c_str(), 0700);
    Write(endless + "/mod.txt", "id=endless\nname=endless\nversion=1\napi=1\nentry=main.wasm\nbudget=1\n");
    Write(endless + "/main.wasm", std::string(reinterpret_cast<const char*>(kForever), sizeof(kForever)));
    manager.Poll(14000);
    list = manager.List();
    bool trapped = false;
    for (size_t index = 0; index < list.size(); ++index)
        trapped = trapped || (list[index].id == "endless" && list[index].state == "error"
                              && list[index].error.find("instruction budget") != std::string::npos);
    Check(trapped, "a WebAssembly call that loops for ever traps when its budget is spent");

    Write(wasm + "/main.wasm", "not a module");
    Touch(wasm + "/main.wasm");
    manager.Poll(15000);
    list = manager.List();
    bool refused = false;
    for (size_t index = 0; index < list.size(); ++index)
        refused = refused || (list[index].folder == "native" && list[index].state == "error");
    Check(refused, "a WebAssembly file that is not a module is refused");


    {
        MakeMod(root, "hooker", "hooker", "main.lua", R"(
mod.hook("LivingEntity_hurt", function(call)
  if call.self:getName() == "Steve" then call.cancel = true end
  call.damage = call.damage * 2
end)
mod.hook("ServerPlayerGameMode_destroyBlock", function(call)
  if call.x == 99 then return false end
  game.log("break " .. call.x .. "," .. call.y .. "," .. call.z)
end)
mod.command("badhook", function() mod.hook("nothing_like_it", function() end) end)
)");
        manager.Poll(17000);
        Check(manager.HooksActive(api::eHook_LivingEntity_hurt) && manager.HooksActive(api::eHook_ServerPlayerGameMode_destroyBlock) && !manager.HooksActive(api::eHook_ServerPlayerGameMode_useItem),
              "a hook is active only while a mod listens to it");
        std::vector<Value> hurt;
        hurt.push_back(Value::FromHandle(api::eHandle_entity, 3));
        hurt.push_back(Value::FromHandle(api::eHandle_DamageSource, 1));
        hurt.push_back(Value::FromFloat(4.0));
        hurt.push_back(Value::FromBool(false));
        Check(manager.RunHook(api::eHook_LivingEntity_hurt, hurt) && hurt[2].Float() == 8.0, "a hook can change a mutable argument");
        hurt[0] = Value::FromHandle(api::eHandle_entity, 7);
        hurt[2] = Value::FromFloat(4.0);
        Check(!manager.RunHook(api::eHook_LivingEntity_hurt, hurt), "a hook can cancel through call.cancel, calling the host on a handle");
        std::vector<Value> dig;
        dig.push_back(Value::FromHandle(api::eHandle_ServerPlayerGameMode, 1));
        dig.push_back(Value::FromInt(1));
        dig.push_back(Value::FromInt(2));
        dig.push_back(Value::FromInt(3));
        dig.push_back(Value::FromInt(0));
        Check(manager.RunHook(api::eHook_ServerPlayerGameMode_destroyBlock, dig) && services.Logged("break 1,2,3"),
              "a hook sees its arguments");
        dig[1] = Value::FromInt(99);
        Check(!manager.RunHook(api::eHook_ServerPlayerGameMode_destroyBlock, dig), "returning false from a hook cancels it");
        Check(manager.Command("badhook", "Alex", "", &reply) && reply.find("no hook") != std::string::npos,
              "a hook name that does not exist is an error, not a silent miss");
    }

    {
        const std::string hookWasm = root + "/hookwasm";
        mkdir(hookWasm.c_str(), 0700);
        Write(hookWasm + "/mod.txt", "id=hookwasm\nname=hw\nversion=1\napi=1\nentry=main.wasm\n");
        Write(hookWasm + "/main.wasm", HookWasmFor(api::eHook_LivingEntity_hurt));
        manager.Poll(19000);
        Check(manager.HooksActive(api::eHook_LivingEntity_hurt), "a WebAssembly mod registers a hook");
        std::vector<Value> hurt;
        hurt.push_back(Value::FromHandle(api::eHandle_entity, 3));
        hurt.push_back(Value::FromHandle(api::eHandle_DamageSource, 1));
        hurt.push_back(Value::FromFloat(4.0));
        hurt.push_back(Value::FromBool(false));
        const bool allowed = manager.RunHook(api::eHook_LivingEntity_hurt, hurt);
        Check(!allowed && hurt[2].Float() == 99.0, "a WebAssembly hook cancels and sets an argument through hook_set");
    }


    {
        MakeMod(root, "lib", "lib", "main.lua", "game.log('lib ready')");
        Write(root + "/lib/mod.txt", "id=lib\nname=Lib\nversion=1.2.0\napi=1\nentry=main.lua\n");
        Write(root + "/lib/util.lua", "local u = {} function u.twice(x) return x * 2 end return u");
        MakeMod(root, "user", "user", "main.lua", "local u = require('lib:util') mod.command('twice', function(s, a) return tostring(u.twice(21)) end)");
        Write(root + "/user/mod.txt", "id=user\nname=User\nversion=1\napi=1\nentry=main.lua\ndepends=lib>=1.0\n");
        MakeMod(root, "ghostly", "ghostly", "main.lua", "game.log('never')");
        Write(root + "/ghostly/mod.txt", "id=ghostly\nname=G\nversion=1\napi=1\nentry=main.lua\ndepends=nobody\n");
        MakeMod(root, "toonew", "toonew", "main.lua", "game.log('never')");
        Write(root + "/toonew/mod.txt", "id=toonew\nname=T\nversion=1\napi=1\nentry=main.lua\ndepends=lib>=2.0\n");
        MakeMod(root, "sneaky", "sneaky", "main.lua", "local ok = pcall(require, 'lib:util') mod.command('sneak', function() return tostring(ok) end)");
        manager.Poll(21000);
        Check(manager.Command("twice", "Alex", "", &reply) && reply == "42", "a mod uses a module of a library it depends on");
        Check(manager.Command("sneak", "Alex", "", &reply) && reply == "false", "a mod cannot require a library it did not declare");
        bool missing = false, old = false;
        std::vector<Manager::Status> now = manager.List();
        for (size_t index = 0; index < now.size(); ++index)
        {
            missing = missing || (now[index].id == "ghostly" && now[index].state == "error" && now[index].error.find("needs the mod nobody") != std::string::npos);
            old = old || (now[index].id == "toonew" && now[index].state == "error" && now[index].error.find("found 1.2.0") != std::string::npos);
        }
        Check(missing, "a missing dependency is named and the mod does not start");
        Check(old, "a dependency that is too old is named with the version found");
        Write(root + "/lib/util.lua", "local u = {} function u.twice(x) return x * 3 end return u");
        Write(root + "/lib/main.lua", "game.log('lib reloaded a bit differently')");
        Touch(root + "/lib/main.lua");
        manager.Poll(23000);
        Check(manager.Command("twice", "Alex", "", &reply) && reply == "63", "changing a library reloads the mods that depend on it");
    }

    {
        MakeMod(root, "orderb", "orderb", "main.lua", "mod.hook('LivingEntity_hurt', function(c) c.damage = c.damage + 1 end, {priority = -5})");
        MakeMod(root, "ordera", "ordera", "main.lua", "mod.hook('LivingEntity_hurt', function(c) c.damage = c.damage * 10 end, {priority = 5})");
        manager.Poll(25000);
        std::vector<Value> hurt;
        hurt.push_back(Value::FromHandle(api::eHandle_entity, 3));
        hurt.push_back(Value::FromHandle(api::eHandle_DamageSource, 1));
        hurt.push_back(Value::FromFloat(4.0));
        hurt.push_back(Value::FromBool(false));
        std::vector<Manager::Status> keep = manager.List();
        (void)keep;
        manager.RunHook(api::eHook_LivingEntity_hurt, hurt);
        Check(hurt[2].Float() >= 50.0 && hurt[2].Float() < 1000.0, "hooks run by priority, not by load order");
    }

    {
        MakeMod(root, "publisher", "publisher", "main.lua", "mod.command('buy', function(s, a) local ok = mod.fire('shop:buy', s, 5) return tostring(ok) end)");
        MakeMod(root, "subscriber", "subscriber", "main.lua", "mod.listen('shop:buy', function(who, price) game.log('bought ' .. who .. ' ' .. price) if price > 3 then return false end end)");
        manager.Poll(27000);
        Check(manager.Command("buy", "Alex", "", &reply) && reply == "false" && services.Logged("bought Alex 5"),
              "a mod can fire an event another mod listens to, and the listener can cancel it");
    }

    {
        MakeMod(root, "small", "small", "main.lua", "local ok = pcall(string.rep, 'x', 3 * 1024 * 1024) mod.command('big', function() return tostring(ok) end)");
        Write(root + "/small/mod.txt", "id=small\nname=S\nversion=1\napi=1\nentry=main.lua\nmemory=1\n");
        MakeMod(root, "roomy", "roomy", "main.lua", "local ok = pcall(string.rep, 'x', 3 * 1024 * 1024) mod.command('room', function() return tostring(ok) end)");
        manager.Poll(29000);
        Check(manager.Command("big", "Alex", "", &reply) && reply == "false" && manager.Command("room", "Alex", "", &reply) && reply == "true",
              "a mod can ask for a lower memory limit than the host's default");
    }

    {
        std::vector<std::pair<std::string, std::string> > files;
        files.push_back(std::make_pair("mod.txt", "id=zipped\nname=Z\nversion=1\napi=1\nentry=main.lua\n"));
        files.push_back(std::make_pair("main.lua", "local lib = require('helper') mod.command('zipped', function() return lib end)"));
        files.push_back(std::make_pair("helper.lua", "return 'from the archive'"));
        Write(root + "/zipped.zip", StoredZip(files));
        mkdir((root + "/zipped_modData").c_str(), 0700);
        manager.Poll(31000);
        Check(manager.Command("zipped", "Alex", "", &reply) && reply == "from the archive", "a mod can be a zip archive");
        std::vector<Manager::Status> all = manager.List();
        bool strayFolder = false;
        for (size_t i = 0; i < all.size(); ++i)
            strayFolder = strayFolder || all[i].folder == "zipped_modData";
        Check(!strayFolder, "a mod's data folder is not mistaken for a mod");
    }

    {
        MakeMod(root, "writer", "writer", "main.lua", R"(
mod.command("files", function()
  local out = {}
  out[#out + 1] = tostring(file.write("assets/Common/res/new.txt", "data"))
  out[#out + 1] = tostring(file.read("assets/Common/res/new.txt"))
  out[#out + 1] = tostring(file.exists("assets/Common/res/new.txt"))
  out[#out + 1] = tostring(file.write("../escape.txt", "x"))
  out[#out + 1] = tostring(file.write("/tmp/escape.txt", "x"))
  out[#out + 1] = tostring(file.read("a/../../b"))
  out[#out + 1] = tostring(file.read_game("Common/res/original.txt"))
  out[#out + 1] = tostring(file.read_game("../secret"))
  return table.concat(out, " ")
end)
)");
        manager.Poll(33000);
        Check(manager.Command("files", "Alex", "", &reply)
                  && reply == "true data true false false nil original nil",
              "a mod writes and reads inside its own data folder only, and reads the original game files");
        std::string written;
        Check(archive::Files::Read(root + "/writer/modData/assets/Common/res/new.txt", written) && written == "data",
              "the data folder of a plain-folder mod is modData inside it");
        std::vector<std::string> roots = manager.DataDirectories();
        Check(!roots.empty() && roots.back().size() > 0, "running mods publish their data folders for the overlay");
    }

    {
        // the example mods in the documentation are the API's own users: each loads, so a hook or call that was renamed or removed shows here
        AnsweringServices exampleServices;
        Manager examples(&exampleServices);
        examples.SetDirectory(LCE_EXAMPLE_MODS);
        examples.LoadAll();
        std::vector<Manager::Status> loaded = examples.List();
        Check(!loaded.empty(), "the example mods are found");
        for (size_t index = 0; index < loaded.size(); ++index)
            Check(loaded[index].state == "running", ("the example mod " + loaded[index].folder + " loads: " + loaded[index].error).c_str());
    }

    if (const char* extra = getenv("LCE_EXTRA_MODS"))
    {
        // a folder of mods the caller made (the SDK template), each of which must load
        AnsweringServices extraServices;
        Manager more(&extraServices);
        more.SetDirectory(extra);
        more.LoadAll();
        std::vector<Manager::Status> loaded = more.List();
        Check(!loaded.empty(), "the extra mods are found");
        for (size_t index = 0; index < loaded.size(); ++index)
            Check(loaded[index].state == "running", ("the extra mod " + loaded[index].folder + " loads: " + loaded[index].error).c_str());
    }

    std::string cleanup = "rm -rf " + root;
    if (system(cleanup.c_str()) != 0)
        printf("could not remove %s\n", root.c_str());
    printf("%s\n", failures == 0 ? "all passed" : "FAILED");
    return failures == 0 ? 0 : 1;
}
