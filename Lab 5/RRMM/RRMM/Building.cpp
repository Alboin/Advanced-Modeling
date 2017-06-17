#include "Building.h"

Building::Building(string inputFile)
{
	//parseFile(inputFile); //TODO

	vector<string> textureLocations;

	//pair<vector<vec3>, vector<ivec3> > generatedData = generateCube(5, 6, 3);
	//addNewShape(generatedData.first, generatedData.second);

	//generatedData = generateCube(7, 2, 5);
	//addNewShape(generatedData.first, generatedData.second);

	pair<vector<vec3>, vector<ivec3> > generatedData = generateCylinder(5, 5, 5);
	addNewShape(generatedData.first, generatedData.second);
	textureLocations.push_back("textures/wall2.jpg");

	generatedData = generateConeRoof(3, 2, 3);
	addNewShape(translateVertices(generatedData.first, vec3(0,6,0)), generatedData.second);
	textureLocations.push_back("textures/roofCone.jpg");

	//generatedData = generatePyramidRoof(7, 1, 5);
	//addNewShape(translateVertices(generatedData.first, vec3(0, 2, 0)), generatedData.second);

	//generatedData = generateSquareRoof(5, 0.8f, 3);
	//addNewShape(translateVertices(generatedData.first, vec3(0, 6, 0)), generatedData.second);
	Shape hej("Cone", vec3(0));

	loadTextures(textureLocations);

	createVBO();

	//TODO: function for translating vertices

}

void Building::parseFile(string fileName)
{

}

