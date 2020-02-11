#include "precomp.h"
#include "Scene.h"
#include "AxisAllignedBox.h"
#include "BVHBranch.h"
#include "BVHLeaf.h"

static unsigned int tmp = 0;

TraceInfo Scene::TraceRay(Ray& a_Ray, EMode a_Mode)
{

    Color pixel;




    float3 origin = a_Ray.m_Origin;
    float dist = std::numeric_limits<float>::max();

    TraceInfo info;
    Intersection hit = CastRay(a_Ray, dist);

    switch (a_Mode)
    {
    //case EMode::EColor:
    case EMode::ENormals:

        if (hit.m_Hit)
        {
            float3 hitPoint = a_Ray.m_Origin + a_Ray.m_Direction * dist;

            float3 normal = hit.m_Hit->GetDataAtIntersection(hitPoint);

            if (hit.m_InverseTransform != nullptr)
            {
                normal = hit.m_InverseTransform->Transposed().TransformPoint(normal);
            }

            pixel.r = static_cast<uint8_t>((normal.x * 0.5f + 0.5f) * 255.0f);
            pixel.g = static_cast<uint8_t>((normal.y * 0.5f + 0.5f) * 255.0f);
            pixel.b = static_cast<uint8_t>((normal.z * 0.5f + 0.5f) * 255.0f);
            pixel.a = 255;

            Ray shadowRay = m_Lights[0]->GetShadowRay(hitPoint);

            float shadowRayDist = length(m_Lights[0]->GetPosition() - shadowRay.m_Origin);
            float ndotl = 0.0f;
            if (!CastShadowRay(shadowRay, shadowRayDist))
            {
                ndotl = clamp(dot(normal, shadowRay.m_Direction), 0.0f, 1.0f);
            }

            pixel.r *= ndotl;
            pixel.g *= ndotl;
            pixel.b *= ndotl;

        }
        else
        {
            pixel.rgb[0] = 255;
            pixel.rgb[1] = 255;
            pixel.rgb[2] = 255;
            pixel.rgb[3] = 0;

        }

        info.m_Intersection = hit;
        info.m_Color = pixel;

        return info;

    case EMode::EBVHTests:

        if (tmp < hit.m_NumBVHChecks)
        {
            tmp = hit.m_NumBVHChecks;
            std::cout << tmp << std::endl;
        }

        uint8_t num = (hit.m_NumBVHChecks / 200000.0f) * 255;
        pixel.r = num;
        pixel.g = 255 - num;
        pixel.b = 0;

        info.m_Intersection = hit;
        info.m_Color = pixel;

        return info;

    }


}

Intersection Scene::CastRay(Ray& a_Ray, float& a_Dist)
{

    Intersection hit;
    hit = m_BVHRoot->Intersect(a_Ray, a_Dist);

    return hit;
}

bool Scene::CastShadowRay(Ray& a_Ray, float a_MaxDist)
{
    return m_BVHRoot->ShadowRayIntersect(a_Ray, a_MaxDist);
}

void Scene::ConstructBvh()
{
    if (m_Hitables.size() <= g_NumHitablesPerNode)
        m_BVHRoot = std::make_unique<BVHLeaf>(nullptr);
    else
        m_BVHRoot = std::make_unique<BVHBranch>(nullptr);

    m_BVHRoot->Construct(m_Hitables);
}


void Scene::AddHitable(Hitable& a_Triangle)
{
    m_Hitables.push_back(&a_Triangle);
}

void Scene::AddLight(Light& a_Light)
{
    m_Lights.push_back(&a_Light);
}
