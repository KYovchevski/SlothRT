#pragma once
#include "Structs.h"
#include "Hitable.h"

class Triangle : public Hitable 
{
public:
    Triangle(float3 a_A, float3 a_B, float3 a_C);

    void SetColor(Color a_Color) { m_Color = a_Color; }

    Color GetColor() const override final { return m_Color; }

    float3 GetDataAtIntersection(float3 a_IntersectionPoint) override;


    void CalculateBoundingBox() override;
    Hitable* Intersect(Ray& a_Ray, float& a_Dist) override final;
    bool ShadowRayIntersect(Ray& a_Ray, float a_MaxDist) override;

private:
    Color m_Color;

    union
    {
        struct
        {
            float3 m_A, m_B, m_C;
        };
        float3 points[3];
    };
    float3 m_Normal;
};

