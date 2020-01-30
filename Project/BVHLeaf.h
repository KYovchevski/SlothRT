#pragma once
#include "BVHNode.h"

class BVHLeaf : public BVHNode
{
public:

    BVHLeaf();

    void Construct(std::vector<Hitable*> a_Hitables) override;
    void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) override;

    void CalculateBoundingBox() override;

    Hitable* Intersect(Ray& a_Ray, float& a_Dist) override;

    Color GetColor() const override { return Color(255, 0, 255, 0); };

private:

    std::vector<Hitable*> m_Hitables;
};

