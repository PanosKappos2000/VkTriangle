#include "Application.h"

int main()
{
	Application* main = new Application();
	main->Run();
	delete main;
}