
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
	void drawBuilding(const GLuint shaderProgramID) const;

private:

	vector<Shape> allShapes;
	vector<GLuint> textures;

	void loadTextures(vector<string> textureLocations);

	vector<int> subdivide(const char &axis, Shape &shape, const vector<float> ratios, const vector<string> typeNames);
	vector<int> subdivide2(const char &axis, const Shape shape, const vector<float> ratios);
	vector<int> subdivideFace(const char &axis, const Shape shape, const vector<float> ratios);
	vector<int> subDivideCylinderHorizontal(const Shape shape, const vector<float> ratios, const vector<string> typeNames);
	vector<int> separateFaces(Shape &shape, const string &typeName);
	vector<int> extrudeFace(Shape &shape, float extrude);
	vector<float> createRatios(const char &axis, vector<float> input, const Shape &shape);
};