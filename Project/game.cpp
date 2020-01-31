
#include "precomp.h" // include (only) this in every .cpp file
#include "Raytracer.h"

void Game::Dummy()
{
	printf( "Task A\n" );	
}

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	AddTask( [&](){ this->Dummy(); } );
	AddTask( [](){} );
	RunTasks();
	WaitForAll();
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

static Raytracer raytracer(520, 400);

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{

    GetKeyState(GLFW_KEY_W);
	// clear the graphics window
	screen->Clear( 0 );
	// print something in the graphics window
	screen->Print( "hello world", 2, 2, 0xffffff );
	// draw a sprite
    raytracer.Render();
    raytracer.Present(screen);

    
}

void Game::MouseDown(int button)
{
    raytracer.DebugRay(m_MousePos);
    std::cout << m_MousePos.x << ", " << m_MousePos.y << std::endl;
}

void Game::KeyUp(int a_Key)
{
    raytracer.UpdateKeyState(a_Key, GLFW_RELEASE);
}

void Game::KeyDown(int a_Key)
{
    raytracer.UpdateKeyState(a_Key, GLFW_PRESS);
}
