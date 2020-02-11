#include "precomp.h"
#include "BVHBranch.h"
#include "BVHLeaf.h"

BVHBranch::BVHBranch(BVHNode* a_Parent)
    : BVHNode(a_Parent)
    , m_Left(nullptr)
    , m_Right(nullptr)
{
    m_InverseTransform = mat4::Identity();
}

void BVHBranch::Construct(std::vector<Hitable*> a_Hitables)
{
    float fmin = std::numeric_limits<float>::lowest();
    float fmax = std::numeric_limits<float>::max();
    float3 min{ fmax,fmax,fmax };
    float3 max{ fmin,fmin,fmin };

    for (auto hitable : a_Hitables)
    {
        float3 hmin, hmax;
        hmin = hitable->GetBoundingBox()->GetMinExtent();
        hmax = hitable->GetBoundingBox()->GetMaxExtent();

        float3 boxPoints[8];
        boxPoints[0] = { hmin.x, hmin.y, hmin.z };
        boxPoints[1] = { hmin.x, hmin.y, hmax.z };
        boxPoints[2] = { hmin.x, hmax.y, hmin.z };
        boxPoints[3] = { hmin.x, hmax.y, hmax.z };
        boxPoints[4] = { hmax.x, hmin.y, hmin.z };
        boxPoints[5] = { hmax.x, hmin.y, hmax.z };
        boxPoints[6] = { hmax.x, hmax.y, hmin.z };
        boxPoints[7] = { hmax.x, hmax.y, hmax.z };

        for (size_t j = 0; j < 8; j++)
        {
            mat4 transform = hitable->GetTransform();

            float3 point = transform.TransformPoint(boxPoints[j]);
            for (size_t i = 0; i < 3; i++)
            {
                min.v[i] = std::min(point.v[i], min.v[i]);
                max.v[i] = std::max(point.v[i], max.v[i]);
            }
        }
    }

    m_Box = std::make_unique<AxisAllignedBox>(min, max);

    float3 dimension = max - min;

    int largestDimension;
    if (dimension.x > dimension.y && dimension.x > dimension.z)
        largestDimension = 0;
    else if (dimension.y > dimension.z && dimension.y > dimension.x)
        largestDimension = 1;
    else
        largestDimension = 2;


    std::sort(a_Hitables.begin(), a_Hitables.end(), [largestDimension](Hitable*& a_Left, Hitable*& a_Right)
        {
            return a_Left->GetBoundingBox()->GetCenter().v[largestDimension] < a_Right->GetBoundingBox()->GetCenter().v[largestDimension];
        });

    float median = (min.v[largestDimension] + max.v[largestDimension]) / 2.0f;

    std::vector<Hitable*> left, right;

    for (size_t i = 0; i < a_Hitables.size(); i++)
    {
        //auto hit = a_Hitables[i];
        //
        //if (hit->GetBoundingBox()->GetCenter().v[largestDimension] > median)
        //{
        //    left.push_back(a_Hitables[i]);
        //}
        //else
        //{
        //    right.push_back(a_Hitables[i]);
        //}

        
        if (i < a_Hitables.size() / 2)
            left.push_back(a_Hitables[i]);
        else
            right.push_back(a_Hitables[i]);
    }

    if (left.empty() && right.size() > 1)
    {
        left.push_back(right.back());
        right.pop_back();
    }
    else if (right.empty() && left.size() > 1)
    {
        right.push_back(left.back());
        left.pop_back();
    }

    if (left.size() > g_NumHitablesPerNode)
        m_Left = std::make_unique<BVHBranch>(this);
    else
        m_Left = std::make_unique<BVHLeaf>(this);


    if (right.size() > g_NumHitablesPerNode)
        m_Right = std::make_unique<BVHBranch>(this);
    else
        m_Right = std::make_unique<BVHLeaf>(this);

    m_Left->Construct(left);
    m_Right->Construct(right);

    Hitable::CalculateBoundingBox();
}

void BVHBranch::Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables)
{
    std::vector<Hitable*> hitables;

    for (auto& hitable : a_Hitables)
    {
        hitables.push_back(hitable.get());
    }

    Construct(hitables);
}

bool BVHBranch::Refit()
{
    __m128& currMin = m_Box->GetF4MinExtent(), & currMax = m_Box->GetF4MaxExtent();
    //__m128 newMin = currMin, newMax = currMax;


    const float fmin = std::numeric_limits<float>::lowest();
    const float fmax = std::numeric_limits<float>::max();
    __m128 newMin = _mm_set_ps(fmax, fmax, fmax, fmax);
    __m128 newMax = _mm_set_ps(fmin, fmin, fmin, fmin);

    
    newMin = _mm_min_ps(newMin, m_Left->GetBoundingBox()->GetF4MinExtent());
    newMax = _mm_max_ps(newMax, m_Left->GetBoundingBox()->GetF4MaxExtent());

    newMin = _mm_min_ps(newMin, m_Right->GetBoundingBox()->GetF4MinExtent());
    newMax = _mm_max_ps(newMax, m_Right->GetBoundingBox()->GetF4MaxExtent());

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
        if (m_Parent)
        {
            m_Parent->Refit();
        }
        return true;
    }

    return false;
}

