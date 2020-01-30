#include "precomp.h"
#include "Raytracer.h"
#include "Triangle.h"

Raytracer::Raytracer(unsigned short a_Width, unsigned short a_Height)
    : m_Width(a_Width)
    , m_Height(a_Height)
{
    m_Data = new Color[static_cast<int>(m_Width) * static_cast<int>(m_Height)];

    m_Surface = new Surface(m_Width, m_Height, &m_Data->pix, m_Width);
    m_Sprite = new Sprite(m_Surface, 1);

    Triangle* triangle = new Triangle(float3{ 200.0f, 200.0f, 99.0f },
        float3{ 200.0f, 250.0f, 100.0f },
        float3{ 250.0f, 200.0f, 100.0f });

    Color c;
    c.rgb[0] = 255;
    c.rgb[1] = 0;
    c.rgb[2] = 0;
    c.rgb[3] = 0;

    triangle->SetColor(c);
    m_Scene.AddHitable(*triangle);


    triangle = new Triangle(float3{ 100.0f, 200.0f, 101.0f },
        float3{ 300.0f, 250.0f, 100.0f },
        float3{ 350.0f, 400.0f, 100.0f });
    c.rgb[0] = 0;
    c.rgb[1] = 0;
    c.rgb[2] = 255;
    c.rgb[3] = 0;

    triangle->SetColor(c);
    m_Scene.AddHitable(*triangle);

    triangle = new Triangle(float3{ 300.0f, 300.0f, 110.0f },
        float3{ 100.0f, 400.0f, 110.0f },
        float3{ 500.0f, 400.0f, 110.0f });
    c.rgb[0] = 0;
    c.rgb[1] = 255;
    c.rgb[2] = 0;
    c.rgb[3] = 0;

    triangle->SetColor(c);
    m_Scene.AddHitable(*triangle);

    Mesh* m = m_MeshLoader.LoadMesh("assets/Lantern/Lantern.gltf", mat4::Scale(10.0f));



    m_Scene.AddHitable(*m->m_BVHRoot.get());

    m_Scene.ConstructBvh();
}

void Raytracer::Render()
{
    for (unsigned short x = 0; x < m_Width; ++x)
    {
        for (unsigned short  y = 0; y < m_Height; y++)
        {
            Color* pixel = &m_Data[y * m_Width + x];

            Ray ray(float3{ static_cast<float>(x), static_cast<float>(y), -100.0f }, float3{ 0.0f, 0.0f, 1.0f });

            *pixel = m_Scene.TraceRay(ray);


        }
    }
    
}

void Raytracer::Present(Surface* a_Screen)
{
    m_Sprite->Draw(a_Screen, 0, 0);
}
