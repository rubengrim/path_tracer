#include "Application.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Eigen/Core"

#include "Renderer.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Color.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

Application::~Application()
{
	Shutdown();
}

bool Application::Initialize()
{
	bool ret = true;

	if (!m_Logger.Initialize())
	{
		ret = false;
	}

	if (!m_Window.Create(800, 600))
	{
		return false; // If the window creation fails we return immediately and don't run the app
	}

	Mouse::Initialize(m_Window.GetGLFWWindowHandle());
	Keyboard::Initialize(m_Window.GetGLFWWindowHandle());

	InitializeImGui();

	return ret;
}

void Application::Run()
{
	// TODO: Abstract colors into class

	m_IsRunning = true;
	PULSE_TRACE("Pulse is initialized and running.");

	m_ViewportWidth = 400;
	m_ViewportHeight = 200;
	float timeToRender = 0.0f;

	m_Camera = new Camera(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, -1.0f), m_ViewportWidth, m_ViewportHeight);
	m_Image = new Image(m_ViewportWidth, m_ViewportHeight);

	float mouseSensitivity = 0.07f;
	float movementSpeed = 1.0f;

	m_BeginFrame = std::chrono::high_resolution_clock::now();
	m_EndFrame = std::chrono::high_resolution_clock::now();
		
	// Set up scene
	Sphere sphere1(Eigen::Vector3f(-1.0f, 0.0f, -4.0f), 0.5f, Color::Red());
	Sphere sphere2(Eigen::Vector3f(1.0f, 0.0f, -4.0f), 0.3f, Color::Green());
	Scene activeScene;
	activeScene.AddSpheres({sphere1, sphere2});

#define RUN 1
#if RUN
	// Main loop
	while (m_IsRunning)
	{
		m_EndFrame = std::chrono::high_resolution_clock::now();
		float timeStep = std::chrono::duration_cast<std::chrono::microseconds>(m_EndFrame - m_BeginFrame).count() * 0.000001f;
		m_BeginFrame = std::chrono::high_resolution_clock::now();

		m_Window.Update();

		// Update mouse and keyboard
		Mouse::Update();
		Keyboard::Update();

		m_Image->Resize(m_ViewportWidth, m_ViewportHeight);
		m_Camera->Resize(m_ViewportWidth, m_ViewportHeight);
		m_Camera->Update(timeStep);

		Renderer::Render(m_Image, &activeScene, m_Camera, timeToRender);
			
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();

		{
			ImGui::Begin("Info");

			//if (ImGui::Button("Render"))
			//	Renderer::Render(m_Image, &activeScene, m_Camera, timeToRender);

			ImGui::Text("CAMERA");
			ImGui::Text("Pitch: %.2f", m_Camera->GetPitch());
			ImGui::Text("Yaw: %.2f", m_Camera->GetYaw());
			ImGui::Text("Position: %.2f, %.2f, %.2f", m_Camera->GetPosition().x(), m_Camera->GetPosition().y(), m_Camera->GetPosition().z());
			ImGui::Text("Forward: %.2f, %.2f, %.2f", m_Camera->GetForwardDirection().x(), m_Camera->GetForwardDirection().y(), m_Camera->GetForwardDirection().z());

			ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.0f, 0.5f);
			ImGui::SliderFloat("Speed", &movementSpeed, 0.0f, 4.0f);
			m_Camera->SetMouseSensitivity(mouseSensitivity);
			m_Camera->SetMovementSpeed(movementSpeed);

			if (ImGui::Button("Reset Camera"))
				m_Camera->ResetForwardAndPosition();
				
			ImGui::NewLine();
			ImGui::Text("Last frame total: %.2f ms", timeStep * 1000.0f);
			ImGui::Text("Rendering: %.2f ms", timeToRender);
			ImGui::End();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Viewport");

			m_ViewportWidth = ImGui::GetContentRegionAvail().x;
			m_ViewportHeight = ImGui::GetContentRegionAvail().y;

			if (m_Image)
				ImGui::Image((void*)(intptr_t)(m_Image->GetTextureId()), ImVec2(m_Image->GetWidth(), m_Image->GetHeight()));
				
			ImGui::End();
			ImGui::PopStyleVar();
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

		m_IsRunning = !m_Window.GetWindowCloseFlag();

		// Clamp framerate
		// std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(50));
	}
#endif
}

bool Application::InitializeImGui()
{
	bool ret = true;

	//if (!m_Window)
	//{
	//	PULSE_ERROR("Attempting to initialize ImGui before the window is created.")
	//	ret = false;
	//}

	// Setup Dear ImGui context
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
	ImGui_ImplGlfw_InitForOpenGL(m_Window.GetGLFWWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	return ret;
}

void Application::Shutdown()
{
	m_Logger.Shutdown();
	m_Window.Shutdown();

	ImGui::DestroyContext();
}