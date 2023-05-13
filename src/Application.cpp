#include "Application.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "Random.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "Eigen/Core"

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <string>

Application::~Application()
{
	Shutdown();
}

void ErrorCallback(int error, const char* description)
{
	LOG_ERROR("GLFW error: {}", description)
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
    Logger::Error("Error during initialization of GLFW.");
		succeeded = false;
	}

	// BEGIN: Initialize GLFW
	glfwSetErrorCallback(ErrorCallback);

  // Need to use 3.2 with forward compatability and core profile to run correctly on macos.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // This works on windows only.
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	m_WindowHandle = glfwCreateWindow(800, 600, "Path tracer", NULL, NULL);
	if (m_WindowHandle == nullptr)
	{
    Logger::Error("Error during GLFW window creation");
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
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

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

	io.Fonts->AddFontFromFileTTF("external/imgui/misc/fonts/Cousine-Regular.ttf", 13.0f);
	// END: Initialize IMGUI

	Mouse::Initialize(m_WindowHandle);
	Keyboard::Initialize(m_WindowHandle);

	Random::Initialize();

	return succeeded;
}

void Application::Run()
{
	m_IsRunning = true;
  Logger::Info("Application is initialized and running.");

	m_ViewportWidth = 400;
	m_ViewportHeight = 200;

	float timeToRender = 0.0f;

	m_Camera = new Camera(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, -1.0f), m_ViewportWidth, m_ViewportHeight);
	m_ViewportImage = new Image(m_ViewportWidth, m_ViewportHeight);
	m_Renderer = new Renderer(m_ViewportWidth, m_ViewportHeight);

	// Set up scene
	Scene activeScene;

	Material lightGrey("Light grey", Utils::Color::LightGrey());
	Material red("Red", Utils::Color::Red());
	Material green("Green", Utils::Color::Green());
	Material emissive("White emissive", Utils::Color::White(), true, 20.0f, Utils::Color::White());
	activeScene.AddMaterials({lightGrey, red, green, emissive});

	Sphere ground(Eigen::Vector3f(0.0f, -300.5f, -4.0f), 300.0f, 0);
	Sphere sphere1(Eigen::Vector3f(0.0f, 0.0f, -4.0f), 0.5f, 1);
	Sphere lightSource(Eigen::Vector3f(0.0f, 1.0f, -4.0f), 0.2f, 3);
	activeScene.AddSpheres({ground, sphere1, lightSource});

	CameraSettings cameraSettings;
	cameraSettings.MouseSensitivity = 0.07f;
	cameraSettings.MovementSpeed = 1.0f;

	RenderSettings renderSettings;
	renderSettings.Accumulate = true;
	renderSettings.MaxAccumulatedSamples = 100;
	renderSettings.AccumulateForever = true;
	renderSettings.BackgroundColor = Utils::Color::DarkGrey();
	bool shouldResetAccumulation = false;

	auto beginFrame = std::chrono::high_resolution_clock::now();
	auto endFrame = std::chrono::high_resolution_clock::now();

