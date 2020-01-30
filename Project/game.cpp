
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
	// clear the graphics window
	screen->Clear( 0 );
	// print something in the graphics window
	screen->Print( "hello world", 2, 2, 0xffffff );
	// draw a sprite
    raytracer.Render();
    raytracer.Present(screen);
}
