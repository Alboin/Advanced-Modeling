#include "Shape.h"

#include <iostream>

Shape::Shape(string shapeType, vec3 dimensions)
	:active(true)
{
	if (shapeType.compare("Cone"))
	{
		cout << "Hej1";
	}
	else if (shapeType.compare("Cube"))
	{
		cout << "Hej2";
	}

}

//pair<vector<vec3>, vector<ivec3> > generateCube(float x, float y, float z) const
//pair<vector<vec3>, vector<ivec3> > generateCylinder(float x, float y, float z) const;
//pair<vector<vec3>, vector<ivec3> > generateConeRoof(float x, float y, float z) const;
//pair<vector<vec3>, vector<ivec3> > generateSquareRoof(float x, float y, float z) const;
//pair<vector<vec3>, vector<ivec3> > generatePyramidRoof(float x, float y, float z) const;