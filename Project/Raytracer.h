#pragma once

#include "Structs.h"
#include "Scene.h"
#include "ModelLoader.h"

class Raytracer
{
public:
    Raytracer(unsigned short a_Width, unsigned short a_Height);

    void Render();

    void Present(Surface* a_Screen);

    void UpdateKeyState(int a_Key, int a_State);

    void DebugRay(float2 a_ScreenPos);

private:

    void ProcessInput();

    bool IsKeyPressed(int a_Key) { return m_KeyStates[a_Key] == GLFW_PRESS; }

    float3 m_CamPos;

    std::map<int, int> m_KeyStates;

    std::chrono::high_resolution_clock::time_point m_LastFrame;

    ModelLoader m_MeshLoader;

    Scene m_Scene;

    const unsigned short m_Width;
    const unsigned short m_Height;

    Color* m_Data;

    Surface* m_Surface;
    Sprite* m_Sprite;
};

inline void Raytracer::UpdateKeyState(int a_Key, int a_State)
{
    m_KeyStates[a_Key] = a_State;
}

