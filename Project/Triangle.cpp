#include "precomp.h"
#include "Triangle.h"

Triangle::Triangle(float3 a_A, float3 a_B, float3 a_C)
    : m_A(a_A)
    , m_B(a_B)
    , m_C(a_C)
{

    m_Color.rgb[0] = 255;
    m_Color.rgb[1] = 0;
    m_Color.rgb[2] = 255;
    m_Color.rgb[3] = 255;


    float3 v1 = m_B - m_A;
    float3 v2 = m_C - m_A;

    m_Normal = normalize(cross(v1, v2));

    CalculateBoundingBox();
}

float3 Triangle::GetDataAtIntersection(float3 a_IntersectionPoint)
{
    return m_Normal;
}

void Triangle::CalculateBoundingBox()
{
    float fmin = std::numeric_limits<float>::lowest();
    float fmax = std::numeric_limits<float>::max();
    float3 min {fmax,fmax,fmax };
    float3 max {fmin,fmin,fmin };
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            min.v[j] = std::min(min.v[j], points[i].v[j]);
            max.v[j] = std::max(max.v[j], points[i].v[j]);
        }
    }

    m_Box = std::make_unique<AxisAllignedBox>(min, max);
}

Hitable* Triangle::Intersect(Ray& a_Ray, float& a_Dist)
{
    float denom = dot(a_Ray.m_Direction, m_Normal);
    float dist = std::numeric_limits<float>::lowest();

    if (fabs(denom) > 1e-6)
    {
        float3 originToPoint = m_A - a_Ray.m_Origin;

        dist = dot(originToPoint, m_Normal) / denom;

        // if behind ray origin or further from the closest found intersection
        if (dist < 0 || dist > a_Dist)
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }

    float3 planeIntersection = a_Ray.m_Origin + a_Ray.m_Direction * dist;

    float3 e1, e2, e3;
    float3 c1, c2, c3;
    float d1, d2, d3;

    float3 pAToIntersection = planeIntersection - m_A;
    float3 pBToIntersection = planeIntersection - m_B;
    float3 pCToIntersection = planeIntersection - m_C;


    e1 = m_A - m_B;
    e2 = m_B - m_C;
    e3 = m_C - m_A;

    c1 = cross(e1, pBToIntersection);
    c2 = cross(e2, pCToIntersection);
    c3 = cross(e3, pAToIntersection);

    d1 = dot(m_Normal, c1);
    d2 = dot(m_Normal, c2);
    d3 = dot(m_Normal, c3);

    if (d1 < 0 && d2 < 0 && d3 < 0)
    {
        a_Dist = dist;
        return this;
    }
    return nullptr;
}

bool Triangle::ShadowRayIntersect(Ray& a_Ray, float a_MaxDist)
{
    return Intersect(a_Ray, a_MaxDist) != nullptr;
}
