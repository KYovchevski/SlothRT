#include "precomp.h"
#include "Raytracer.h"
#include "Triangle.h"

Raytracer::Raytracer(unsigned short a_Width, unsigned short a_Height)
    : m_Width(a_Width)
    , m_Height(a_Height)
    , m_CamPos({ 0.0f, 0.0f, 0.0f + 200.0f })
{
    m_Data = new Color[static_cast<int>(m_Width) * static_cast<int>(m_Height)];

    m_Surface = new Surface(m_Width, m_Height, &m_Data->pix, m_Width);
    m_Sprite = new Sprite(m_Surface, 1);

    Triangle* triangle = new Triangle(float3{ 200000.0f, -250.0f, -100000.0f },
        float3{ 200000.0f, -250.0f, 100000.0f },
        float3{ -250000.0f, -250.0f, 100000.0f });

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
    //m_Scene.AddHitable(*triangle);

    triangle = new Triangle(float3{ 300.0f, 300.0f, 110.0f },
        float3{ 100.0f, 400.0f, 110.0f },
        float3{ 500.0f, 400.0f, 110.0f });
    c.rgb[0] = 0;
    c.rgb[1] = 255;
    c.rgb[2] = 0;
    c.rgb[3] = 0;

    triangle->SetColor(c);
    //m_Scene.AddHitable(*triangle);

    Mesh* m = m_MeshLoader.LoadMesh("assets/Lantern/Lantern.gltf", mat4::Scale(20.0f));
    //m->m_BVHRoot->SetTransform(mat4::Scale(2.0f));


    m_Scene.AddHitable(*m->m_BVHRoot.get());

    m_Scene.ConstructBvh();
}

void Raytracer::Render()
{
    ProcessInput();
    for (unsigned short x = 0; x < m_Width; ++x)
    {
        for (unsigned short  y = 0; y < m_Height; y++)
        {
            Color* pixel = &m_Data[y * m_Width + x];

            float3 dir = float3{ static_cast<float>(x - m_Width / 2), static_cast<float>(m_Height / 2 - y), -100.0f };
            dir = normalize(dir);

            Ray ray(m_CamPos, dir);

            *pixel = m_Scene.TraceRay(ray);

            
        }
    }
    
}

void Raytracer::Present(Surface* a_Screen)
{
    m_Sprite->Draw(a_Screen, 0, 0);
}

void Raytracer::DebugRay(float2 a_ScreenPos)
{
    float3 dir = float3{ static_cast<float>(a_ScreenPos.x - m_Width / 2), static_cast<float>(m_Height / 2 - a_ScreenPos.y), -100.0f };
    dir = normalize(dir);

    Ray ray(m_CamPos, dir);

    m_Scene.TraceRay(ray);
}

void Raytracer::ProcessInput()
{
    if (IsKeyPressed(GLFW_KEY_A))
    {
        m_CamPos += float3{ -1.0f, 0.0f, 0.0f };
    }
    if (IsKeyPressed(GLFW_KEY_D))
    {
        m_CamPos += float3{ 1.0f, 0.0f, 0.0f };
    }

    if (IsKeyPressed(GLFW_KEY_W))
    {
        m_CamPos += float3{ 0.0f, 0.0f, -1.0f };
    }
    if (IsKeyPressed(GLFW_KEY_S))
    {
        m_CamPos += float3{ 0.0f, 0.0f, 1.0f };
    }

    if (IsKeyPressed(GLFW_KEY_Q))
    {
        m_CamPos += float3{ 0.0f, -1.0f, 0.0f };
    }
    if (IsKeyPressed(GLFW_KEY_E))
    {
        m_CamPos += float3{ 0.0f, 1.0f, 0.0f };
    }
}
