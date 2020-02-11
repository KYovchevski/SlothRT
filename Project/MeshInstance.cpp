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
    hit.m_NumBVHChecks++;

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

    float fmin = std::numeric_limits<float>::min(), fmax = std::numeric_limits<float>::max();

    __m128 bbmin = _mm_set_ps(fmax, fmax, fmax, fmax);
    __m128 bbmax = _mm_set_ps(fmin, fmin, fmin, fmin);
    newMin = m_MeshReference->m_BVHRoot->GetBoundingBox()->GetMinExtent();
    newMax = m_MeshReference->m_BVHRoot->GetBoundingBox()->GetMaxExtent();
    {
        float3 points[8];
        points[0] = make_float3(newMin.v[0], newMin.v[1], newMin.v[2]);
        points[1] = make_float3(newMin.v[0], newMin.v[1], newMax.v[2]);
        points[2] = make_float3(newMin.v[0], newMax.v[1], newMin.v[2]);
        points[3] = make_float3(newMin.v[0], newMax.v[1], newMax.v[2]);
        points[4] = make_float3(newMax.v[0], newMin.v[1], newMin.v[2]);
        points[5] = make_float3(newMax.v[0], newMin.v[1], newMax.v[2]);
        points[6] = make_float3(newMax.v[0], newMax.v[1], newMin.v[2]);
        points[7] = make_float3(newMax.v[0], newMax.v[1], newMax.v[2]);

        for (size_t i = 0; i < 8; i++)
        {
            union
            {
                __m128 f4;
                float3 f;
            }mn;
            mn.f = a_Transform.TransformPoint(points[i]);

            bbmin = _mm_min_ps(bbmin, mn.f4);
            bbmax = _mm_max_ps(bbmax, mn.f4);

        }
    }

    m_Box->SetExtents(bbmin,bbmax);
}
