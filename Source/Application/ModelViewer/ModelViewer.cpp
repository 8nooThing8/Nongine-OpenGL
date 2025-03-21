#include "Application.h"
#include "ModelViewer.h"

#include <InputHandler.h>

#include <string>

#include "../Engine/GraphicsEngine/ShaderCompiler.h"
#include "../Engine/GraphicsEngine/Material/Material.h"

#include <Timer.h>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Vector\Vector3.hpp"
#include "Matrix\Matrix4x4.hpp"

#include <Engine\GraphicsEngine\Vertex.h>

#include "../Engine/GameEngine/camera.h"
#include "../Engine/GameEngine/InspectorCamera.h"

#include "../Engine/GameEngine/ModelSprite/ModelFactory.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../Engine/GraphicsEngine/stb_image.h"

ModelViewer::ModelViewer() = default;

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool ModelViewer::Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR /*aWindowTitle*/)
{
	//int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	//if (iResult != 0)
	//{
	//	std::cout << "WSAStartup failed: " << iResult << std::endl;
	//	return false;
	//}

	//LPCWSTR windowClassName = L"NonGine";

	//// First we create our Window Class
	//WNDCLASS windowClass = {};
	//windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	//windowClass.lpfnWndProc = aWindowProcess;
	//windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//windowClass.lpszClassName = windowClassName;
	//RegisterClass(&windowClass);

	//LONG posX = (GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2;
	//if (posX < 0)
	//	posX = 0;

	//LONG posY = (GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2;
	//if (posY < 0)
	//	posY = 0;

	//// Then we use the class to create our window
	//myMainWindowHandle = CreateWindow(
	//	windowClassName,                                // Classname
	//	windowClassName,                                   // Window Title
	//	WS_OVERLAPPEDWINDOW | WS_POPUP,                 // Flags
	//	posX,
	//	posY,
	//	aWindowSize.cx,
	//	aWindowSize.cy,
	//	nullptr,
	//	nullptr,
	//	nullptr,
	//	nullptr
	//);

	//MVLOG(Log, "Ready!");

	//ShowWindow(myMainWindowHandle, 5);
	//SetForegroundWindow(myMainWindowHandle);

	// Init GLFW
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		myWindow = glfwCreateWindow(1280, 720, "Nongine", NULL, NULL);
		if (!myWindow)
		{
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(myWindow);

		glfwSetFramebufferSizeCallback(myWindow, FrameBufferSizeCallBack);
	}

	// Init GLAD
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	return true;
}

