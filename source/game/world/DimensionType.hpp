#ifndef GAME_WORLD_DIMENSIONTYPE_HPP
#define GAME_WORLD_DIMENSIONTYPE_HPP

#include <string>

class Dimension;

class DimensionType
{
public:
    static const DimensionType* OVERWORLD;
    static const DimensionType* NETHER;
    static const DimensionType* THE_END;

    // the most dimensions the game keeps levels for at once; the three of the game come first, then the mods'
    static const int kMaxDimensions = 8;

    DimensionType(int id, const std::wstring& name, const std::wstring& fileSuffix);

    int getId() const;
    // the place of the type in the order they were made: 0 overworld, 1 nether, 2 end, then each mod's
    int getIndex() const;
    bool isModded() const;
    // where the client keeps and renders the level: a mod's dimension takes the overworld's place, since a player is in one at a time
    int getClientSlot() const;
    std::wstring getName() const;
    std::wstring getFileSuffix() const;

    Dimension* create() const;

    static const DimensionType* getById(int id);
    static const DimensionType* getByName(const std::wstring& name);
    // a dimension of a mod; NULL when the number or the name is taken, is not usable or there is no room
    static const DimensionType* Register(int id, const std::wstring& name);
    static int count();
    static const DimensionType* at(int index);

private:
    int id;
    int index;
    std::wstring name;
    std::wstring fileSuffix;
};

#endif
