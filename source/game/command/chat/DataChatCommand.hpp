#ifndef GAME_COMMAND_CHAT_DATACHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_DATACHATCOMMAND_HPP

#include <string>

#include <boost/shared_ptr.hpp>

#include "game/command/ChatCommand.hpp"

class BlockEntity;
class BlockPos;
class CompoundTag;
class NbtSelection;
class Tag;

class DataChatCommand : public ChatCommand
{
public:
    DataChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);

private:
    // what the data lives in: a block entity, an entity or the command storage, and the word after its arguments
    struct Holder
    {
        Holder() : next(0), usage(false) {}
        CompoundTag* read() const;
        void write(CompoundTag* tag) const;

        boost::shared_ptr<BlockEntity> blockEntity;
        EntityPtr entity;
        std::wstring storageId;
        std::wstring subject;
        size_t next;
        // the words did not name a holder, so the usage is shown
        bool usage;
    };

    bool resolveHolder(const CommandSource& source, const Words& words, size_t kindIndex, Holder* holder);
    void report(const CommandSource& source, const std::wstring& subject, CompoundTag* root,
                const std::wstring& path, bool hasPath);
    bool resolveBlock(const CommandSource& source, const Words& words, size_t index, BlockPos* pos,
                      boost::shared_ptr<BlockEntity>* blockEntity, std::wstring* subject);
    bool resolveEntity(const CommandSource& source, const std::wstring& selector, EntityPtr* entity,
                       std::wstring* subject);
    bool removeFrom(const CommandSource& source, const Holder& holder, CompoundTag* current, const std::wstring& path);
    bool modifyFrom(const CommandSource& source, const Holder& holder, CompoundTag* current, const std::wstring& path,
                    const Words& words, size_t index);
    bool resolveModifySource(const CommandSource& source, const Words& words, size_t kindIndex, CompoundTag** root,
                             NbtSelection* values, std::wstring* error);
};

#endif
