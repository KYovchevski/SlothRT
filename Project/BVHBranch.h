#pragma once
#include "BVHNode.h"

class BVHBranch : public BVHNode
{
public:

    BVHBranch();

    void Construct(std::vector<Hitable*> a_Hitables) override;
    void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) override;

    void CalculateBoundingBox() override;

    Hitable* Intersect(Ray& a_Ray, float& a_Dist) override;

    Color GetColor() const override { return Color(255, 255, 255); };

private:

    std::unique_ptr<BVHNode> m_Left;
    std::unique_ptr<BVHNode> m_Right;
};