#define GO_INTO_LOOP 1
#if GO_INTO_LOOP 

	// Main loop
	while (m_IsRunning)
	{
		float timeStep = std::chrono::duration_cast<std::chrono::microseconds>(endFrame - beginFrame).count() * 0.000001f;
		beginFrame = std::chrono::high_resolution_clock::now();

		m_Camera->SetMouseSensitivity(cameraSettings.MouseSensitivity);
		m_Camera->SetMovementSpeed(cameraSettings.MovementSpeed);

		glfwSwapBuffers(m_WindowHandle);
		glfwPollEvents();

		Mouse::Update();
		Keyboard::Update();

		m_Camera->Update(timeStep);

		if (m_Camera->HasMoved() || shouldResetAccumulation)
			m_Renderer->ResetAccumulation();
		m_Renderer->Render(*m_ViewportImage, activeScene, *m_Camera, timeToRender, renderSettings);
		shouldResetAccumulation = false;

		// BEGIN: Render GUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();

		float widgetWidth = ImGui::GetFontSize() * 12.0f;

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
				m_ViewportImage->Resize(m_ViewportWidth, m_ViewportHeight);
				m_Camera->Resize(m_ViewportWidth, m_ViewportHeight);
				m_Renderer->Resize(m_ViewportWidth, m_ViewportHeight);
			}

			if (m_ViewportImage)
				ImGui::Image((void*)(intptr_t)(m_ViewportImage->GetTextureId()), ImVec2(m_ViewportImage->GetWidth(), m_ViewportImage->GetHeight()));

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


		{
			ImGui::Begin("Settings");
			ImGui::PushItemWidth(widgetWidth);

			// Camera settings
			ImGui::Text("CAMERA");
			ImGui::DragFloat("Sensitivity", &cameraSettings.MouseSensitivity, 0.002f, 0.0f, 2.0f);
			ImGui::DragFloat("Speed", &cameraSettings.MovementSpeed, 0.01f, 0.0f, 10.0f);
			if (ImGui::Button("Reset Camera"))
			{
				m_Camera->ResetForwardAndPosition();
				shouldResetAccumulation = true;
			}

			ImGui::NewLine();

			// Render settings
			ImGui::Text("RENDERING");
			shouldResetAccumulation |= ImGui::Checkbox("Accumulate samples", &renderSettings.Accumulate);
			shouldResetAccumulation |= ImGui::Checkbox("Accumulate forever", &renderSettings.AccumulateForever);
			shouldResetAccumulation |= ImGui::DragInt("Max samples", &renderSettings.MaxAccumulatedSamples, 0.5f, 0, 1000);
			
			ImGui::NewLine();
			
			// Background color
			shouldResetAccumulation |= ImGui::ColorEdit3("Background color", renderSettings.BackgroundColor.data());
			
			ImGui::PopItemWidth();
			ImGui::End();
		}

		// Materials
		{
			ImGui::Begin("Materials");
			ImGui::PushItemWidth(widgetWidth);

			ImGui::Text("MATERIALS");

			std::vector<int> materialsToBeDeleted;
			for (int i = 0; i < activeScene.Materials().size(); i++)
			{

				ImGui::PushID(i);

				Material& material = activeScene.Materials()[i];
				ImGui::InputText("Name", &material.Name);
				shouldResetAccumulation |= ImGui::ColorEdit3("Albedo", material.Albedo.data());
				shouldResetAccumulation |= ImGui::Checkbox("Emits light", &material.EmitsLight);
				shouldResetAccumulation |= ImGui::DragFloat("Light intensity", &material.LightIntensity, 0.1f, 0.0f);
				shouldResetAccumulation |= ImGui::ColorEdit3("Light color", material.LightColor.data());

				if (ImGui::Button("Delete"))
				{
					materialsToBeDeleted.push_back(i);
					shouldResetAccumulation = true;
				}

				ImGui::PopID();
				ImGui::NewLine();
			}
			// Delete flagged materials.
			for (int matID : materialsToBeDeleted)
			{
				activeScene.DeleteMaterial(matID);
				for (Sphere& sphere : activeScene.Spheres())
				{
					if (sphere.MaterialID == matID)
						sphere.MaterialID = 0;
				}
			}

			if (ImGui::Button("Add Material"))
			{
				Material newMaterial("New material", Utils::Color::LightGrey());
				activeScene.AddMaterial(newMaterial);
				shouldResetAccumulation = true;
			}

			ImGui::PopItemWidth();
			ImGui::End();
		}

		// Spheres
		{
			ImGui::Begin("Spheres");
			ImGui::PushItemWidth(widgetWidth);
			ImGui::Text("SPHERES");

			std::vector<int> spheresToBeDeleted;
			for (int i = 0; i < activeScene.Spheres().size(); i++)
			{
				ImGui::PushID(i);

				Sphere& sphere = activeScene.Spheres()[i];
				shouldResetAccumulation |= ImGui::DragFloat3("Position", sphere.Center.data(), 0.01f);
				shouldResetAccumulation |= ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);

				std::vector<Material>& materials = activeScene.Materials();
				int selected = sphere.MaterialID;
				if (ImGui::BeginCombo("Material", activeScene.GetMaterial(selected).Name.c_str()))
				{
					for (int i = 0; i < materials.size(); i++)
					{
						bool isSelected = selected == sphere.MaterialID;
						if (ImGui::Selectable(activeScene.GetMaterial(i).Name.c_str(), &isSelected))
						{
							selected = i;
							shouldResetAccumulation = true;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				sphere.MaterialID = selected;

				if (ImGui::Button("Delete"))
				{
					spheresToBeDeleted.push_back(i);
					shouldResetAccumulation = true;
				}

				ImGui::PopID();
				ImGui::NewLine();
			}

			
			// Delete flagged spheres.
			for (int sphereID : spheresToBeDeleted)
				activeScene.DeleteSphere(sphereID);

			if (ImGui::Button("Add Sphere"))
			{
				Sphere newSphere(Eigen::Vector3f::Zero(), 0.5f, 0);
				activeScene.AddSphere(newSphere);
				shouldResetAccumulation = true;
			}

			ImGui::PopItemWidth();
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
