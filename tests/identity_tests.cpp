#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include "identity/IdArchive.hpp"
#include "identity/IdFiles.hpp"
#include "identity/IdLock.hpp"
#include "identity/IdProfile.hpp"
#include "identity/IdStore.hpp"
#include "identity/IdUuid.hpp"
#include "p2p_check.hpp"

using namespace identity;

namespace
{
std::vector<uint8_t> FakePng(size_t extra)
{
    const uint8_t head[8] = {0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
    std::vector<uint8_t> png(head, head + 8);
    for (size_t index = 0; index < extra; ++index)
        png.push_back(static_cast<uint8_t>(index * 31));
    return png;
}

std::string Directory()
{
    char path[64];
    snprintf(path, sizeof(path), "/tmp/lce-identity-test-%d", 4242);
    return path;
}
}

int RunIdentityTests()
{
    const int before = CheckFailures();

    {
        Archive archive;
        archive.Set("a.txt", std::vector<uint8_t>(3, 'a'));
        archive.Set("empty", std::vector<uint8_t>());
        archive.Set("big.bin", std::vector<uint8_t>(100000, 7));
        const std::vector<uint8_t> bytes = archive.Write();
        Archive read;
        std::vector<uint8_t> got;
        Check(Archive::Read(bytes, read) && read.Count() == 3 && read.Get("a.txt", got) && got.size() == 3
                  && read.Get("big.bin", got) && got.size() == 100000 && read.Get("empty", got) && got.empty(),
              "an archive reads back what was written");
        std::vector<uint8_t> broken = bytes;
        broken[40] ^= 1;
        Check(!Archive::Read(broken, read), "a changed byte fails the checksum");
        broken = bytes;
        broken.resize(broken.size() - 5);
        Check(!Archive::Read(broken, read), "a truncated archive is refused");
        {
            const int first = Profile::DefaultSkin("Anto248z");
            bool inRange = true;
            for (int index = 0; index < 200; ++index)
            {
                const int skin = Profile::DefaultSkin(std::string("player") + static_cast<char>('a' + index % 26)
                                                      + static_cast<char>('a' + index / 26));
                inRange = inRange && skin >= 1 && skin <= 18;
            }
            Check(first == 18 && first == Profile::DefaultSkin("anto248Z") && inRange,
                  "the default skin follows the name, ignores case and stays in 1 to 18");
        }
        Check(!Archive::ValidName("../x") && !Archive::ValidName("a/b") && !Archive::ValidName(".hidden")
                  && !Archive::ValidName("") && Archive::ValidName("skin.png"),
              "entry names cannot climb out of the archive");
        FILE* file = fopen("/tmp/lce-identity-test.zip", "wb");
        fwrite(&bytes[0], 1, bytes.size(), file);
        fclose(file);
    }

    Profile profile;
    Check(Profile::Generate("Steve_01", profile), "an identity is generated");
    Check(!Profile::Generate("no spaces", profile) && !Profile::Generate("ab", profile) && !Profile::Generate("", profile),
          "a username must be 3 to 16 letters, digits and underscores");
    Profile::Generate("Steve_01", profile);
    {
        const std::string uuid = profile.Uuid();
        Check(uuid.size() == 32 && uuid[12] == '8', "the UUID is a version 8 UUID");
        Profile other;
        Profile::Generate("Steve_01", other);
        Check(other.Uuid() != uuid, "another key gives another UUID");
        uint8_t first[16], second[16];
        Uuid::FromKey(profile.Key().Public(), first);
        Uuid::FromKey(profile.Key().Public(), second);
        Check(memcmp(first, second, 16) == 0 && Uuid::Hex(first) == uuid, "a UUID comes from its key alone");
    }
    {
        std::string error;
        Profile loaded;
        Check(Profile::Load(profile.Save(), loaded, &error) && loaded.Name() == "Steve_01"
                  && loaded.Uuid() == profile.Uuid() && loaded.Key().Public() == profile.Key().Public()
                  && !loaded.HasBundledSkin() && loaded.SkinName().empty(),
              "an identity file loads back to the same identity");
        profile.SetNamedSkin("dlcskin00000010.png");
        Check(Profile::Load(profile.Save(), loaded, &error) && loaded.SkinName() == "dlcskin00000010.png",
              "a skin chosen by name is kept");
        Check(profile.SetBundledSkin(FakePng(200)) && profile.SkinName().empty() && !profile.SkinHash().empty(),
              "a bundled skin replaces the named one and has a hash");
        Check(Profile::Load(profile.Save(), loaded, &error) && loaded.HasBundledSkin()
                  && loaded.SkinData() == profile.SkinData() && loaded.SkinHash() == profile.SkinHash(),
              "a bundled skin and its hash are kept");
        Check(!profile.SetBundledSkin(std::vector<uint8_t>(100, 1)) && !profile.SetBundledSkin(FakePng(2 * 1024 * 1024)),
              "a skin must be a PNG of reasonable size");

        std::vector<uint8_t> file = profile.Save();
        Archive archive;
        Archive::Read(file, archive);
        std::vector<uint8_t> skin;
        archive.Get("skin.png", skin);
        skin[20] ^= 1;
        archive.Set("skin.png", skin);
        Check(!Profile::Load(archive.Write(), loaded, &error), "a skin that does not match its hash is refused");

        Archive::Read(file, archive);
        std::vector<uint8_t> manifest;
        archive.Get("identity.txt", manifest);
        std::string text(manifest.begin(), manifest.end());
        const size_t at = text.find("Steve_01");
        text.replace(at, 8, "Alex_001");
        archive.Set("identity.txt", std::vector<uint8_t>(text.begin(), text.end()));
        Check(!Profile::Load(archive.Write(), loaded, &error), "a changed username breaks the signature");

        Archive::Read(file, archive);
        Profile other;
        Profile::Generate("Mallory1", other);
        Archive otherArchive;
        Archive::Read(other.Save(), otherArchive);
        std::vector<uint8_t> otherKey;
        otherArchive.Get("identity.key", otherKey);
        archive.Set("identity.key", otherKey);
        Check(!Profile::Load(archive.Write(), loaded, &error), "a key from another identity is refused");
        Check(!Profile::Load(std::vector<uint8_t>(50, 3), loaded, &error), "random bytes are not an identity");
    }

    {
        const std::string directory = Directory();
        Files::MakeDirectory(directory);
        Files::Remove(directory + "/active");
        Store store(directory);
        const std::vector<std::string> old = store.List();
        for (size_t index = 0; index < old.size(); ++index)
            store.Remove(old[index]);
        Check(store.List().empty(), "a new store is empty");
        std::string first, second;
        Profile created;
        Check(store.Create("Player_A", &first, &created) && store.Create("Player_A", &second, NULL),
              "identities are created");
        Check(first == "Player_A" && second == "Player_A-2" && store.List().size() == 2, "a taken file name gets a number");
        Profile loaded;
        std::string error;
        Check(store.Load(first, loaded, &error) && loaded.Uuid() == created.Uuid(), "a stored identity loads");
        Check(store.Active().empty() && store.SetActive(second) && store.Active() == second, "the active identity is remembered");
        loaded.SetName("Renamed_A");
        Check(store.Save(first, loaded), "an identity is saved");
        Profile again;
        Check(store.Load(first, again, &error) && again.Name() == "Renamed_A" && again.Uuid() == created.Uuid(),
              "renaming keeps the key and the UUID");
        Check(!store.Load("../x", again, &error) && !Store::ValidStem("a/b"), "a file name cannot leave the folder");
        store.Remove(first);
        store.Remove(second);
        Check(store.List().empty(), "identities are removed");
    }

    {
        Lock first, second;
        const std::string path = Directory() + "/probe.lock";
        Check(first.Acquire(path) && !second.Acquire(path), "a second holder of the same identity is refused");
        first.Release();
        Check(second.Acquire(path), "the identity is free again once released");
    }
    return CheckFailures() - before;
}
