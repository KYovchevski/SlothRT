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
        pixel = hit->GetColor();
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
    if (m_Hitables.size() < g_NumHitablesPerNode)
        m_BVHRoot = std::make_unique<BVHLeaf>();
    else
        m_BVHRoot = std::make_unique<BVHBranch>();

    m_BVHRoot->Construct(m_Hitables);
}

void Scene::AddHitable(Hitable& a_Triangle)
{
    m_Hitables.push_back(&a_Triangle);
}
