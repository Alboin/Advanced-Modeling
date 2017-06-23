#include "Shape.h"

Shape::Shape(const string &type, const vec3 &shapeDimensions)
	:active(true), terminal(false), dimensions(shapeDimensions), shapeType(type), name(type), transformations(mat4())
{
	// There is 4 different types of shapes to create.
	if (shapeType.compare("Lot") == 0)
	{
		active = false;
		//terminal = true;
		return;
	}
	else if (shapeType.compare("Cube") == 0)
		generateCube();
	else if (shapeType.compare("Cylinder") == 0)
		generateCylinder();
	else if (shapeType.compare("ConeRoof") == 0)
		generateConeRoof();
	else if (shapeType.compare("PyramidRoof") == 0)
		generatePyramidRoof();
	else
	{
		cout << "ERROR: No shape with name '" << shapeType << "' could be found!" << endl;
		return;
	}

	createVBO();
}

Shape::Shape(const string &shapeName, const vector<vec3> &newVertices, const vector<ivec3> &newIndices, const int texture)
	:active(true), terminal(false), name(shapeName), textureIndex(texture)
{
	vboArray = newVertices;
	indices = newIndices;
	createVBO();

	if (vboArray.size() == 8)
		shapeType = "Face";
}


//Set-functions.

void Shape::setTextureToWindow()
{
	// Set the texture to window and make sure the texture fills the whole face and is not tiled.
	textureIndex = 3;
	vboArray[1] = vec3(0, 0, 0);
	vboArray[3] = vec3(1, 0, 0);
	vboArray[5] = vec3(0, 1, 0);
	vboArray[7] = vec3(1, 1, 0);
	terminal = true;
}

void Shape::setTextureToDoor()
{
	// Set the texture to door and make sure the texture fills the whole face and is not tiled.
	textureIndex = 4;
	vboArray[1] = vec3(0, 0, 0);
	vboArray[3] = vec3(1, 0, 0);
	vboArray[5] = vec3(0, 1, 0);
	vboArray[7] = vec3(1, 1, 0);
	terminal = true;
}

//Functions for creating geometry.

