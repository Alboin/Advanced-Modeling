#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

using namespace std;
using namespace glm;

class Shape
{
public:
	Shape(string shapeType, vec3 dimensions);

private:
	pair<vector<vec3>, vector<ivec3> > generateCube(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateCylinder(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateConeRoof(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generateSquareRoof(float x, float y, float z) const;
	pair<vector<vec3>, vector<ivec3> > generatePyramidRoof(float x, float y, float z) const;

	static vector<GLuint> textures;
protected:
	vector<vec3> vboArray;
	vector<ivec3> indices;
	GLuint textureIndex;
	GLuint VBO, VAO, EBO;
	bool active;
};