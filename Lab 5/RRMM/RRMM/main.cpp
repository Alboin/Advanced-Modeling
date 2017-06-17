#pragma region Includes.

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "shaderLoader.h"
#include "Building.h"

#pragma endregion

using namespace glm;
using namespace std;

#pragma region Global variables.
// Global variables for rendering
glm::mat4 view;
glm::mat4 model = mat4(1.0f);
glm::mat4 projection;
GLuint shaderProgramID;

// Global variables for controls
bool leftMousePressed = false;
double mouseX, mouseY;
int wireframe = 0;
bool exitProgram = false;

// Global variables for the fps-counter
double t0 = 0.0;
int frames = 0;
char titlestring[200];
#pragma endregion

// Function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateMeshUniforms();


int main()
{

	#pragma region initiation
	int windowWidth = 1000;
	int windowHeight = 1000;

	//Starting position of camera
	view = lookAt(vec3(0.0f, 0.1f, 2.0f), vec3(0, 0, 0), vec3(0, 1, 0));
	//Projection matrix
	projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 5.0f);

	//Initiate glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


	//Try to create a window
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FRR", nullptr, nullptr);
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


	// Create and compile the GLSL program from the shaders
	shaderProgramID = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");

	//Register external intpu in GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	#pragma endregion

	Building house = Building("rules.txt");

	//Rendering commands here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glPointSize(5);


	while (!glfwWindowShouldClose(window) && !exitProgram)
	{
		//Checks if any events are triggered (like keyboard or mouse events)
		glfwPollEvents();
		//Update mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (wireframe == 0)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (wireframe == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

		//Draw building
		updateMeshUniforms();
		house.drawBuilding(shaderProgramID);

		//Swap the buffers
		glfwSwapBuffers(window);

		#pragma region fps-counter
		//Show fps in window title
		double t = glfwGetTime();
		// If one second has passed, or if this is the very first frame
		if ((t - t0) > 1.0 || frames == 0)
		{
			double fps = (double)frames / (t - t0);
			sprintf(titlestring, "FRR (%.1f fps)", fps);
			glfwSetWindowTitle(window, titlestring);
			t0 = t;
			frames = 0;
		}
		frames++;
		#pragma endregion

	}

	glfwTerminate();

	return 0;
}


void updateMeshUniforms()
{
	vec3 cameraPos = (inverse(view))[3];

	//Send all matrixes needed to mesh shaders.
	GLuint viewLoc = glGetUniformLocation(shaderProgramID, "viewMatrix");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	GLuint modelLoc = glGetUniformLocation(shaderProgramID, "modelMatrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	GLuint projectiontransLoc = glGetUniformLocation(shaderProgramID, "projectionMatrix");
	glUniformMatrix4fv(projectiontransLoc, 1, GL_FALSE, glm::value_ptr(projection));
	GLint cameraPosLoc = glGetUniformLocation(shaderProgramID, "cameraPos");
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		wireframe++;
		if (wireframe == 3)
			wireframe = 0;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		exitProgram = true;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (leftMousePressed)
	{

		//Rotate around Y-axis
		view = rotate(view, (float)(xpos - mouseX) / 100, vec3(0.0f, 1.0f, 0.0f));

		//Rotate around up/down
		mat3 rotMat(view);
		vec3 transl(view[3]);
		vec3 camera_pos = -transl * rotMat;

		camera_pos = vec3(camera_pos.x, 0.0f, camera_pos.z);
		vec3 temp = normalize(cross(vec3(0.0f, 1.0f, 0.0f), camera_pos));
		view = rotate(view, (float)(ypos - mouseY) / 150, temp);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Zoom in and out through scrolling
	view = scale(view, vec3(1.0 + 0.1*yoffset));

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMousePressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftMousePressed = false;
}