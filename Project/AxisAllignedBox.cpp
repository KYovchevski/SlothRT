#include "precomp.h"
#include "AxisAllignedBox.h"

AxisAllignedBox::AxisAllignedBox(float3 a_Min, float3 a_Max)
    : m_Min(a_Min)
    , m_Max(a_Max)
{
    m_F4Center = _mm_add_ps(m_F4Min, m_F4Min);
    m_Center /= 2.0f;
}

bool AxisAllignedBox::Intersect(Ray& a_Ray, float& a_Dist) const
{
    float nearDist = -1;
    float farDist = std::numeric_limits<float>::max();
        
    auto& rayDir = a_Ray.m_Direction;
    auto& rayOrigin = a_Ray.m_Origin;


       
    __m128 dist1, dist2, t1, t2;
    t1 = _mm_mul_ps(_mm_sub_ps(m_F4Min, a_Ray.m_F4Origin), a_Ray.m_RF4Direction);
    t2 = _mm_mul_ps(_mm_sub_ps(m_F4Max, a_Ray.m_F4Origin), a_Ray.m_RF4Direction);

    /*for (size_t i = 0; i < 3; i++)
    {
        if (dist1.m128_f32[i] > dist2.m128_f32[i])
        {
            std::swap(dist1, dist2);
        }
    }*/

    dist1 = _mm_min_ps(t1, t2);
    dist2 = _mm_max_ps(t1, t2);

    using namespace std;

    nearDist = max(max(dist1.m128_f32[0], dist1.m128_f32[1]), dist1.m128_f32[2]);
    farDist = min(min(dist2.m128_f32[0], dist2.m128_f32[1]), dist2.m128_f32[2]);

    if (farDist < 0 || farDist < nearDist)
    {
        return false;
    }
    if (a_Dist > nearDist && (nearDist > 0 || (nearDist <= 0 && farDist > 0)))
    {
        a_Dist = nearDist;
        return true;
    }
    return false;

    // For each of the 3 axes
    for (int i = 0; i < 3; i++)
    {
        // If the ray isn't collinear, determine the enter and exit point for the ray
        if (rayDir.v[i] != 0.f)
        {
            bool isBehind = false;

            // If a new closest distance is found, create a new normal and note down the new nearest distance
            if (nearDist < dist1.m128_f32[i])
            {
                //nearDist = dist1.m128_f32[i];
            }

            //farDist = fminf(farDist, dist2.m128_f32[i]);

            // If the nearest intersection point is further from the ray origin that the furthest origin point, no intersection occurs
            if (nearDist > farDist || farDist < 0)
            {
                return false;
            }
        }
        // If the ray is collinear with the axis, check if its origin is between the two extents on this axis
        else if (rayOrigin.v[i] < m_Min.v[i] || rayOrigin.v[i] > m_Max.v[i])
        {
            return false;
        }
    }

    // If the AABB is closer than the previous closest intersection, modify the intersection data
    if (a_Dist > nearDist && (nearDist > 0 || (nearDist <= 0 && farDist > 0)))
    {
        a_Dist = nearDist;
        return true;
    }
    return false;
    
}

bool AxisAllignedBox::ContainsPoint(float3 a_Point) const
{
    for (size_t i = 0; i < 3; i++)
    {
        if (a_Point.v[i] < m_Min.v[i] || a_Point.v[i] > m_Max.v[i])
        {
            return false;
        }
    }

    return true;
}
