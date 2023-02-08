#include "Application.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "Color.h"
#include "Random.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "Eigen/Core"

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

Application::~Application()
{
	Shutdown();
}

void ErrorCallback(int error, const char* description)
{
	PULSE_ERROR("GLFW error: {}", description)
}

bool Application::Initialize()
{
	bool succeeded = true;

	if (!m_Logger.Initialize())
	{
		succeeded = false;
	}

	if (!glfwInit())
	{
		PULSE_ERROR("Error during initialization of GLFW.")
		succeeded = false;
	}

	// BEGIN: Initialize GLFW
	glfwSetErrorCallback(ErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	m_WindowHandle = glfwCreateWindow(800, 600, "My Title", NULL, NULL);
	if (m_WindowHandle == nullptr)
	{
		PULSE_ERROR("Error during GLFW window creation.")
		succeeded = false;
	}

	glfwMakeContextCurrent(m_WindowHandle);
	gladLoadGL(glfwGetProcAddress);

	glfwSwapInterval(0);
	// END: Initialize GLFW

	// BEGIN: Initialize IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// END: Initialize IMGUI

	Mouse::Initialize(m_WindowHandle);
	Keyboard::Initialize(m_WindowHandle);

	Random::Initialize();

	return succeeded;
}

void Application::Run()
{
	m_IsRunning = true;
	PULSE_TRACE("Pulse is initialized and running.");

	m_ViewportWidth = 400;
	m_ViewportHeight = 200;
	float timeToRender = 0.0f;

	m_Camera = new Camera(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, -1.0f), m_ViewportWidth, m_ViewportHeight);
	m_Image = new Image(m_ViewportWidth, m_ViewportHeight);
	m_Renderer = new Renderer(m_ViewportWidth, m_ViewportHeight);

	// Set up scene
	Scene activeScene;

	Material defaultGrey(Color::DarkGrey());
	Material red(Color::Red());
	Material green(Color::Green());
	activeScene.AddMaterials({defaultGrey, red, green});

	Sphere sphere1(Eigen::Vector3f(0.0f, 0.0f, -4.0f), 0.5f, 1);
	//Sphere sphere2(Eigen::Vector3f(0.7f, 0.0f, -4.0f), 0.3f, 2);
	Sphere sphere3(Eigen::Vector3f(0.0f, -50.5f, -4.0f), 50.0f, 0);
	activeScene.AddSpheres({sphere1, sphere3});

	Settings settings;
	settings.MouseSensitivity = 0.07f;
	settings.MovementSpeed = 1.0f;

	auto beginFrame = std::chrono::high_resolution_clock::now();
	auto endFrame = std::chrono::high_resolution_clock::now();

#define RUN 1
#if RUN

	// Main loop
	while (m_IsRunning)
	{
		float timeStep = std::chrono::duration_cast<std::chrono::microseconds>(endFrame - beginFrame).count() * 0.000001f;
		beginFrame = std::chrono::high_resolution_clock::now();

		glfwSwapBuffers(m_WindowHandle);
		glfwPollEvents();

		Mouse::Update();
		Keyboard::Update();

		m_Camera->Update(timeStep);

		if (m_Camera->HasMoved())
			m_Renderer->ResetAccumulation();
		m_Renderer->Render(*m_Image, activeScene, *m_Camera, timeToRender);
		
		// BEGIN: Render GUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();

		// Viewport
		{ 
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Viewport");

			int prevViewportWidth = m_ViewportWidth;
			int prevViewportHeight = m_ViewportHeight;
			m_ViewportWidth = ImGui::GetContentRegionAvail().x;
			m_ViewportHeight = ImGui::GetContentRegionAvail().y;
			if (prevViewportWidth != m_ViewportWidth || prevViewportHeight != m_ViewportHeight)
			{
				m_Image->Resize(m_ViewportWidth, m_ViewportHeight);
				m_Camera->Resize(m_ViewportWidth, m_ViewportHeight);
				m_Renderer->Resize(m_ViewportWidth, m_ViewportHeight);
			}

			if (m_Image)
				ImGui::Image((void*)(intptr_t)(m_Image->GetTextureId()), ImVec2(m_Image->GetWidth(), m_Image->GetHeight()));
				
			ImGui::End();
			ImGui::PopStyleVar();
		}

		// Info window
		{
			ImGui::Begin("Info");
			ImGui::Text("Camera Position: %.2f, %.2f, %.2f", m_Camera->GetPosition().x(), m_Camera->GetPosition().y(), m_Camera->GetPosition().z());
			ImGui::Text("Camera Direction: %.2f, %.2f, %.2f", m_Camera->GetForwardDirection().x(), m_Camera->GetForwardDirection().y(), m_Camera->GetForwardDirection().z());
			ImGui::NewLine();
			ImGui::Text("Last frame total: %.2f ms", timeStep * 1000.0f);
			ImGui::Text("Rendering: %.2f ms", timeToRender);
			ImGui::End();
		}

		// Settings window
		{
			ImGui::Begin("Settings");

			ImGui::Text("CAMERA");
			ImGui::DragFloat("Sensitivity", &m_Camera->MouseSensitivity(), 0.002f, 0.0f, 2.0f);
			ImGui::DragFloat("Speed", &m_Camera->MovementSpeed(), 0.01f, 0.0f, 10.0f);
			if (ImGui::Button("Reset Camera"))
				m_Camera->ResetForwardAndPosition();

			ImGui::NewLine();

			ImGui::Text("SCENE");

			if (ImGui::Button("Add Sphere"))
			{
				Sphere newSphere(Eigen::Vector3f::Zero(), 0.5f, 0);
				activeScene.AddSphere(newSphere);
			}

			ImGui::NewLine();

			std::vector<int> spheresToBeDeleted;
			// Draw settings for all spheres in the scene
			for (int i = 0; i < activeScene.Spheres().size(); i++)
			{
				ImGui::PushID(i);

				Sphere& sphere = activeScene.Spheres()[i];
				ImGui::DragFloat3("Position", sphere.Center.data(), 0.01f);
				ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);

				Material& material = activeScene.Materials()[sphere.MaterialIndex];
				Eigen::Vector3f albedo(material.Albedo.R, material.Albedo.G, material.Albedo.B);
				ImGui::ColorEdit3("Albedo", albedo.data());
				material.Albedo = Color(albedo.x(), albedo.y(), albedo.z());

				if (ImGui::Button("Delete"))
					spheresToBeDeleted.push_back(i);

				ImGui::PopID();
				ImGui::NewLine();
			}
			// Delete flagged spheres
			for (int i : spheresToBeDeleted)
				activeScene.DeleteSphere(i);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		// For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		// END: Render GUI

		m_IsRunning = !glfwWindowShouldClose(m_WindowHandle);
		endFrame = std::chrono::high_resolution_clock::now();
	}

#endif
}

void Application::Shutdown()
{
	m_Logger.Shutdown();

	glfwDestroyWindow(m_WindowHandle);
	glfwTerminate();

	ImGui::DestroyContext();
}