#pragma once
#include "precomp.h"

struct Ray
{
    Ray(float3 a_Origin, float3 a_Direction) : m_Origin(a_Origin), m_Direction(normalize(a_Direction)){}
    float3 m_Origin;
    float3 m_Direction;
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