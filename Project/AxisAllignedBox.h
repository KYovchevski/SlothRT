#pragma once
#include "Structs.h"

#include <xmmintrin.h>

class AxisAllignedBox// : public Hitable
{
public:

    
    AxisAllignedBox(float3 a_Min = { 0.0f, 0.0f, 0.0f }, float3 a_Max = { 0.0f, 0.0f, 0.0f });
    ~AxisAllignedBox() {}

    bool Intersect(Ray& a_Ray, float& a_Dist) const;
    bool ContainsPoint(float3 a_Point) const;

    void SetExtents(__m128 a_NewMin, __m128 a_NewMax) {
        m_F4Min = a_NewMin, m_F4Max = a_NewMax; m_F4Center = _mm_mul_ps(_mm_add_ps(m_F4Min, m_F4Min), _mm_set_ps(0.5f, 0.5f, 0.5f, 0.5f));
    };

    float3 GetMinExtent() const { return m_Min; };
    float3 GetMaxExtent() const { return m_Max; };


    __m128 GetF4MinExtent() const { return m_F4Min; };
    __m128 GetF4MaxExtent() const { return m_F4Max; };


    float3 GetCenter() const { return m_Center; }

private:

    union
    {
        float3 m_Min;
        __m128 m_F4Min;
    };

    
    union
    {
        float3 m_Max;
        __m128 m_F4Max;
    };


    union
    {
        float3 m_Center;
        __m128 m_F4Center;
    };

};

