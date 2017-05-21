#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "taulaMC.hpp"


using namespace std;


class MarchingCubes
{
public:
	MarchingCubes(string inputFileName, GLuint shaderProgramID);

	void changeThreshold(float newThreshold);
	void draw(GLuint shaderProgramID, GLuint diffuseTexID, GLuint specularTexID);

	pair<float, float> dataMaxMin;
	float currentThreshold;
	float stepSize;
	int dimensionSize;

private:

	// Rendering variables.
	GLuint VBO, VAO, EBO;
	GLuint shaderID;
	vector<glm::vec3> vertices;
	vector<glm::ivec3> indices;
	vector<glm::vec3> vboArray;
	vector<set<int>> verticeNeighbours; //this one is computed in "computeNumberOfShells()-function"

	// Marching cubes variables.
	float wishedStepSize = 0.01f;
	vector<float> inputData;
	int nShells;

	// Table with triangle lookup.
	MCcases getTriangleConfig;

	// Functions for creating the marching cubes triangles.
	void MarchingCubes::createTriangles();
	double getDataAtPoint(int i, int j, int k);
	// Creates the vertices for one triangle.
	vector<glm::vec3> generateVertices(vector<int> edges, int i, int j, int k);
	void createTriangles(vector<int> edges, int i, int j, int k);

	unordered_map<long long int, int> verticePointers;
	long long int hash(int i, int j, int k, int e);


	void createVBOarray();
	void createBuffers(GLuint shaderProgramID);
	void computeNormals();
	void mergeVerticeDuplicates();
	long long int hashFunction(const glm::vec3 &vertex) const;
	int computeNumberOfShells();

};