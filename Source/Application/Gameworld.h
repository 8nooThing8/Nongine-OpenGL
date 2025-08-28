#pragma once

struct GLFWvidmode;
struct GLFWwindow;
struct GLFWmonitor;

class Client;

#include "Vector/Vector2.hpp"

class GameWorld
{
public:
	GameWorld();

	bool Initialize();

    void PingUpdate();

    //void MainLoop();
	int Run();

private:

	bool myIsRunning = true;

	GLFWvidmode* fullscreenMode;
	CommonUtilities::Vector2u windowedMode;

	GLFWmonitor* monitor;

	int success;
	char infoLog[512];

	float pingCheckTimer = 0.3;

	Client* myClient;
};
