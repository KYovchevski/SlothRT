#pragma once
#include "Structs.h"
#include "AxisAllignedBox.h"
#include <memory>

struct Ray;

class BVHNode;
class Hitable;

struct Intersection
{
    Intersection()
        : m_Hit(nullptr)
        , m_Transform(nullptr)
        , m_InverseTransform(nullptr)
        , m_NumBVHChecks(0){}
    Hitable* m_Hit;
    const mat4* m_Transform;
    const mat4* m_InverseTransform;
    unsigned int m_NumBVHChecks;

    Intersection& operator=(Intersection& a_Other)
    {
        m_Hit = a_Other.m_Hit;
        m_Transform = a_Other.m_Transform;
        m_InverseTransform = a_Other.m_InverseTransform;
        m_NumBVHChecks = a_Other.m_NumBVHChecks;

        return *this;
    }
};

class Hitable
{
public:
    Hitable();
    virtual void SetParent(BVHNode* a_Parent) {};

    virtual Intersection Intersect(Ray& a_Ray, float& a_Dist) = 0;
    virtual bool ShadowRayIntersect(Ray& a_Ray, float a_MaxDist) = 0;

    void CalculateBoundingBox() { CalculateBoundingBox(m_Transform); };
    virtual void CalculateBoundingBox(mat4 a_Transform) = 0;

    virtual Color GetColor() const = 0;

    const AxisAllignedBox* GetBoundingBox() const { return m_Box.get(); }

    virtual float3 GetDataAtIntersection(float3 a_IntersectionPoint);

    virtual void SetTransform(mat4 a_Transform) { m_Transform = a_Transform; m_InverseTransform = m_Transform.Inverted(); };

    mat4 GetTransform() const { return m_Transform; };
    mat4 GetInverseTransform() const { return m_InverseTransform; }

    
protected:

    mat4 m_Transform;
    mat4 m_InverseTransform;

    std::unique_ptr<AxisAllignedBox> m_Box;
};

inline Hitable::Hitable()
    : m_Transform(mat4::Identity())
    , m_InverseTransform(mat4::Identity())
    , m_Box(std::make_unique<AxisAllignedBox>())
{
    
}

inline float3 Hitable::GetDataAtIntersection(float3 a_IntersectionPoint)
{
    return float3{ 0.0f, 0.0f, 0.0f };
}

