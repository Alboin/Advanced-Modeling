#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vector>
#include <string>

using namespace std;
using namespace glm;

class Shape
{
public:
	Shape(const string &shapeType, vec3 shapeDimensions);
	// Constructor for face-extracion.
	Shape(const vector<vec3> &vertices, const vector<ivec3> &index, int texture, vec3 translation, const string &typeName);
	// Constructer for face-extrusion.
	Shape(const vector<vec3> &vertices, const vector<ivec3> &index, int texture, vec3 translation, bool extruded);
	
	void drawShape() const;

	// Get-functions.
	int getTextureIndex() const { return textureIndex; };
	vec3 getDimensions() const { return dimensions; };
	vec3 getCoordinateAxes() const { return coordinateAxes; };
	const vector<pair<vec3, float> > &getRotations() const { return appliedRotations; };
	vec3 getTranslatation() const { return translationSum; };
	vector<vec3> getVBOarray() const { return vboArray; };
	vector<ivec3> getIndexArray() const { return indices; };
	bool isActive() const { return active; };
	bool isTerminal() const { return terminal; };
	string getType() const { return type; };

	// Set-functions.
	void setInactive() { active = false; };
	void setTextureToWindow();
	void setTextureToDoor();
	void setType(const string &typeName) { type = typeName; };
	void setToTerminal() { terminal = true; };

	bool is(const string &shapeType) const { if (shapeType.compare(type) == 0) return true; return false; };

	void translate(vec3 translation);
	void rotate(vec3 rotationAxis, float angle);
	void scale(vec3 scaleFactors);

private:
	// Functions for generating geometries.
	void generateCube();
	void generateCylinder();
	void generateConeRoof();
	void generatePyramidRoof();
	void generateWindow();
	vector<vec3> generatePlane(const vec2 &sizes, bool window = false);
	void rotatePlane(vec3 rotationAxis, float angle, vector<vec3> &vertices) const;
	void translatePlane(vec3 translation, vector<vec3> &vertices) const;
	void Shape::addPlane(const vector<vec3> &vertices);


	void createVBO();

	// Private variables.
	vector<vec3> vboArray;
	vector<ivec3> indices;
	GLuint VBO, VAO, EBO;
	vec3 dimensions;
	vec3 translationSum;
	vector<pair<vec3, float> > appliedRotations;
	vec3 coordinateAxes;
	int textureIndex;
	string type;
	bool active;
	bool terminal = false;
};