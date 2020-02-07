#include "precomp.h"
#include "MeshInstance.h"

#include "ModelLoader.h"
#include "BVHLeaf.h"
#include "AxisAllignedBox.h"

MeshInstance::MeshInstance(Mesh& a_RefMesh)
    : m_MeshReference(&a_RefMesh)
{
    m_Box = std::make_unique<AxisAllignedBox>();

    Hitable::CalculateBoundingBox();
}

void MeshInstance::SetParent(BVHNode* a_Parent)
{
    m_BVHParent = a_Parent;
}

Intersection MeshInstance::Intersect(Ray& a_Ray, float& a_Dist)
{
    Ray transformedRay = a_Ray;
    transformedRay.Transform(m_InverseTransform);

    Intersection hit = m_MeshReference->m_BVHRoot->Intersect(transformedRay, a_Dist);

    hit.m_Transform = &m_Transform;
    hit.m_InverseTransform = &m_InverseTransform;

    return hit;
}

bool MeshInstance::ShadowRayIntersect(Ray& a_Ray, float a_MaxDist)
{
    Ray transformedRay = a_Ray;
    transformedRay.Transform(m_InverseTransform);

    return m_MeshReference->m_BVHRoot->ShadowRayIntersect(transformedRay, a_MaxDist);
}

void MeshInstance::SetTransform(mat4 a_Transform)
{
    Hitable::SetTransform(a_Transform);

    Hitable::CalculateBoundingBox();
    m_BVHParent->Refit();
}

void MeshInstance::CalculateBoundingBox(mat4 a_Transform)
{

    union
    {
        struct
        {
            float3 newMin;
            float padding;
            float3 newMax;
            float padding1;
        };
        struct
        {
            __m128 newF4Min, newF4Max;
        };
    };

    newMin = m_MeshReference->m_BVHRoot->GetBoundingBox()->GetMinExtent();
    newMax = m_MeshReference->m_BVHRoot->GetBoundingBox()->GetMaxExtent();

    newMin = a_Transform.TransformPoint(newMin);
    newMax = a_Transform.TransformPoint(newMax);

    m_Box->SetExtents(newF4Min, newF4Max);
}
