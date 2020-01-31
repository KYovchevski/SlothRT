#pragma once
#include "Hitable.h"

#include <vector>

const unsigned int g_NumHitablesPerNode = 1;

class BVHNode : public  Hitable
{
public:

    BVHNode() {};

    virtual void Construct(std::vector<Hitable*> a_Hitables) = 0;
    virtual void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) = 0;


    void SetTransform(mat4 a_Transform);

protected:

};

