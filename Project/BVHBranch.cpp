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

            if (transform != mat4::Identity())
            {
                std::cout << "l" << std::endl;
            }

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
    {
        float dist1 = a_Dist, dist2 = a_Dist;
        Hitable* hit1 = nullptr, * hit2 = nullptr;

        hit1 = m_Left->Intersect(transformedRay, dist1);
        hit2 = m_Right->Intersect(transformedRay, dist2);

        if (dist1 < dist2)
        {
            if (hit1)
            {
                hit = hit1;
                a_Dist = dist1;
            }
        }
        else
        {
            if (hit2)
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
