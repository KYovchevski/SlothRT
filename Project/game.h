#pragma once

namespace Tmpl8 {

class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Dummy();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
    void MouseDown(int button);
    void MouseMove(int x, int y) { m_MousePos = { static_cast<float>(x), static_cast<float>(y) }; }
    void KeyUp(int a_Key);
    void KeyDown(int a_Key);
	Surface* screen;

private:

    float2 m_MousePos;
};

}; // namespace Tmpl8