int ModelViewer::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	//Non::Texture texture;
	//Non::LoadTexture("Assets/Textures/TGA_Bro_M.png", texture);

	int width, height, nrchannels;

	Material material("Assets/Shaders/DefaultFS.glsl", "Assets/Shaders/DefaultVS.glsl");

	//stbi_set_flip_vertically_on_load(true);

	// Create diffuse
	unsigned diffuse;
	{
		unsigned char* data1 = stbi_load("Assets/Textures/TGA_Bro_C.png", &width, &height, &nrchannels, 0);
		glGenTextures(1, &diffuse);
		glBindTexture(GL_TEXTURE_2D, diffuse);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_POINT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data1);
	}

	// Create specular
	unsigned specular;
	{
		unsigned char* data2 = stbi_load("Assets/Textures/TGA_Bro_M.png", &width, &height, &nrchannels, 0);
		glGenTextures(1, &specular);
		glBindTexture(GL_TEXTURE_2D, specular);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_POINT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data2);
	}

	unsigned vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	unsigned objectVertexbuffer;
	glGenVertexArrays(1, &objectVertexbuffer);

	unsigned IndexVertexBuffer;
	glGenBuffers(1, &IndexVertexBuffer);

	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Mesh* mesh = CreateModel("Assets/Models/SK_C_TGA_Bro.fbx");

	// Vertex
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TGA::FBX::Vertex) * mesh->Elements[0].Vertices.size(), mesh->Elements[0].Vertices.data(), GL_STATIC_DRAW);
	}

	// Index
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVertexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * mesh->Elements[0].Indices.size(), mesh->Elements[0].Indices.data(), GL_STATIC_DRAW);
	}

	CommonUtilities::Vector3f position = { 0.3f, 0.3f, 0.f };

	CU::Matrix4x4<float> obectMatrix;

	InspectorCamera cam;

	cam.SetPerspectiveProjection();

	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

	/*CommonUtilities::Matrix4x4<float> translations[125];
	int index = 0;
	for (int z = 0; z < 5; z++)
	{
		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				CommonUtilities::Matrix4x4<float> translation;

				translation.SetPosition(CU::Vector4f{ (float)x * 100.f , (float)y * 100.f , (float)z * 100.f, 1.f });
				translations[index++] = translation;
			}
		}
	}*/

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	while (myIsRunning)
	{
		InputHandler::GetInstance()->UpdateInput();

		CommonUtilities::Time::Update();

		cam.Update(CommonUtilities::Time::GetDeltaTime());
		cam.UpdateAlways();

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}

			if (!InputHandler::GetInstance()->myHWnd)
			{
				InputHandler::GetInstance()->myHWnd = msg.hwnd;
			}

			InputHandler::GetInstance()->UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}
		if (InputHandler::GetInstance()->GetKeyDown(keycode::ESCAPE))
		{
			myIsRunning = false;
		}

		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set layout in shader, (location = 0), second is count of vals, last is layout offset
		{
			int offset = 0;

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TGA::FBX::Vertex), (void*)offset);
			glEnableVertexAttribArray(0);

			offset += (4 * sizeof(float));

			for (int i = 0; i < 4; i++)
			{
				glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(TGA::FBX::Vertex), (void*)offset);
				glEnableVertexAttribArray(1 + i);
				offset += (4 * sizeof(float));
			}

			for (int i = 0; i < 2; i++)
			{
				glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(TGA::FBX::Vertex), (void*)offset);
				glEnableVertexAttribArray(5 + i);
				offset += (4 * sizeof(float));
			}

			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(TGA::FBX::Vertex), (void*)offset);
			glEnableVertexAttribArray(7);
		}

		material.Use();

		//for (unsigned int i = 0; i < 125; i++)
		//{
		//	material.SetMatrix4x4(("offsets[" + std::to_string(i) + "]"), translations[i]);
		//}

		material.SetInt("material.diffuse", 0);
		material.SetInt("material.materialTex", 1);

		material.SetMatrix4x4("objectMatrix", obectMatrix);
		material.SetMatrix4x4("projectionMatrix", cam.GetProjectionMatrix());
		material.SetMatrix4x4("invViewMatrix", cam.GetInverse());

		material.SetVector3("viewPos", cam.position);

		material.SetVector3("light.lightPos", CU::Vector3f(400.f, 700.f, 800.f));
		material.SetVector3("light.lightColor", CU::Vector3f(1.f, 1.f, 1.f));
		material.SetVector3("light.lightDir", CU::Vector3f(400.f, 700.f, 800.f).GetNormalized());

		material.SetFloat("material.specularStrength", 0.5f);
		material.SetFloat("material.shininess", 32.f);

		// Bind primary and seconday tetxures
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuse);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular);
		}

		glBindVertexArray(objectVertexbuffer);

		// Wire frame draw
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Draw indexed
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVertexBuffer);
			glDrawElements(GL_TRIANGLES, sizeof(unsigned) * mesh->Elements[0].Indices.size(), GL_UNSIGNED_INT, 0);

			//glDrawElementsInstanced(GL_TRIANGLES, sizeof(unsigned) * mesh->Elements[0].Indices.size(), GL_UNSIGNED_INT, 0, sizeof(translations) / 16);
		}

		// Draw
		//{
		//	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
		//}


		glfwSwapBuffers(myWindow);
	}

	glfwTerminate();
	return 0;
}