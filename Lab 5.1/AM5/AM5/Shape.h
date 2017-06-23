#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace glm;

class Shape
{
public:
	// Constructors
	Shape(const string &shapeType, const vec3 &shapeDimensions);
	Shape(const string &shapeName, const vector<vec3> &newVertices, const vector<ivec3> &newIndices, const int texture);


	// Rendering functions.
	void drawShape() const;

	// Get-functions.
	int getTextureIndex() const { return textureIndex; };
	vec3 getDimensions() const { return dimensions; };
	vec3 getCoordinateAxes() const { return coordinateAxes; };
	mat4 getTransformations() const { return transformations; };
	vector<vec3> getVBOarray() const { return vboArray; };
	vector<ivec3> getIndexArray() const { return indices; };
	bool isActive() const { return active; };
	bool isTerminal() const { return terminal; };
	bool is(const string &shapeName) const;
	string getName() const { return name; };
	string getShapeType() const { return shapeType; };

	// Set-functions.
	void setInactive() { active = false; };
	void setToTerminal() { terminal = true; };
	void setName(const string &typeName) { name = typeName; };
	void setTextureToWindow();
	void setTextureToDoor();

	//Geometry modification functions.
	void transform(const mat4 trans);

private:
	// Shape properties.
	int textureIndex;
	vec3 dimensions;
	vec3 coordinateAxes;
	mat4 transformations;
	string name;
	string shapeType;
	bool active;
	bool terminal;

	// Rendering variables.
	vector<vec3> vboArray;
	vector<ivec3> indices;
	GLuint VBO, VAO, EBO;

	// Functions for generating geometries.
	void generateCube();
	void generateCylinder();
	void generateConeRoof();
	void generatePyramidRoof();
	vector<vec3> generatePlane(const vec2 &sizes, bool window = false);

	// Help-functions.
	void rotatePlane(vec3 rotationAxis, float angle, vector<vec3> &vertices) const;
	void translatePlane(vec3 translation, vector<vec3> &vertices) const;
	void Shape::addPlane(const vector<vec3> &vertices);

	// Rendering functions.
	void createVBO();


};