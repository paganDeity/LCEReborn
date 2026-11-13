#include "game/data/McRegionLevelStorage.hpp"

#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/CMinecraftApp.hpp"
#include "game/core/RegionFileCache.hpp"
#include "game/data/ConsoleSaveFile.hpp"
#include "game/data/DataFixerUpper.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/LevelData.hpp"
#include "game/world/NetherDimension.hpp"
#include "game/world/TheEndDimension.hpp"
#include "game/world/chunk/McRegionChunkStorage.hpp"
#include "game/core/MemSect.hpp"
#include "game/core/ModHooks.hpp"
#include "game/world/DimensionType.hpp"
#include "game/world/chunk/ScriptedChunkStorage.hpp"

McRegionLevelStorage::McRegionLevelStorage(ConsoleSaveFile* saveFile, File folder, const std::wstring& levelId,
                                           bool createIfMissing, DataFixerUpper* fixer)
    : DirectoryLevelStorage(saveFile, folder, levelId, createIfMissing, fixer)
{
    gRegionFileCache._clear();
}

McRegionLevelStorage::~McRegionLevelStorage()
{
    gRegionFileCache._clear();
}

ChunkStorage* McRegionLevelStorage::createChunkStorage(Dimension* dimension)
{
    if (ModHooks::IsActive(kModHook_chunk_store))
        return new ScriptedChunkStorage(mSaveFile->getFilename(), dimension != NULL ? dimension->getType()->getId() : 0,
                                        mFixer);

    if (dimension != NULL && dynamic_cast<NetherDimension*>(dimension) != NULL)
    {
        if (gApp.GetResetNether() == 0)
            return new McRegionChunkStorage(mSaveFile, NETHER_FOLDER, mFixer);

        std::vector<FileEntry*>* netherFiles = mSaveFile->getFilesWithPrefix(NETHER_FOLDER);
        if (netherFiles != NULL)
        {
            std::vector<FileEntry*>::iterator it;
            for (it = netherFiles->begin(); it != netherFiles->end(); ++it)
                mSaveFile->deleteFile(*it);
            delete netherFiles;
        }
        std::vector<FileEntry*>* fortressFiles = mSaveFile->getFilesWithPrefix(L"data/Fortress");
        if (fortressFiles != NULL)
        {
            std::vector<FileEntry*>::iterator it;
            for (it = fortressFiles->begin(); it != fortressFiles->end(); ++it)
                mSaveFile->deleteFile(*it);
            delete fortressFiles;
        }
        resetNetherPlayerPositions();
        return new McRegionChunkStorage(mSaveFile, NETHER_FOLDER, mFixer);
    }

    if (dimension != NULL && dynamic_cast<TheEndDimension*>(dimension) != NULL)
    {
        if (gApp.GetResetEnd() == 0)
        {
            short saveVersion = mSaveFile->getSaveVersion();
            if (saveVersion == 0 || saveVersion >= 4)
                return new McRegionChunkStorage(mSaveFile, ENDER_FOLDER, mFixer);
        }

        std::vector<FileEntry*>* endFiles = mSaveFile->getFilesWithPrefix(ENDER_FOLDER);
        if (endFiles != NULL)
        {
            std::vector<FileEntry*>::iterator it;
            for (it = endFiles->begin(); it != endFiles->end(); ++it)
                mSaveFile->deleteFile(*it);
            delete endFiles;
        }
        resetEndPlayerPositions();
        return new McRegionChunkStorage(mSaveFile, ENDER_FOLDER, mFixer);
    }

    if (dimension != NULL && dimension->getType()->isModded())
        return new McRegionChunkStorage(mSaveFile, L"DIM" + std::to_wstring(dimension->getType()->getId()) + L"/", mFixer);

    return new McRegionChunkStorage(mSaveFile, L"", mFixer);
}

void McRegionLevelStorage::saveLevelData(LevelData* levelData, std::vector<boost::shared_ptr<Player> >* players)
{
    levelData->setVersion(19132);
    MemSect(38);
    DirectoryLevelStorage::saveLevelData(levelData, players);
    MemSect(0);
}

void McRegionLevelStorage::closeAll()
{
    gRegionFileCache._clear();
}