void Shape::generateCube()
{
	float x = dimensions.x, y = dimensions.y, z = dimensions.z;

	vector<vec3> planeVertices;

	//Plane 0
	planeVertices = generatePlane(vec2(dimensions.x, dimensions.y));
	translatePlane(vec3(0, dimensions.y / 2, -dimensions.z / 2), planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	//Plane 1
	planeVertices = generatePlane(vec2(dimensions.x, dimensions.y));
	rotatePlane(vec3(0, 1, 0), 3.14, planeVertices);
	translatePlane(vec3(0, dimensions.y / 2, dimensions.z / 2), planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	//Plane 2
	planeVertices = generatePlane(vec2(dimensions.z, dimensions.y));
	rotatePlane(vec3(0, 1, 0), 3.14 / 2, planeVertices);
	translatePlane(vec3(dimensions.x / 2, dimensions.y / 2, 0), planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	//Plane 3
	planeVertices = generatePlane(vec2(dimensions.z, dimensions.y));
	rotatePlane(vec3(0, 1, 0), -3.14 / 2, planeVertices);
	translatePlane(vec3(-dimensions.x / 2, dimensions.y / 2, 0), planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	//Plane 4
	planeVertices = generatePlane(vec2(dimensions.x, dimensions.z));
	rotatePlane(vec3(1, 0, 0), 3.14 / 2, planeVertices);
	translatePlane(vec3(0, dimensions.y, 0), planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	//Plane 5
	planeVertices = generatePlane(vec2(dimensions.x, dimensions.z));
	rotatePlane(vec3(1, 0, 0), -3.14 / 2, planeVertices);
	addPlane(planeVertices);
	planeVertices.clear();

	translatePlane(vec3(dimensions.x / 2, 0, dimensions.z / 2), vboArray);

	// Set the texture index to wall texture == 1.
	textureIndex = 1;
}

void Shape::generateCylinder()
{
	//terminal = true;
	float x = dimensions.x, y = dimensions.y, z = dimensions.z;

	// Add center vertice for bottom and top.
	vboArray.push_back(vec3(x / 2.0f, 0, z / 2.0f));
	vboArray.push_back(vec3(0.5f, 0.5f, 0));
	vboArray.push_back(vec3(x / 2.0f, y, z / 2.0f));
	vboArray.push_back(vec3(0.5f, 0.5f, 0));

	// Create vertices in a circle around the top and bottom.
	for (float angle = 0; angle <= 2 * 3.14f; angle += 0.1f)
	{
		vboArray.push_back(vec3(sin(angle) * x / 2.0f + x / 2.0f, 0, cos(angle) * z / 2.0f + z / 2.0f));
		vboArray.push_back(vec3(angle / (2 * 3.14) * x * z / 1.0f, 0, 0));
		vboArray.push_back(vec3(sin(angle) * x / 2.0f + x / 2.0f, y, cos(angle) * z / 2.0f + z / 2.0f));
		vboArray.push_back(vec3(angle / (2 * 3.14) * x * z / 1.0f, y, 0));
	}
	// Add last vertices at starting position to complete the cylinder.
	vboArray.push_back(vec3(sin(0) * x / 2.0f + x / 2.0f, 0, cos(0) * z / 2.0f + z / 2.0f));
	vboArray.push_back(vec3(x * z / 1.0f, 0, 0));
	vboArray.push_back(vec3(sin(0) * x / 2.0f + x / 2.0f, y, cos(0) * z / 2.0f + z / 2.0f));
	vboArray.push_back(vec3(x * z / 1.0f, y, 0));

	// Create vertex indexes.
	for (int i = 2; i < vboArray.size() / 2 - 3; i += 2)
	{
		indices.push_back(ivec3(0, i + 2, i)); //Bottom
		indices.push_back(ivec3(1, i + 1, i + 3)); //Top
		indices.push_back(ivec3(i, i + 2, i + 3)); //Side part 1
		indices.push_back(ivec3(i + 3, i + 1, i)); //Side part 2
	}
	// Set the texture index to wall texture == 1.
	textureIndex = 1;
}

void Shape::generateConeRoof()
{
	terminal = true;
	float x = dimensions.x, y = dimensions.y, z = dimensions.z;

	vboArray.push_back(vec3(x / 2.0f, 0, z / 2.0f));
	vboArray.push_back(vec3(0.5f, 0.5f, 0));
	vboArray.push_back(vec3(x / 2.0f, y, z / 2.0f));
	vboArray.push_back(vec3(0.5f, 0.5f, 0));

	for (float angle = 0; angle <= 2 * 3.14f; angle += 0.1f)
	{
		vboArray.push_back(vec3(sin(angle) * (x / 2.0f) + (x / 2.0f), 0, cos(angle) * (z / 2.0f) + (z / 2.0f)));
		vboArray.push_back(vec3((cos(angle) + 1) / 2, (sin(angle) + 1) / 2, 0));
	}
	vboArray.push_back(vec3(sin(0) * (x / 2.0f) + (x / 2.0f), 0, cos(0) * (z / 2.0f) + (z / 2.0f)));
	vboArray.push_back(vec3((cos(0) + 1) / 2, (sin(0) + 1) / 2, 0));


	for (int i = 2; i < vboArray.size() / 2 - 1; i++)
	{
		indices.push_back(ivec3(0, i + 1, i));
		indices.push_back(ivec3(1, i, i + 1));
	}
	// Set the texture index to roof texture == 2.
	textureIndex = 2;
}

void Shape::generatePyramidRoof()
{
	terminal = true;

	vboArray.push_back(vec3(dimensions.x / 2, dimensions.y, dimensions.z / 2));
	vboArray.push_back(vec3(dimensions.x / 2, dimensions.y / 2, 0));
	vboArray.push_back(vec3(0, 0, 0));
	vboArray.push_back(vec3(0, 0, 0));
	vboArray.push_back(vec3(dimensions.x, 0, 0));
	vboArray.push_back(vec3(dimensions.x, 0, 0));
	vboArray.push_back(vec3(0, 0, dimensions.z));
	vboArray.push_back(vec3(dimensions.z, 0, 0));
	vboArray.push_back(vec3(dimensions.x, 0, dimensions.z));
	vboArray.push_back(vec3(0, 0, 0));

	indices.push_back(ivec3(0, 2, 1));
	indices.push_back(ivec3(0, 1, 3));
	indices.push_back(ivec3(0, 3, 4));
	indices.push_back(ivec3(0, 4, 2));
	indices.push_back(ivec3(1, 2, 4));
	indices.push_back(ivec3(1, 4, 3));

	// Set the texture index to roof texture == 0.
	textureIndex = 0;
}

vector<vec3> Shape::generatePlane(const vec2 &sizes, bool window)
{
	vector<vec3> vertices;
	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(sizes.x, 0, 0));
	vertices.push_back(vec3(sizes.x, 0, 0));
	vertices.push_back(vec3(0, sizes.y, 0));
	vertices.push_back(vec3(0, sizes.y, 0));
	vertices.push_back(vec3(sizes.x, sizes.y, 0));
	vertices.push_back(vec3(sizes.x, sizes.y, 0));
	translatePlane(vec3(-sizes.x / 2, -sizes.y / 2, 0), vertices);

	if (window)
	{
		vertices[1] = vec3(0, 0, 0);
		vertices[3] = vec3(1, 0, 0);
		vertices[5] = vec3(0, 1, 0);
		vertices[7] = vec3(1, 1, 0);
	}

	return vertices;
}

//Geometry modification functions.
void Shape::transform(const mat4 trans)
{
	for (int i = 0; i < vboArray.size(); i += 2)
		vboArray[i] = vec3(trans * vec4(vboArray[i], 1.0f));

	transformations = trans * transformations;
}

//Help-functions.

void Shape::addPlane(const vector<vec3> &vertices)
{
	int size = vboArray.size() / 2;
	for (int i = 0; i < vertices.size(); i++)
		vboArray.push_back(vertices[i]);

	indices.push_back(ivec3(size, size + 2, size + 3));
	indices.push_back(ivec3(size, size + 3, size + 1));
}

void Shape::rotatePlane(vec3 rotationAxis, float angle, vector<vec3> &vertices) const
{
	//vec3 planeTranslation = vertices[vertices.size() - 1] / 2.0f;
	for (int i = 0; i < vertices.size(); i += 2)
		vertices[i] = glm::rotate(vertices[i], angle, rotationAxis);
}

void Shape::translatePlane(vec3 translation, vector<vec3> &vertices) const
{
	// This function translates all vertices without saving the translation history to the object.
	for (int i = 0; i < vertices.size(); i += 2)
		vertices[i] = vertices[i] + translation;
}

//Rendering functions.

void Shape::createVBO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vboArray.size(), &vboArray[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec3)*indices.size(), &indices[0], GL_STATIC_DRAW);

	//Vertex position attribute	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex texture coordinate
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Shape::drawShape() const
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vboArray.size(), &vboArray[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, sizeof(ivec3)*indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Shape::is(const string &shapeName) const
{
	if (name == shapeName)
		return true;
	return false;
}
