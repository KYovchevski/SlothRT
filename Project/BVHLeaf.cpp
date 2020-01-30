#include "precomp.h"
#include "BVHLeaf.h"

BVHLeaf::BVHLeaf()
{
}

void BVHLeaf::Construct(std::vector<Hitable*> a_Hitables)
{
    m_Hitables = a_Hitables;
    CalculateBoundingBox();
}

void BVHLeaf::Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables)
{
    std::vector<Hitable*> hitables;

    for (auto& hitable : a_Hitables)
    {
        hitables.push_back(hitable.get());
    }

    Construct(hitables);
}

void BVHLeaf::CalculateBoundingBox()
{

    float fmin = std::numeric_limits<float>::lowest();
    float fmax = std::numeric_limits<float>::max();
    float3 min{ fmax,fmax,fmax };
    float3 max{ fmin,fmin,fmin };

    for (auto hitable : m_Hitables)
    {
        float3 hmin, hmax;
        hmin = hitable->GetBoundingBox()->GetMinExtent();
        hmax = hitable->GetBoundingBox()->GetMaxExtent();

        for (size_t i = 0; i < 3; i++)
        {
            min.v[i] = std::min(hmin.v[i], min.v[i]);
            max.v[i] = std::max(hmax.v[i], max.v[i]);
        }
    }

    m_Box = std::make_unique<AxisAllignedBox>(min, max);
}

Hitable* BVHLeaf::Intersect(Ray& a_Ray, float& a_Dist)
{
    auto transformedOrigin = m_InverseTransform.TransformPoint(a_Ray.m_Origin);
    auto transformedDirection = m_InverseTransform.TransformVector(a_Ray.m_Direction);

    Ray transformedRay(transformedOrigin, transformedDirection);
    Hitable* hit = nullptr;

    for (Hitable* hitable : m_Hitables)
    {
        Hitable* newHit = hitable->Intersect(transformedRay, a_Dist);

        if (newHit)
        {
            hit = newHit;
        }
    }

    return hit;
}
