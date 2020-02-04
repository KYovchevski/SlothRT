#include "precomp.h"
#include "BVHNode.h"


void BVHNode::SetParent(BVHNode* a_Parent)
{
    m_Parent = a_Parent;
}

void BVHNode::SetTransform(mat4 a_Transform)
{
    std::cout << "Refit" << std::endl;
    m_Transform = a_Transform;
    m_InverseTransform = a_Transform.Inverted();
    CalculateBoundingBox();
    Refit();
}
