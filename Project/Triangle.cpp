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

    Hitable::CalculateBoundingBox();
}

float3 Triangle::GetDataAtIntersection(float3 a_IntersectionPoint)
{
    return m_Normal;
}

void Triangle::CalculateBoundingBox(mat4 a_Transform)
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

    min = { fmax, fmax, fmax };
    max = { fmin, fmin, fmin };

    for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                min.v[j] = std::min(min.v[j], points[i].v[j]);
                max.v[j] = std::max(max.v[j], points[i].v[j]);
            }
        }
    
    min = a_Transform.TransformPoint(min);
    max = a_Transform.TransformPoint(max);

    __m128 bbmin, bbmax;

    bbmin = _mm_min_ps(f4min, f4max);
    bbmax = _mm_max_ps(f4min, f4max);

    m_Box->SetExtents(bbmin, bbmax);
}


Intersection Triangle::Intersect(Ray& a_Ray, float& a_Dist)
{
    float denom = dot(a_Ray.m_Direction, m_Normal);
    float dist = std::numeric_limits<float>::lowest();

    Intersection hitIntersection;

    if (fabs(denom) > 1e-6)
    {
        float3 originToPoint = m_A - a_Ray.m_Origin;

        dist = dot(originToPoint, m_Normal) / denom;

        // if behind ray origin or further from the closest found intersection
        if (dist < 0 || dist > a_Dist)
        {
            return hitIntersection;
        }
    }
    else
    {
        return hitIntersection;
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
        hitIntersection.m_Hit = this;
        return hitIntersection;
    }
    return hitIntersection;
}

bool Triangle::ShadowRayIntersect(Ray& a_Ray, float a_MaxDist)
{
    return Intersect(a_Ray, a_MaxDist).m_Hit != nullptr;
}
