#include "Application.h"

Application::Application()
	:m_window(), m_graphics()
{

}

Application::~Application()
{
	
}

void Application::Run()
{
	m_window.Init();
	m_graphics.Init(m_window);
	while (!m_window.ShouldClose())
	{
		m_window.MainLoop();
		m_graphics.MainLoop();
	}
	m_window.Cleanup();
	m_graphics.Cleanup();
}