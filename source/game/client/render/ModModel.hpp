#ifndef GAME_CLIENT_RENDER_MODMODEL_HPP
#define GAME_CLIENT_RENDER_MODMODEL_HPP

#include <string>
#include <vector>

#include "game/client/render/Model.hpp"

class ModelPart;

// a model of boxes a mod describes in text, drawn like the game's own: one `texture <width> <height>` line, then a `part` line for each box,
// `part <name> <x> <y> <z> <u> <v> <dx> <dy> <dz> <width> <height> <depth> [head|legA|legB]`: the part pivots at x y z (in sixteenths of a block, y down,
// 24 the ground), its box starts dx dy dz from the pivot and is width by height by depth, its picture starts at u v of the texture, and the last word
// moves it as the game's mobs move (the head turns, legA and legB swing against each other)
class ModModel : public Model
{
public:
    enum EMotion
    {
        eMotion_None,
        eMotion_Head,
        eMotion_LegA,
        eMotion_LegB
    };

    struct PartSpec
    {
        float x, y, z;
        int u, v;
        float dx, dy, dz;
        int width, height, depth;
        int motion;
    };

    // false with the line that is wrong when the text is not a model
    static bool Parse(const std::wstring& text, int* texWidth, int* texHeight, std::vector<PartSpec>* parts, std::wstring* problem);

    explicit ModModel(const std::wstring& text);

    virtual void render(boost::shared_ptr<Entity>& entity, float limbSwing, float limbSwingAmount, float ageInTicks,
                        float netHeadYaw, float headPitch, float scale, bool useDisplayList);
    virtual void setupAnim(float limbSwing, float limbSwingAmount, float ageInTicks, float netHeadYaw, float headPitch,
                           float scale, boost::shared_ptr<Entity> entity, _SkinAdjustments skinAdjustments);

private:
    std::vector<ModelPart*> m_boxes;
    std::vector<int> m_motions;
};

#endif
