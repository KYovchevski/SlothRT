#pragma once
#include "Hitable.h"

struct Mesh;
class BVHLeaf;

class MeshInstance :
    public Hitable
{
public:
    MeshInstance(Mesh& a_RefMesh);

    void SetParent(BVHNode* a_Parent) override;

    Intersection Intersect(Ray& a_Ray, float& a_Dist) override;

    bool ShadowRayIntersect(Ray& a_Ray, float a_MaxDist) override;

    void SetTransform(mat4 a_Transform) override;

    void CalculateBoundingBox(mat4 a_Transform) override;

    Color GetColor() const override { return Color(255, 0, 255, 255); };

private:

    BVHNode* m_BVHParent;

    Mesh* m_MeshReference;
};

