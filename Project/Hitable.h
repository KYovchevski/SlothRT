#pragma once
#include "Structs.h"
#include "AxisAllignedBox.h"
#include <memory>

struct Ray;

class Hitable
{
public:
    Hitable();

    virtual Hitable* Intersect(Ray& a_Ray, float& a_Dist) = 0;

    virtual void CalculateBoundingBox() = 0;

    virtual Color GetColor() const = 0;

    const AxisAllignedBox* GetBoundingBox() const { return m_Box.get(); }

    virtual float3 GetDataAtIntersection(float3 a_IntersectionPoint);

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
{
    
}

inline float3 Hitable::GetDataAtIntersection(float3 a_IntersectionPoint)
{
    return float3{ 0.0f, 0.0f, 0.0f };
}

