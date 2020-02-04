#pragma once
#include "Structs.h"

class Light
{
public:
    Light(float3 a_Position, float a_Attenuation, Color a_Color = Color(255, 255, 255, 255));

    float3 GetPosition() const;
    Color GetColor() const;
    float GetAttenuation() const;

    Ray GetShadowRay(float3 a_From);

private:

    float3 m_Position;
    Color m_Color;
    float m_Attenuation;
};

