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

    BVHNode::CalculateBoundingBox();
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

void BVHLeaf::CalculateBoundingBox(mat4 a_Transform)
{
    float fmin = std::numeric_limits<float>::lowest();
    float fmax = std::numeric_limits<float>::max();

    union
    {
        struct {
            float3 min;
            float padding;
            float3 max;
            float padding1;
        };

        struct
        {
            __m128 f4min, f4max;
        };
    };

    min = { fmax,fmax,fmax};
    max = { fmin,fmin,fmin };

    for (auto hitable : m_Hitables)
    {
        __m128 hmin, hmax;
        hmin = hitable->GetBoundingBox()->GetF4MinExtent();
        hmax = hitable->GetBoundingBox()->GetF4MaxExtent();

        f4min = _mm_min_ps(f4min, hmin);
        f4max = _mm_max_ps(f4max, hmax);
    }



    min = a_Transform.TransformPoint(min);
    max = a_Transform.TransformPoint(max);

    __m128 bbmin, bbmax;

    bbmin = _mm_min_ps(f4min, f4max);
    bbmax = _mm_max_ps(f4min, f4max);

    m_Box->SetExtents(bbmin, bbmax);
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

Intersection BVHLeaf::Intersect(Ray& a_Ray, float& a_Dist)
{
    Intersection hit;

    for (Hitable* hitable : m_Hitables)
    {
        Ray transformedRay = a_Ray;
        //transformedRay.Transform(hitable->GetInverseTransform());
        Intersection newHit = hitable->Intersect(transformedRay, a_Dist);

        if (newHit.m_Hit)
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
        //transformedRay.Transform(hitable->GetInverseTransform());
        bool hit = hitable->ShadowRayIntersect(transformedRay, a_MaxDist);

        if (hit)
        {
            return true;
        }
    }
    return false;
}
