#pragma once
#include "BVHNode.h"

class BVHBranch : public BVHNode
{
public:

    BVHBranch(BVHNode* a_Parent);

    void Construct(std::vector<Hitable*> a_Hitables) override;
    void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) override;

    bool Refit() override;

    void CalculateBoundingBox(mat4 a_Transform) override;

    Intersection Intersect(Ray& a_Ray, float& a_Dist) override;
    bool ShadowRayIntersect(Ray& a_Ray, float a_MaxDist) override;

    Color GetColor() const override { return Color(255, 255, 255); };

private:

    std::unique_ptr<BVHNode> m_Left;
    std::unique_ptr<BVHNode> m_Right;
};

