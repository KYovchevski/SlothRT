#include "precomp.h"
#include "Raytracer.h"
#include "Triangle.h"
#include "MeshInstance.h"

static MeshInstance* m = nullptr;
static MeshInstance* m1 = nullptr;

Raytracer::Raytracer(unsigned short a_Width, unsigned short a_Height)
    : m_Width(a_Width)
    , m_Height(a_Height)
    , m_CamPos({ 0.0f, 0.0f, 0.0f + 500.0f })
{
    m_RenderMode = EMode::ENormals;
    
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

    Light* l = new Light(make_float3(300.0f, 300.0f, 100.0f), 100.0f);

    m_Scene.AddLight(*l);

    triangle->SetColor(c);
    //m_Scene.AddHitable(*triangle);

    Mesh* mesh = m_MeshLoader.LoadMesh("assets/Quacker/Duck.gltf", mat4::Scale(1.0f));

    m = new MeshInstance(*mesh);
    m1 = new MeshInstance(*mesh);
    //m->m_BVHRoot->SetTransform(mat4::Translate(make_float3(1000000.0f, 0.0f, 0.0f)));


    m_Scene.AddHitable(*m);
    m_Scene.AddHitable(*m1);

    m_Scene.ConstructBvh();

    m_LastFrame = std::chrono::high_resolution_clock::now();
}

static int frame = 0;
static float timer = 0.0f;
static float rot = 0.0f;
void Raytracer::Render()
{
    ProcessInput();

    frame++;
    auto currentFrame = std::chrono::high_resolution_clock::now();

    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - m_LastFrame);

    //std::cout << delta.count() << std::endl;

    m_LastFrame = currentFrame;


    timer += delta.count() / 1000.0f;

    if (timer > 1.0f)
    {
        timer -= 1.0f;
        std::cout << "Rough FPS: " << frame << std::endl;
        frame = 0;
    }


    rot += 60.0f * (delta.count() / 1000.0f);

    m1->SetTransform(mat4::Translate(100.0f, 0.0f, 0.0f) * mat4::Rotate(0.0f, 1.0f, 0.0f, (180.0f - rot) / 180.0f * 3.14159f));
            //                         100
    m->SetTransform(mat4::Translate(-100.0f, 0.0f, 0.0f) * mat4::Rotate(0.0f, 1.0f, 0.0f,rot / 180.0f * 3.14159f));

    float fov = 45.0f; // degrees
    float tgfov = std::tan(fov / 180.0f * 3.14159f);

    float dist;

    dist = tgfov * m_Width;
    for (unsigned short y = 0; y < m_Height; y++)
    {
        for (unsigned short x = 0; x < m_Width; ++x)
        {
            Color* pixel = &m_Data[y * m_Width + x];

            float3 dir = float3{ static_cast<float>(x - m_Width / 2), static_cast<float>(m_Height / 2 - y), -dist };
            dir = normalize(dir);

            Ray ray(m_CamPos, dir);

            *pixel = m_Scene.TraceRay(ray, m_RenderMode).m_Color;            
        }
    }
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

    auto i = m_Scene.TraceRay(ray, m_RenderMode);
    std::cout << i.m_Intersection.m_NumBVHChecks << std::endl;
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

    if (IsKeyPressed(GLFW_KEY_M) && !m_MPressed)
    {
        m_RenderModeIndex += 1;
        m_RenderModeIndex %= EMode::ECount;
    }
    m_MPressed = IsKeyPressed(GLFW_KEY_M);
}
