#include "precomp.h"
#include "Raytracer.h"
#include "Triangle.h"

const Mesh* m = nullptr;

Raytracer::Raytracer(unsigned short a_Width, unsigned short a_Height)
    : m_Width(a_Width)
    , m_Height(a_Height)
    , m_CamPos({ 0.0f, 0.0f, 0.0f + 500.0f })
{
    m_Data = new Color[static_cast<int>(m_Width) * static_cast<int>(m_Height)];

    m_Surface = new Surface(m_Width, m_Height, &m_Data->pix, m_Width);
    m_Sprite = new Sprite(m_Surface, 1);

    Triangle* triangle = new Triangle(float3{ 200000.0f, -250.0f, 100000.0f },
        float3{ 200000.0f, -250.0f, -100000.0f },
        float3{ -250000.0f, -250.0f, -100000.0f });

    Color c;
    c.rgb[0] = 255;
    c.rgb[1] = 255;
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

    triangle = new Triangle(float3{ 5.0f, 100.0f, 110.0f },
                            float3{ 5.0f, 160.0f, 110.0f },
                            float3{ 5.0f, 100.0f, 150.0f });
    c.rgb[0] = 255;
    c.rgb[1] = 255;
    c.rgb[2] = 255;
    c.rgb[3] = 0;

    Light* l = new Light(make_float3(300.0f, 300.0f, 300.0f), 100.0f);

    m_Scene.AddLight(*l);

    triangle->SetColor(c);
    //m_Scene.AddHitable(*triangle);

    m = m_MeshLoader.LoadMesh("assets/Quacker/Duck.gltf", mat4::Scale(1.0f));
    //m->m_BVHRoot->SetTransform(mat4::Translate(make_float3(1000000.0f, 0.0f, 0.0f)));


    m_Scene.AddHitable(*m->m_BVHRoot.get());

    m_Scene.ConstructBvh();

    m_LastFrame = std::chrono::high_resolution_clock::now();
}

static float rot = 0.0f;
void Raytracer::Render()
{
    ProcessInput();

    rot += 10.0f;

    m->m_BVHRoot->SetTransform(mat4::Rotate(0.0f, 1.0f, 0.0f,rot / 180.0f * 3.14159f));

    float fov = 45.0f; // degrees
    float tgfov = std::tan(fov / 180.0f * 3.14159f);

    float dist;

    dist = tgfov * m_Width;
    for (unsigned short x = 0; x < m_Width; ++x)
    {
        for (unsigned short  y = 0; y < m_Height; y++)
        {
            Color* pixel = &m_Data[y * m_Width + x];

            

            float3 dir = float3{ static_cast<float>(x - m_Width / 2), static_cast<float>(m_Height / 2 - y), -dist };
            dir = normalize(dir);

            Ray ray(m_CamPos, dir);

            *pixel = m_Scene.TraceRay(ray);

            
        }
    }

    auto currentFrame = std::chrono::high_resolution_clock::now();

    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - m_LastFrame);

    std::cout << delta.count() << std::endl;

    m_LastFrame = currentFrame;
}

void Raytracer::Present(Surface* a_Screen)
{
    m_Sprite->Draw(a_Screen, 0, 0);
}

void Raytracer::DebugRay(float2 a_ScreenPos)
{
    float fov = 45.0f; // degrees
    float tgfov = std::tan(fov / 180.0f * 3.14159f);

    float dist;

    dist = tgfov * m_Width;

    float3 dir = float3{ static_cast<float>(a_ScreenPos.x - m_Width / 2), static_cast<float>(m_Height / 2 - a_ScreenPos.y), -dist };

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
