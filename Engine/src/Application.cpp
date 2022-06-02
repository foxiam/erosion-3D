// STL
#include <iostream>
#include <memory>

// GLM
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

// Project
#include "../includes/Application.h"

#include "../includes/common_classes/flyingCamera.h"

#include "../includes/common_classes/freeTypeFont.h"
#include "../includes/common_classes/shaderManager.h"
#include "../includes/common_classes/shaderProgramManager.h"
#include "../includes/common_classes/textureManager.h"
#include "../includes/common_classes/samplerManager.h"
#include "../includes/common_classes/matrixManager.h"

#include "../includes/common_classes/static_meshes_3D/skybox.h"
#include "../includes/common_classes/static_meshes_3D/heightmap.h"

#include "../includes/common_classes/shader_structs/ambientLight.h"
#include "../includes/common_classes/shader_structs/diffuseLight.h"

FlyingCamera camera(glm::vec3(0.0f, 25.0f, -60.0f), glm::vec3(0.0f, 25.0f, -59.0f), glm::vec3(0.0f, 1.0f, 0.0f), 15.0f);


std::unique_ptr<static_meshes_3D::Heightmap> heightmap;
std::unique_ptr<static_meshes_3D::Skybox> skybox;

float rotationAngleRad = 0.0f;
bool displayNormals = false;
bool checkErosion = false;
bool checkCursor = true;
shader_structs::AmbientLight ambientLight(glm::vec3(0.6f, 0.6f, 0.6f));
shader_structs::DiffuseLight diffuseLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f)), 0.4f);

const glm::vec3 heightMapSize(200.0f, 50.0f, 200.0f);

