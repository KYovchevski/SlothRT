#pragma once
#include "precomp.h"
#include <xmmintrin.h>

struct Ray
{
    Ray(){}
    Ray(float3 a_Origin, float3 a_Direction)
    : m_Origin(a_Origin)
    , m_Direction(normalize(a_Direction))
    , m_RF4Direction(_mm_div_ps(_mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f), m_F4Direction)){}

    void Transform(const mat4& a_Transform)
    {
        m_Origin = a_Transform.TransformPoint(m_Origin);
        m_Direction = a_Transform.TransformVector(m_Direction);
        m_RF4Direction = _mm_div_ps(_mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f), m_F4Direction);
    }

    union
    {
        float3 m_Origin;
        __m128 m_F4Origin;
    };


    union
    {
        float3 m_Direction;
        __m128 m_F4Direction;
    };
    __m128 m_RF4Direction;
};


struct Color
{
    Color(uint8_t a_R = 0, uint8_t a_G = 0, uint8_t a_B = 0, uint8_t a_A = 255)
        : r(a_R)
        , g(a_G)
        , b(a_B)
        , a(a_A)
    {
        
    }

    union
    {
        Pixel pix;
        struct
        {
            uint8_t r, g, b, a;
        };
        uint8_t rgb[4];
    };
};