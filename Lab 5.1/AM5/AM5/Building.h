#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <iostream>
#include <math.h>
#include <stack>
#include <queue>

#include "stb_image.h"
#include "Shape.h"

using namespace std;
using namespace glm;

class Building
{
public:
	Building(const int config);

	void drawBuilding(const GLuint shaderProgramID) const;

private:

	vector<Shape> allShapes;
	vector<GLuint> textures;

	void grammars(Shape &shape);

	// Variables used in the grammar.
	int configuration;
	int currentShape;
	Shape *currShape;
	vec3 currentTranslation;
	stack<vec3> currentSize;
	stack<queue<mat4>> transformations;
	float currentRandom;

	// All possible functions to be used in the grammar.
	void translate(const vec3 translation);
	void rotate(const vec3 rotationAxis, const float angle);
	void scale(const vec3 scaleSize);
	void create(const string &type);
	void create(const string &type, const vec3 dimensions);
	void separateFaces(const string &newName) { separateFaces(allShapes[currentShape], newName); };
	void separateFaces(Shape &shape, const string &newName);
	void extrudeFace(const float extrude) { extrudeFace(allShapes[currentShape], extrude); };
	void extrudeFace(Shape &shape, const float extrude);
	void subdivide(const char &axis, const vector<double> ratios, const vector<string> typeNames) { subdivide(allShapes[currentShape], axis, ratios, typeNames); };
	void subdivide(Shape &shape, const char &axis, const vector<double> ratios, const vector<string> shapeNames);
	vector<int> subdivideShape(const char &axis, const Shape shape, const vector<double> ratios);
	vector<int> subdivideFace(const char &axis, const Shape shape, const vector<double> ratios);
	void push();
	void pop();

	// Helping functions.
	void loadTextures(vector<string> textureLocations);
	void updateRandom() { currentRandom = (double)rand() / (RAND_MAX); };
	// Returns true with probability "prob".
	bool probability(const float prob) const { return ((double)rand() / (RAND_MAX)) < prob; };
	// Returns a random number between "start" and "end".
	float random(const float start, const float end) const { return start + ((double)rand() / (RAND_MAX)) * (end - start); };
	vector<double> createRatios(const char &axis, vector<double> input, const Shape &shape);

	// Different configurations
	void config1(Shape &shape);
	void config2(Shape &shape);
	void config3(Shape &shape);

};