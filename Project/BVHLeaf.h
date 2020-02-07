#pragma once
#include "BVHNode.h"

class BVHLeaf : public BVHNode
{
public:

    BVHLeaf(BVHNode* a_Parent);

    void Construct(std::vector<Hitable*> a_Hitables) override;
    void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) override;

    void CalculateBoundingBox(mat4 a_Transform) override;

    bool Refit();

    Intersection Intersect(Ray& a_Ray, float& a_Dist) override;
    bool ShadowRayIntersect(Ray& a_Ray, float a_MaxDist) override;

    Color GetColor() const override { return Color(255, 0, 255, 0); };

private:

    std::vector<Hitable*> m_Hitables;
};

