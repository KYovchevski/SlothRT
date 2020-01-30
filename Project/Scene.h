#pragma once
#include "Structs.h"
#include "BVHNode.h"

class Scene
{
public:
    Color TraceRay(Ray& a_Ray);

    void AddHitable(Hitable& a_Triangle);

    void ConstructBvh();

private:

    std::unique_ptr<BVHNode> m_BVHRoot;

    std::vector<Hitable*> m_Hitables;
};


