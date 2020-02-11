#pragma once
#include "Structs.h"
#include "BVHNode.h"
#include "Light.h"

struct TraceInfo
{
    Intersection m_Intersection;
    Color m_Color;
};

enum EMode
{
    //EColor,
    ENormals,
    EBVHTests,

    ECount
};

class Scene
{
public:
    TraceInfo TraceRay(Ray& a_Ray, EMode a_Mode);
    Intersection CastRay(Ray& a_Ray, float& a_Dist);
    bool CastShadowRay(Ray& a_Ray, float a_MaxDist);

    void AddHitable(Hitable& a_Triangle);
    void AddLight(Light& a_Light);

    void ConstructBvh();

private:

    std::unique_ptr<BVHNode> m_BVHRoot;

    std::vector<Hitable*> m_Hitables;
    std::vector<Light*> m_Lights;
};


