#include "precomp.h"
#include "Light.h"

Light::Light(float3 a_Position, float a_Attenuation, Color a_Color)
    : m_Position(a_Position)
    , m_Attenuation(a_Attenuation)
    , m_Color(a_Color)
{
}

float3 Light::GetPosition() const
{
    return m_Position;
}

Color Light::GetColor() const
{
    return m_Color;
}

float Light::GetAttenuation() const
{
    return m_Attenuation;
}

Ray Light::GetShadowRay(float3 a_From)
{
    float3 direction = -normalize(a_From - m_Position);
    float3 origin = a_From + direction + 0.00001f;
    return Ray(origin, direction);
}
