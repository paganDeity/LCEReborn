#include "game/client/render/ModModel.hpp"

#include <math.h>
#include <stdlib.h>
#include <wchar.h>

#include <sstream>

#include "game/client/render/ModelPart.hpp"

namespace
{
const size_t kMaxParts = 64;
const float kPi = 3.14159265f;
}

bool ModModel::Parse(const std::wstring& text, int* texWidth, int* texHeight, std::vector<PartSpec>* parts, std::wstring* problem)
{
    *texWidth = 64;
    *texHeight = 32;
    parts->clear();
    std::wistringstream lines(text);
    std::wstring line;
    while (std::getline(lines, line))
    {
        std::wistringstream words(line);
        std::wstring kind;
        if (!(words >> kind))
            continue;
        if (kind == L"texture")
        {
            if (!(words >> *texWidth >> *texHeight) || *texWidth < 1 || *texHeight < 1 || *texWidth > 1024 || *texHeight > 1024)
            {
                *problem = L"texture needs a width and a height";
                return false;
            }
            continue;
        }
        PartSpec part = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, eMotion_None};
        std::wstring name;
        if (kind != L"part" || !(words >> name >> part.x >> part.y >> part.z >> part.u >> part.v >> part.dx >> part.dy >> part.dz >> part.width
                                 >> part.height >> part.depth)
            || part.width < 1 || part.height < 1 || part.depth < 1 || parts->size() >= kMaxParts)
        {
            *problem = L"a line is not a part";
            return false;
        }
        std::wstring motion;
        if (words >> motion)
        {
            if (motion == L"head")
                part.motion = eMotion_Head;
            else if (motion == L"legA")
                part.motion = eMotion_LegA;
            else if (motion == L"legB")
                part.motion = eMotion_LegB;
            else
            {
                *problem = L"the last word of a part is head, legA or legB";
                return false;
            }
        }
        parts->push_back(part);
    }
    if (parts->empty())
    {
        *problem = L"a model has at least one part";
        return false;
    }
    return true;
}

ModModel::ModModel(const std::wstring& text)
{
    std::vector<PartSpec> specs;
    std::wstring problem;
    int width = 64;
    int height = 32;
    Parse(text, &width, &height, &specs, &problem);
    mTexWidth = width;
    mTexHeight = height;
    for (size_t index = 0; index < specs.size(); ++index)
    {
        const PartSpec& spec = specs[index];
        ModelPart* part = new ModelPart(this, spec.u, spec.v);
        part->addBox(spec.dx, spec.dy, spec.dz, spec.width, spec.height, spec.depth, 0.0f);
        part->setPos(spec.x, spec.y, spec.z);
        m_boxes.push_back(part);
        m_motions.push_back(spec.motion);
    }
}

void ModModel::setupAnim(float limbSwing, float limbSwingAmount, float ageInTicks, float netHeadYaw, float headPitch, float scale,
                         boost::shared_ptr<Entity> entity, _SkinAdjustments skinAdjustments)
{
    (void)ageInTicks;
    (void)scale;
    (void)entity;
    (void)skinAdjustments;
    for (size_t index = 0; index < m_boxes.size(); ++index)
    {
        ModelPart* part = m_boxes[index];
        switch (m_motions[index])
        {
        case eMotion_Head:
            part->mYRot = netHeadYaw / (180.0f / kPi);
            part->mXRot = headPitch / (180.0f / kPi);
            break;
        case eMotion_LegA:
            part->mXRot = cosf(limbSwing * 0.6662f) * 1.4f * limbSwingAmount;
            break;
        case eMotion_LegB:
            part->mXRot = cosf(limbSwing * 0.6662f + kPi) * 1.4f * limbSwingAmount;
            break;
        default:
            break;
        }
    }
}

void ModModel::render(boost::shared_ptr<Entity>& entity, float limbSwing, float limbSwingAmount, float ageInTicks, float netHeadYaw,
                      float headPitch, float scale, bool useDisplayList)
{
    setupAnim(limbSwing, limbSwingAmount, ageInTicks, netHeadYaw, headPitch, scale, entity, _SkinAdjustments());
    for (size_t index = 0; index < m_boxes.size(); ++index)
        m_boxes[index]->render(scale, useDisplayList, false);
}
