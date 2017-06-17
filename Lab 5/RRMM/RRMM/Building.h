
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <math.h>

#include "stb_image.h"
#include "Shape.h"

using namespace std;
using namespace glm;

class Building
{
public:
	Building(string inputFile);
	Building() {}; //Default constructor.
	void drawBuilding(const GLuint shaderProgramID) const;
private:
	vector<vector<vec3>> vboArray;
	vector<vector<ivec3>> indices;
	vector<GLuint> textures;
	vector<GLuint> VBO, VAO, EBO;

	void createVBO();
	void loadTextures(vector<string> textureLocations);
	void parseFile(string fileName);
	pair<vector<vec3>, vector<ivec3> > generateCube(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateCylinder(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateConeRoof(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateSquareRoof(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generatePyramidRoof(float x, float y, float z) const;
	vector<vec3> translateVertices(vector<vec3> &vertices, vec3 translation);
	void addNewShape(vector<vec3> newVertices, vector<ivec3> newIndices);
};