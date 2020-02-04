#include "precomp.h"
#include "Scene.h"
#include "AxisAllignedBox.h"
#include "BVHBranch.h"
#include "BVHLeaf.h"

Color Scene::TraceRay(Ray& a_Ray)
{

    Color pixel;




    float3 origin = a_Ray.m_Origin;
    float dist = std::numeric_limits<float>::max();

    Hitable* hit = CastRay(a_Ray, dist);

    if (hit)
    {
        float3 hitPoint = a_Ray.m_Origin + a_Ray.m_Direction * dist;

        float3 normal = hit->GetDataAtIntersection(hitPoint);
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

    return pixel;
}

Hitable* Scene::CastRay(Ray& a_Ray, float& a_Dist)
{

    Hitable* hit = nullptr;
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
