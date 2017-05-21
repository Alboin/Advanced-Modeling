#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/src/SOIL.h>

#include <iostream>
#include <vector>
#include <string>
#include <math.h>

#include "shaderLoader.h"
#include "plyloader.h"
#include "PLYDrawer.h"
#include "CubeMap.h"
#include "MarchingCubes.h"

//using namespace glm;
using namespace std;

// Global variables for rendering and controls
glm::mat4 view;
MarchingCubes* cubesPointer;
bool leftMousePressed = false;
bool exitProgram = false;
double mouseX, mouseY;
int wireframe = 0;
int roughness = 0;
int maxRoughness = 6;
float fresnel = 0.4f;
bool renderBRDF = false;

// Global variables for the fps-counter
double t0 = 0.0;
int frames = 0;
char titlestring[200];

// Function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main()
{

	#pragma region initiation
	int windowWidth = 1200;
	int windowHeight = 800;

	//Starting position of camera
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//Initiate glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Try to create a window
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "AM", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Initiate glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//Tell OpenGL the size of the viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);

	// Create and compile the GLSL program from the shaders
	GLuint shaderProgramID = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");
	GLuint skyboxShaderID = LoadShaders("cubemapVert.glsl", "cubemapFrag.glsl");

	//Register external intpu in GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	#pragma endregion


	//Create cubemap
	GLuint VBO_map, VAO_map, EBO_map;
	CubeMap skybox(VBO_map, VAO_map, EBO_map, maxRoughness);

	//Create MarchingCubes
	cubesPointer = new MarchingCubes("models/Blooby.txt" ,shaderProgramID);

	glm::vec3 lightPos;

	while (!glfwWindowShouldClose(window) && !exitProgram)
	{
		//Checks if any events are triggered (like keyboard or mouse events)
		glfwPollEvents();
		//Update mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Set the lightPos to be the same as the camera position.
		lightPos = (inverse(view))[3];

		//Rendering commands here
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(5);


		if (wireframe == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else if(wireframe == 2)
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//RENDERING MESH HERE
		cubesPointer->draw(shaderProgramID, skybox.textures[maxRoughness], skybox.textures[roughness + 1]);


		#pragma region MVP-matrixes for mesh
		//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1/mesh.height, 1/mesh.height, 1/mesh.height));
		//Center the model at origo.
		//model = glm::translate(model, glm::vec3(0, -mesh.height/2 - mesh.minPos.y, 0));
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.0f / float(cubesPointer->dimensionSize)));
		//glm::mat4 model = glm::mat4((1.0f / float(cubesPointer->dimensionSize)));
		model = glm::translate(model, glm::vec3(float(cubesPointer->dimensionSize) / -2.0f));
		glm::mat4 projection;
		projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		glm::vec3 cameraPos = (inverse(view))[3];
		#pragma endregion

		#pragma region Uniforms for mesh
		//Send all matrixes needed to mesh shaders.
		GLuint viewtransLoc1 = glGetUniformLocation(shaderProgramID, "viewMesh");
		glUniformMatrix4fv(viewtransLoc1, 1, GL_FALSE, glm::value_ptr(view));
		GLuint modeltransLoc1 = glGetUniformLocation(shaderProgramID, "modelMesh");
		glUniformMatrix4fv(modeltransLoc1, 1, GL_FALSE, glm::value_ptr(model));
		GLuint projectiontransLoc1 = glGetUniformLocation(shaderProgramID, "projectionMesh");
		glUniformMatrix4fv(projectiontransLoc1, 1, GL_FALSE, glm::value_ptr(projection));
		GLint lightPosLoc1 = glGetUniformLocation(shaderProgramID, "lightPos");
		glUniform3f(lightPosLoc1, lightPos.x, lightPos.y, lightPos.z);
		GLint fresnelLoc = glGetUniformLocation(shaderProgramID, "fresnel");
		glUniform1f(fresnelLoc, fresnel);
		GLint cameraPosLoc1 = glGetUniformLocation(shaderProgramID, "cameraPos");
		glUniform3f(cameraPosLoc1, cameraPos.x, cameraPos.y, cameraPos.z);
		GLint brdfLoc = glGetUniformLocation(shaderProgramID, "BRDF");
		glUniform1i(brdfLoc, renderBRDF);
		#pragma endregion

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//RENDERING SKYBOX HERE
		skybox.drawCubeMap(skyboxShaderID);

		#pragma region model-matrix for cube
		//glm::mat4 cubeModel(1.0f);
		glm::mat4 cubeModel = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		//vec3 camerapos = inverse(view)[3];
		//cubeModel = translate(cubeModel, camerapos);
		glm::mat4 cubeView = glm::mat4(glm::mat3(view));
		#pragma endregion

		#pragma region Uniforms for skybox
		//Send all matrixes needed to cube shaders.
		GLuint viewtransLoc2 = glGetUniformLocation(skyboxShaderID, "viewCube");
		glUniformMatrix4fv(viewtransLoc2, 1, GL_FALSE, glm::value_ptr(cubeView));
		GLuint modeltransLoc2 = glGetUniformLocation(skyboxShaderID, "modelCube");
		glUniformMatrix4fv(modeltransLoc2, 1, GL_FALSE, glm::value_ptr(cubeModel));
		GLuint projectiontransLoc2 = glGetUniformLocation(skyboxShaderID, "projectionCube");
		glUniformMatrix4fv(projectiontransLoc2, 1, GL_FALSE, glm::value_ptr(projection));
		#pragma endregion


		//Swap the buffers
		glfwSwapBuffers(window);

		#pragma region fps-counter
		//Show fps in window title
		double t = glfwGetTime();
		// If one second has passed, or if this is the very first frame
		if ((t - t0) > 1.0 || frames == 0)
		{
			double fps = (double)frames / (t - t0);
			sprintf(titlestring, "AM (%.1f fps)", fps);
			glfwSetWindowTitle(window, titlestring);
			t0 = t;
			frames = 0;
		}
		frames++;
		#pragma endregion

	}

	glfwTerminate();
	//plymodel.FreeMemory();
	delete cubesPointer;

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//By pressing D/S the user can make the model more diffuse/specular
	if (key == GLFW_KEY_D && action == GLFW_PRESS && roughness < maxRoughness)
		roughness++;
	else if (key == GLFW_KEY_S && action == GLFW_PRESS && roughness > 0)
		roughness--;

	//By pressing W/E the user can decrease/increase the fresnel constant
	if (key == GLFW_KEY_W)
		fresnel -= 0.05;
	else if (key == GLFW_KEY_E)
		fresnel += 0.05;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		wireframe++;
		if (wireframe > 2)
			wireframe = 0;
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (renderBRDF)
			renderBRDF = false;
		else
			renderBRDF = true;
	}

	// Change the threshold value
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		cubesPointer->changeThreshold(cubesPointer->currentThreshold + cubesPointer->stepSize);
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		cubesPointer->changeThreshold(cubesPointer->currentThreshold - cubesPointer->stepSize);

	// Change the step-size of threshold value
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		cout << "Stepsize changed from " << cubesPointer->stepSize;
		cubesPointer->stepSize *= 0.9f;
		cout << " to " << cubesPointer->stepSize << endl;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		cout << "Stepsize changed from " << cubesPointer->stepSize;
		cubesPointer->stepSize *= 1.1f;
		cout << " to " << cubesPointer->stepSize << endl;
	}


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		exitProgram = true;

}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (leftMousePressed)
	{
		//Rotate around Y-axis
		view = glm::rotate(view, (float)(xpos - mouseX) / 100, glm::vec3(0.0f, 1.0f, 0.0f));

		//Rotate around up/down
		glm::mat3 rotMat(view);
		glm::vec3 transl(view[3]);
		glm::vec3 camera_pos = -transl * rotMat;

		camera_pos = glm::vec3(camera_pos.x, 0.0f, camera_pos.z);
		glm::vec3 temp = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera_pos));
		view = rotate(view, (float)(ypos - mouseY) / 150, temp);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Zoom in and out through scrolling
	view = glm::scale(view, glm::vec3(1.0 + 0.1*yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMousePressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftMousePressed = false;
}