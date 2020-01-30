#include "precomp.h"
#include "BVHBranch.h"
#include "BVHLeaf.h"

BVHBranch::BVHBranch()
    : m_Left (nullptr)
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

        for (size_t i = 0; i < 3; i++)
        {
            min.v[i] = std::min(hmin.v[i], min.v[i]);
            max.v[i] = std::max(hmax.v[i], max.v[i]);
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

    std::vector<Hitable*> left, right;

    for (size_t i = 0; i < a_Hitables.size(); i++)
    {
        if (i < a_Hitables.size() / 2)
            left.push_back(a_Hitables[i]);
        else
            right.push_back(a_Hitables[i]);
    }

    if (left.size() > g_NumHitablesPerNode)
        m_Left = std::make_unique<BVHBranch>();
    else
        m_Left = std::make_unique<BVHLeaf>();


    if (right.size() > g_NumHitablesPerNode)
        m_Right = std::make_unique<BVHBranch>();
    else
        m_Right = std::make_unique<BVHLeaf>();

    m_Left->Construct(left);
    m_Right->Construct(right);
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

void BVHBranch::CalculateBoundingBox()
{

}

Hitable* BVHBranch::Intersect(Ray& a_Ray, float& a_Dist)
{
    auto transformedOrigin = m_InverseTransform.TransformPoint(a_Ray.m_Origin);
    auto transformedDirection = m_InverseTransform.TransformVector(a_Ray.m_Direction);

    Ray transformedRay(transformedOrigin, transformedDirection);

    float leftDist = std::numeric_limits<float>::max();
    float rightDist = std::numeric_limits<float>::max();

    bool leftHit  = m_Left->GetBoundingBox()->Intersect(transformedRay, leftDist);
    bool rightHit = m_Right->GetBoundingBox()->Intersect(transformedRay, rightDist);

    Hitable* hit = nullptr;

    switch (leftHit + rightHit)
    {
    case 2:
        if (leftDist < rightDist)
        {
            hit = m_Left->Intersect(transformedRay, a_Dist);
            if (!hit)
            {
                hit = m_Right->Intersect(transformedRay, a_Dist);
            }
        }
        else
        {
            hit = m_Right->Intersect(transformedRay, a_Dist);
            if (!hit)
            {
                hit = m_Left->Intersect(transformedRay, a_Dist);
            }
        }
        break;
    case 1:
        if (leftHit)
        {
            hit = m_Left->Intersect(transformedRay, a_Dist);
        }
        else
        {
            hit = m_Right->Intersect(transformedRay, a_Dist);
        }
        break;
    case 0:
        break;
    default:
        std::cout << "how the living fuck you got here?" << std::endl;
        abort();
    }

    return hit;

}
