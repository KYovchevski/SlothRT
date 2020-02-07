#pragma once
#include "Hitable.h"

#include <vector>

const unsigned int g_NumHitablesPerNode = 1;

class BVHBranch;

class BVHNode : public  Hitable
{
public:

    BVHNode(BVHNode* a_Parent)
        : Hitable()
        , m_Parent(a_Parent) {};

    void SetParent(BVHNode* a_Parent) override;

    virtual void Construct(std::vector<Hitable*> a_Hitables) = 0;
    virtual void Construct(std::vector<std::unique_ptr<Hitable>>& a_Hitables) = 0;

    virtual bool Refit() = 0;

    void SetTransform(mat4 a_Transform);

protected:
    BVHNode* m_Parent;
};

