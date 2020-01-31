#pragma once
#include "Structs.h"

class AxisAllignedBox// : public Hitable
{
public:

    AxisAllignedBox(float3 a_Min, float3 a_Max);
    ~AxisAllignedBox() {}

    bool Intersect(Ray& a_Ray, float& a_Dist) const;
    bool ContainsPoint(float3 a_Point) const;

    float3 GetMinExtent() const { return m_Min; };
    float3 GetMaxExtent() const { return m_Max; };
    float3 GetCenter() const { return m_Center; }

private:

    float3 m_Min;
    float3 m_Max;
    float3 m_Center;

};