void Building::createVBO()
{
	VAO.resize(vboArray.size());
	VBO.resize(vboArray.size());
	EBO.resize(vboArray.size());
	
	for (int i = 0; i < vboArray.size(); i++)
	{
		glGenVertexArrays(1, &VAO[i]);
		glGenBuffers(1, &VBO[i]);
		glGenBuffers(1, &EBO[i]);

		glBindVertexArray(VAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vboArray[i].size(), &vboArray[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec3)*indices[i].size(), &indices[i][0], GL_STATIC_DRAW);

		//Vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//Vertex texture coordinate
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)(sizeof(vec3)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}

void Building::loadTextures(vector<string> textureLocations)
{
	textures.resize(textureLocations.size());

	for (int i = 0; i < textureLocations.size(); i++)
	{
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		unsigned char *data = stbi_load(textureLocations[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

}


void Building::drawBuilding(const GLuint shaderProgramID) const
{
	glUseProgram(shaderProgramID);
	
	for (int i = 0; i < vboArray.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vboArray[i].size(), &vboArray[i][0], GL_STATIC_DRAW);

		glBindVertexArray(VAO[i]);

		glDrawElements(GL_TRIANGLES, sizeof(ivec3)*indices[i].size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

pair<vector<vec3>, vector<ivec3> > Building::generateCube(float x, float y, float z) const
{
	vector<vec3> vertices;
	vector<ivec3> indices;

	// Add cube vertices
	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(x, 0, 0));
	vertices.push_back(vec3(0, y, 0));
	vertices.push_back(vec3(x, y, 0));

	vertices.push_back(vec3(0, 0, z));
	vertices.push_back(vec3(x, 0, z));
	vertices.push_back(vec3(0, y, z));
	vertices.push_back(vec3(x, y, z));

	// Add cube indices
	indices.push_back(ivec3(0, 2, 1));
	indices.push_back(ivec3(2, 3, 1));

	indices.push_back(ivec3(4, 5, 6));
	indices.push_back(ivec3(5, 7, 6));

	indices.push_back(ivec3(2, 6, 7));
	indices.push_back(ivec3(7, 3, 2));

	indices.push_back(ivec3(4, 0, 5));
	indices.push_back(ivec3(0, 1, 5));

	indices.push_back(ivec3(0, 4, 6));
	indices.push_back(ivec3(0, 6, 2));

	indices.push_back(ivec3(1, 3, 7));
	indices.push_back(ivec3(1, 7, 5));

	return make_pair(vertices, indices);
}

pair<vector<vec3>, vector<ivec3> > Building::generateCylinder(float x, float y, float z) const
{
	vector<vec3> vertices;
	vector<ivec3> indices;

	// Add center vertice for bottom and top.
	vertices.push_back(vec3(x / 2.0f, 0, z / 2.0f));
	vertices.push_back(vec3(0.5f, 0.5f, 0));
	vertices.push_back(vec3(x / 2.0f, y, z / 2.0f));
	vertices.push_back(vec3(0.5f, 0.5f, 0));

	// Create vertices in a circle around the top and bottom.
	for (float angle = 0; angle <= 2 * 3.14f; angle += 0.1f)
	{
		vertices.push_back(vec3(sin(angle) * x / 2.0f + x / 2.0f, 0, cos(angle) * z / 2.0f + z / 2.0f));
		vertices.push_back(vec3(angle / (2 * 3.14) * x * z / 2.0f, 0, 0));
		vertices.push_back(vec3(sin(angle) * x / 2.0f + x / 2.0f, y, cos(angle) * z / 2.0f + z / 2.0f));
		vertices.push_back(vec3(angle / (2 * 3.14) * x * z / 2.0f, y, 0));
	}
	// Add last vertices at starting position to complete the cylinder.
	vertices.push_back(vec3(sin(0) * x / 2.0f + x / 2.0f, 0, cos(0) * z / 2.0f + z / 2.0f));
	vertices.push_back(vec3(x * z / 2.0f, 0, 0));
	vertices.push_back(vec3(sin(0) * x / 2.0f + x / 2.0f, y, cos(0) * z / 2.0f + z / 2.0f));
	vertices.push_back(vec3(x * z / 2.0f, y, 0));

	// Create vertex indexes.
	for (int i = 2; i < vertices.size()/2 - 3; i += 2)
	{
		indices.push_back(ivec3(0, i + 2, i)); //Bottom
		indices.push_back(ivec3(1, i + 1, i + 3)); //Top
		indices.push_back(ivec3(i, i + 2, i + 3)); //Side part 1
		indices.push_back(ivec3(i + 3, i + 1, i)); //Side part 2
	}

	return make_pair(vertices, indices);
}

pair<vector<vec3>, vector<ivec3> > Building::generateConeRoof(float x, float y, float z) const
{
	vector<vec3> vertices;
	vector<ivec3> indices;

	vertices.push_back(vec3(x / 2.0f, 0, z / 2.0f));
	vertices.push_back(vec3(0.5f, 0.5f, 0));
	vertices.push_back(vec3(x / 2.0f, y, z / 2.0f));
	vertices.push_back(vec3(0.5f, 0.5f, 0));

	for (float angle = 0; angle <= 2 * 3.14f; angle += 0.1f)
	{
		vertices.push_back(vec3(sin(angle) * (x / 2.0f) + (x / 2.0f), 0, cos(angle) * (z / 2.0f) + (z / 2.0f)));
		vertices.push_back(vec3((cos(angle) + 1)/2, (sin(angle) + 1)/2, 0));
	}
	vertices.push_back(vec3(sin(0) * (x / 2.0f) + (x / 2.0f), 0, cos(0) * (z / 2.0f) + (z / 2.0f)));
	vertices.push_back(vec3((cos(0) + 1) / 2, (sin(0) + 1) / 2, 0));


	for (int i = 2; i < vertices.size()/2 - 1; i++)
	{
		indices.push_back(ivec3(0, i + 1, i));
		indices.push_back(ivec3(1, i, i + 1));
	}

	return make_pair(vertices, indices);
}

pair<vector<vec3>, vector<ivec3> > Building::generateSquareRoof(float x, float y, float z) const
{
	vector<vec3> vertices;
	vector<ivec3> indices;

	float temp = (x * z / 2.0f) / 10.0f;

	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(x, 0, 0));
	vertices.push_back(vec3(0, 0, z));
	vertices.push_back(vec3(x, 0, z));
	vertices.push_back(vec3(temp, y, temp));
	vertices.push_back(vec3(x - temp, y, temp));
	vertices.push_back(vec3(temp, y, z - temp));
	vertices.push_back(vec3(x - temp, y, z - temp));

	indices.push_back(ivec3(0, 1, 3));
	indices.push_back(ivec3(0, 3, 2));
	indices.push_back(ivec3(4, 6, 7));
	indices.push_back(ivec3(4, 7, 5));

	indices.push_back(ivec3(0, 4, 5));
	indices.push_back(ivec3(5, 1, 0));
	indices.push_back(ivec3(7, 3, 1));
	indices.push_back(ivec3(1, 5, 7));
	indices.push_back(ivec3(3, 7, 6));
	indices.push_back(ivec3(6, 2, 3));
	indices.push_back(ivec3(0, 2, 6));
	indices.push_back(ivec3(6, 4, 0));

	return make_pair(vertices, indices);
}

pair<vector<vec3>, vector<ivec3> > Building::generatePyramidRoof(float x, float y, float z) const
{
	vector<vec3> vertices;
	vector<ivec3> indices;

	// Add cube vertices
	vertices.push_back(vec3(x / 2.0f, y, z / 2.0f));
	vertices.push_back(vec3(0, 0, 0));
	vertices.push_back(vec3(x, 0, 0));
	vertices.push_back(vec3(0, 0, z));
	vertices.push_back(vec3(x, 0, z));

	indices.push_back(ivec3(1, 2, 4));
	indices.push_back(ivec3(1, 4, 3));
	indices.push_back(ivec3(3, 4, 0));
	indices.push_back(ivec3(1, 0, 2));
	indices.push_back(ivec3(2, 0, 4));
	indices.push_back(ivec3(3, 0, 1));

	return make_pair(vertices, indices);
}

vector<vec3> Building::translateVertices(vector<vec3> &vertices, vec3 translation)
{
	for (int i = 0; i < vertices.size(); i+=2)
		vertices[i] = vertices[i] + translation;
	return vertices;
}

void Building::addNewShape(vector<vec3> newVertices, vector<ivec3> newIndices)
{
	int initialSize = vboArray.size() / 2.0f;

	vector<vec3> tempVbo;
	vector<ivec3> tempIndex;

	for (int i = 0; i < newVertices.size(); i++)
		tempVbo.push_back(newVertices[i]);

	for (int i = 0; i < newIndices.size(); i++)
		tempIndex.push_back(newIndices[i] + ivec3(initialSize));

	vboArray.push_back(tempVbo);
	indices.push_back(tempIndex);
}
