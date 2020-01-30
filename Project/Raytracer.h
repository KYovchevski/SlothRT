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

private:

    ModelLoader m_MeshLoader;

    Scene m_Scene;

    const unsigned short m_Width;
    const unsigned short m_Height;

    Color* m_Data;

    Surface* m_Surface;
    Sprite* m_Sprite;
};

