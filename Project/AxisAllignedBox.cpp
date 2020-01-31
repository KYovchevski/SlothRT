#include "precomp.h"
#include "AxisAllignedBox.h"

AxisAllignedBox::AxisAllignedBox(float3 a_Min, float3 a_Max)
    : m_Min(a_Min)
    , m_Max(a_Max)
    , m_Center((m_Min + m_Max) / 2.0f)
{

}

bool AxisAllignedBox::Intersect(Ray& a_Ray, float& a_Dist) const
{
    float nearDist = -1;
    float farDist = std::numeric_limits<float>::max();
    float3& rayDir = a_Ray.m_Direction;
    float3& rayOrigin = a_Ray.m_Origin;

    // For each of the 3 axes
    for (int i = 0; i < 3; i++)
    {
        // If the ray isn't collinear, determine the enter and exit point for the ray
        if (rayDir.v[i] != 0.f)
        {
            bool isBehind = false;
            float dist1, dist2;
            dist1 = (m_Min.v[i] - rayOrigin.v[i]) / rayDir.v[i];
            dist2 = (m_Max.v[i] - rayOrigin.v[i]) / rayDir.v[i];

            if (dist1 > dist2)
            {
                std::swap(dist1, dist2);
                isBehind = true;
            }

            // If a new closest distance is found, create a new normal and note down the new nearest distance
            if (nearDist < dist1)
            {
                nearDist = dist1;
            }

            nearDist = fmaxf(nearDist, dist1);
            farDist = fminf(farDist, dist2);

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
