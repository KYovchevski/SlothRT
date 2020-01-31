#include "precomp.h"
#include "Scene.h"
#include "AxisAllignedBox.h"
#include "BVHBranch.h"
#include "BVHLeaf.h"

Color Scene::TraceRay(Ray& a_Ray)
{

    Color pixel;



    float dist = std::numeric_limits<float>::max();
    Hitable* hit = nullptr;

    float3 origin = a_Ray.m_Origin;
    if (origin.x == 240 & origin.y == 240)
    {
        std::cout << "k" << std::endl;
    }

    hit = m_BVHRoot->Intersect(a_Ray, dist);



    if (hit)
    {
        float3 hitPoint = a_Ray.m_Origin + a_Ray.m_Direction * dist;

        float3 normal = hit->GetDataAtIntersection(hitPoint);
        pixel.r = static_cast<uint8_t>((normal.x * 0.5f + 0.5f) * 255.0f);
        pixel.g = static_cast<uint8_t>((normal.y * 0.5f + 0.5f) * 255.0f);
        pixel.b = static_cast<uint8_t>((normal.z * 0.5f + 0.5f) * 255.0f);
        pixel.a = 255;
    }
    else
    {
        pixel.rgb[0] = 0;
        pixel.rgb[1] = 0;
        pixel.rgb[2] = 0;
        pixel.rgb[3] = 0;
            
    }

    return pixel;
}

void Scene::ConstructBvh()
{
    if (m_Hitables.size() <= g_NumHitablesPerNode)
        m_BVHRoot = std::make_unique<BVHLeaf>();
    else
        m_BVHRoot = std::make_unique<BVHBranch>();

    m_BVHRoot->Construct(m_Hitables);
}

void Scene::AddHitable(Hitable& a_Triangle)
{
    m_Hitables.push_back(&a_Triangle);
}