void BVHBranch::CalculateBoundingBox(mat4 a_Transform)
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

    newF4Min = _mm_min_ps(m_Left->GetBoundingBox()->GetF4MinExtent(), m_Right->GetBoundingBox()->GetF4MinExtent());
    newF4Max = _mm_max_ps(m_Left->GetBoundingBox()->GetF4MaxExtent(), m_Right->GetBoundingBox()->GetF4MaxExtent());

    newMin = a_Transform.TransformPoint(newMin);
    newMax = a_Transform.TransformPoint(newMax);

    __m128 bbmin, bbmax;
    bbmin = _mm_min_ps(newF4Min, newF4Max);
    bbmax = _mm_max_ps(newF4Min, newF4Max);

    m_Box->SetExtents(bbmin, bbmax);
}

Intersection BVHBranch::Intersect(Ray& a_Ray, float& a_Dist)
{
    Ray leftRay = a_Ray;
    Ray rightRay = a_Ray;


    //leftRay.Transform(m_Left->GetInverseTransform());
    //rightRay.Transform(m_Right->GetInverseTransform());
    float leftDist = std::numeric_limits<float>::max();
    bool leftHit  = m_Left->GetBoundingBox()->Intersect(leftRay, leftDist);

    
    float rightDist = std::numeric_limits<float>::max();
    bool rightHit = m_Right->GetBoundingBox()->Intersect(rightRay, rightDist);

    int intersectionTests = 0;

    Intersection hit;

    switch (leftHit + rightHit)
    {
    case 2:
    {
        float dist1 = a_Dist, dist2 = a_Dist;
        Intersection hit1, hit2;

        if (leftDist < rightDist)
        {
            hit1 = m_Left->Intersect(leftRay, dist1);
            intersectionTests += hit1.m_NumBVHChecks;
            if (dist1 > rightDist)
            {
                hit2 = m_Right->Intersect(rightRay, dist2);
                intersectionTests += hit1.m_NumBVHChecks;

            }
        }
        else
        {
            hit2 = m_Right->Intersect(rightRay, dist2);
            intersectionTests += hit1.m_NumBVHChecks;
            if (dist2 > leftDist)
            {
                hit1 = m_Left->Intersect(leftRay, dist1);
                intersectionTests += hit1.m_NumBVHChecks;

            }
        }

        if (dist1 < dist2)
        {
            if (hit1.m_Hit)
            {
                hit = hit1;
                a_Dist = dist1;
            }
        }
        else
        {
            if (hit2.m_Hit)
            {
                hit = hit2;
                a_Dist = dist2;
            }
        }

        break;
    }
    case 1:
        if (leftHit)
        {
            hit = m_Left->Intersect(leftRay, a_Dist);
            intersectionTests += hit.m_NumBVHChecks;

        }
        else
        {
            hit = m_Right->Intersect(rightRay, a_Dist);
            intersectionTests += hit.m_NumBVHChecks;

        }


        break;
    case 0:
        break;
    default:
        std::cout << "how the living fuck you got here?" << std::endl;
        abort();
    }

    //if (hit.m_Hit)
    {
        hit.m_NumBVHChecks += intersectionTests;
    }

    return hit;

}

bool BVHBranch::ShadowRayIntersect(Ray& a_Ray, float a_MaxDist)
{

    Ray leftRay = a_Ray;
    Ray rightRay = a_Ray;


    //leftRay.Transform(m_Left->GetInverseTransform());
    //rightRay.Transform(m_Right->GetInverseTransform());

    float leftDist = std::numeric_limits<float>::max();
    float rightDist = std::numeric_limits<float>::max();

    bool leftHit = m_Left->GetBoundingBox()->Intersect(leftRay, leftDist);
    bool rightHit = m_Right->GetBoundingBox()->Intersect(rightRay, rightDist);

    switch (leftHit + rightHit)
    {
    case 2:
        return m_Left->ShadowRayIntersect(leftRay, a_MaxDist) || m_Right->ShadowRayIntersect(rightRay, a_MaxDist);
    case 1:
        if (leftHit)
            return m_Left->ShadowRayIntersect(leftRay, a_MaxDist);
        else
            return m_Right->ShadowRayIntersect(rightRay, a_MaxDist);
    case 0:
        return false;

    default:
        return false;
    }
}
