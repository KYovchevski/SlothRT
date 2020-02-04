#include "precomp.h"
#include "BVHLeaf.h"

BVHLeaf::BVHLeaf(BVHNode* a_Parent)
    : BVHNode(a_Parent)
{
}

void BVHLeaf::Construct(std::vector<Hitable*> a_Hitables)
{
    m_Hitables = a_Hitables;
    for (auto element : m_Hitables)
    {
        element->SetParent(this);
    }
    CalculateBoundingBox();
}

void BVHLeaf::Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables)
{
    std::vector<Hitable*> hitables;

    for (auto& hitable : a_Hitables)
    {
        hitables.push_back(hitable.get());
        hitable->SetParent(this);
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

bool BVHLeaf::Refit()
{
    __m128 &currMin = m_Box->GetF4MinExtent(), &currMax = m_Box->GetF4MaxExtent();
    __m128 newMin = currMin, newMax = currMax;

    for (Hitable* hitable : m_Hitables)
    {
        newMin = _mm_min_ps(newMin, hitable->GetBoundingBox()->GetF4MinExtent());
        newMax = _mm_max_ps(newMax, hitable->GetBoundingBox()->GetF4MaxExtent());
    }

    bool toRefit = false;
    for (size_t i = 0; i < 3; i++)
    {
        if (newMin.m128_f32[i] != currMin.m128_f32[i] || newMax.m128_f32[i] != currMax.m128_f32[i])
        {
            toRefit = true;
            break;
        }   
    }

    if (toRefit)
    {
        m_Box->SetExtents(newMin, newMax);
        m_Parent->Refit();
        return true;
    }

    return false;
}

Hitable* BVHLeaf::Intersect(Ray& a_Ray, float& a_Dist)
{
    Hitable* hit = nullptr;

    for (Hitable* hitable : m_Hitables)
    {
        Ray transformedRay = a_Ray;
        transformedRay.Transform(hitable->GetInverseTransform());
        Hitable* newHit = hitable->Intersect(transformedRay, a_Dist);

        if (newHit)
        {
            hit = newHit;
        }
    }

    return hit;
}

bool BVHLeaf::ShadowRayIntersect(Ray& a_Ray, float a_MaxDist)
{

    for (Hitable* hitable : m_Hitables)
    {
        Ray transformedRay = a_Ray;
        transformedRay.Transform(hitable->GetInverseTransform());
        bool hit = hitable->ShadowRayIntersect(transformedRay, a_MaxDist);

        if (hit)
        {
            return true;
        }
    }
    return false;
}