void OpenGLWindow018::initializeScene()
{
	try
	{
		auto& sm = ShaderManager::getInstance();
		auto& spm = ShaderProgramManager::getInstance();
		auto& tm = TextureManager::getInstance();

		sm.loadVertexShader("tut014_main", "../../Engine/data/shaders/tut014-diffuse-lighting/shader.vert");
		sm.loadFragmentShader("tut014_main", "../../Engine/data/shaders/tut014-diffuse-lighting/shader.frag");
		sm.loadFragmentShader("ambientLight", "../../Engine/data/shaders/lighting/ambientLight.frag");
		sm.loadFragmentShader("diffuseLight", "../../Engine/data/shaders/lighting/diffuseLight.frag");

		sm.loadVertexShader("normals", "../../Engine/data/shaders/normals/normals.vert");
		sm.loadGeometryShader("normals", "../../Engine/data/shaders/normals/normals.geom");
		sm.loadFragmentShader("normals", "../../Engine/data/shaders/normals/normals.frag");

		auto& mainShaderProgram = spm.createShaderProgram("main");
		mainShaderProgram.addShaderToProgram(sm.getVertexShader("tut014_main"));
		mainShaderProgram.addShaderToProgram(sm.getFragmentShader("tut014_main"));
		mainShaderProgram.addShaderToProgram(sm.getFragmentShader(ShaderKeys::ambientLight()));
		mainShaderProgram.addShaderToProgram(sm.getFragmentShader(ShaderKeys::diffuseLight()));

		auto& normalsShaderProgram = spm.createShaderProgram("normals");
		normalsShaderProgram.addShaderToProgram(sm.getVertexShader("normals"));
		normalsShaderProgram.addShaderToProgram(sm.getGeometryShader("normals"));
		normalsShaderProgram.addShaderToProgram(sm.getFragmentShader("normals"));

		skybox = std::make_unique<static_meshes_3D::Skybox>("../../Engine/data/skyboxes/desert", "png");

		SamplerManager::getInstance().createSampler("main", MAG_FILTER_BILINEAR, MIN_FILTER_TRILINEAR);
		TextureManager::getInstance().loadTexture2D("crate", "../../Engine/data/textures/crate.png");
		TextureManager::getInstance().loadTexture2D("white_marble", "../../Engine/data/textures/white_marble.jpg");
		TextureManager::getInstance().loadTexture2D("grass", "../../Engine/data/textures/grass.jpg");
		TextureManager::getInstance().loadTexture2D("rocky_terrain", "../../Engine/data/textures/rocky_terrain.jpg");
		TextureManager::getInstance().loadTexture2D("snow", "../../Engine/data/textures/snow.png");


		static_meshes_3D::Heightmap::prepareMultiLayerShaderProgram();
		heightmap = std::make_unique<static_meshes_3D::Heightmap>("../../Engine/data/heightmaps/tut017.png", true, true, true);

		spm.linkAllPrograms();


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplOpenGL3_Init();
		ImGui_ImplGlfw_InitForOpenGL(OpenGLWindow::_window, true);
		ImGui::StyleColorsDark();

	}
	catch (const std::runtime_error& ex)
	{
		std::cout << "Error occured during initialization: " << ex.what() << std::endl;
		closeWindow(true);
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGLWindow018::renderScene()
{
	const auto& spm = ShaderProgramManager::getInstance();
	const auto& tm = TextureManager::getInstance();
	auto& mm = MatrixManager::getInstance();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set matrices in matrix manager
	mm.setProjectionMatrix(getProjectionMatrix());
	mm.setOrthoProjectionMatrix(getOrthoProjectionMatrix());
	mm.setViewMatrix(camera.getViewMatrix());

	// Set up some common properties in the main shader program
	auto& mainProgram = spm.getShaderProgram("main");
	mainProgram.useProgram();
	mainProgram[ShaderConstants::projectionMatrix()] = getProjectionMatrix();
	mainProgram[ShaderConstants::viewMatrix()] = camera.getViewMatrix();
	mainProgram.setModelAndNormalMatrix(glm::mat4(1.0f));
	mainProgram[ShaderConstants::color()] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mainProgram[ShaderConstants::sampler()] = 0;

	// Render skybox first with only ambient light
	shader_structs::AmbientLight(glm::vec3(0.8f, 0.8f, 0.8f), true).setUniform(mainProgram, ShaderConstants::ambientLight());
	shader_structs::DiffuseLight::none().setUniform(mainProgram, ShaderConstants::diffuseLight());
	skybox->render(camera.getEye(), mainProgram);

	// Set up ambient and diffuse light in the shader program
	SamplerManager::getInstance().getSampler("main").bind();
	ambientLight.setUniform(mainProgram, ShaderConstants::ambientLight());
	diffuseLight.setUniform(mainProgram, ShaderConstants::diffuseLight());


	// Render heightmap
	if (checkErosion) {
		heightmap->erode(50);
		heightmap->createFromHeightData(heightmap->_heightData);
	} 

	auto& heightmapShaderProgram = static_meshes_3D::Heightmap::getMultiLayerShaderProgram();
	heightmapShaderProgram.useProgram();
	heightmapShaderProgram[ShaderConstants::projectionMatrix()] = getProjectionMatrix();
	heightmapShaderProgram[ShaderConstants::viewMatrix()] = camera.getViewMatrix();
	heightmapShaderProgram.setModelAndNormalMatrix(glm::mat4(1.0f));
	heightmapShaderProgram[ShaderConstants::color()] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	ambientLight.setUniform(heightmapShaderProgram, ShaderConstants::ambientLight());
	diffuseLight.setUniform(heightmapShaderProgram, ShaderConstants::diffuseLight());

	const auto heightmapModelMatrix = glm::scale(glm::mat4(1.0f), heightMapSize);
	heightmapShaderProgram.setModelAndNormalMatrix(heightmapModelMatrix);
	heightmap->renderMultilayered({ "rocky_terrain", "grass", "snow" }, { 0.0f, 0.0f, 1.0f, 0.0f });

	if (displayNormals)
	{
		// Set up some common properties in the normals shader program
		auto& normalsShaderProgram = spm.getShaderProgram("normals");
		normalsShaderProgram.useProgram();
		normalsShaderProgram[ShaderConstants::projectionMatrix()] = getProjectionMatrix();
		normalsShaderProgram[ShaderConstants::viewMatrix()] = camera.getViewMatrix();
		normalsShaderProgram[ShaderConstants::normalLength()] = 0.5f;


		normalsShaderProgram.setModelAndNormalMatrix(heightmapModelMatrix);
		heightmap->renderPoints();
	}

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(OpenGLWindow::getScreenWidth());
	io.DisplaySize.y = static_cast<float>(OpenGLWindow::getScreenHeight());
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");

	//Erosion
	ImGui::Text("Erosion");
	ImGui::InputInt("remaining", &heightmap->remaining, 1, 1);
	ImGui::InputInt("Erosion step", &heightmap->erosionstep, 1, 1);
	//Particle properties
	ImGui::Text("Particle properties");
	ImGui::InputFloat("dt", &heightmap->dt, 0.01, 0.01);
	ImGui::InputFloat("density", &heightmap->density, 0.1, 0.1);
	ImGui::InputFloat("evapRate", &heightmap->evapRate, 0.001, 0.001);
	ImGui::InputFloat("deposition rate", &heightmap->depositionRate, 0.1, 0.1);
	ImGui::InputFloat("min volume", &heightmap->minVol, 0.01, 0.01);
	ImGui::InputFloat("friction", &heightmap->friction, 0.01, 0.01);

	ImGui::Button("Test");

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void OpenGLWindow018::updateScene()
{
	if (keyPressedOnce(GLFW_KEY_ESCAPE)) {
		closeWindow();
	}

	if (keyPressedOnce(GLFW_KEY_F3)) {
		setVerticalSynchronization(!isVerticalSynchronizationEnabled());
	}

	if (keyPressedOnce(GLFW_KEY_N)) {
		displayNormals = !displayNormals;
	}

	if (keyPressedOnce(GLFW_KEY_Q)) {
		checkErosion = !checkErosion;
		std::cout << "start erosion" << std::endl;
	}
	if (keyPressedOnce(GLFW_KEY_SPACE)) {
		checkCursor = !checkCursor;
	}


	int posX, posY, width, height;
	glfwGetWindowPos(getWindow(), &posX, &posY);
	glfwGetWindowSize(getWindow(), &width, &height);
	camera.setWindowCenterPosition(glm::i32vec2(posX + width / 2, posY + height / 2));
	if (checkCursor) {
		camera.update([this](int keyCode) {return this->keyPressed(keyCode); },
			[this]() {double curPosX, curPosY; glfwGetCursorPos(this->getWindow(), &curPosX, &curPosY); return glm::u32vec2(curPosX, curPosY); },
			[this](const glm::i32vec2& pos) {glfwSetCursorPos(this->getWindow(), pos.x, pos.y); },
			[this](float f) {return this->sof(f); });
		glfwSetInputMode(OpenGLWindow::_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else glfwSetInputMode(OpenGLWindow::_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Update rotation angle
	rotationAngleRad += sof(glm::radians(45.0f));



}

void OpenGLWindow018::releaseScene()
{
	skybox.reset();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	ShaderManager::getInstance().clearShaderCache();
	ShaderProgramManager::getInstance().clearShaderProgramCache();
	TextureManager::getInstance().clearTextureCache();
	SamplerManager::getInstance().clearSamplerCache();


	heightmap.reset();
}
