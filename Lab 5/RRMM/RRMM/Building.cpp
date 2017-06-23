#include "Building.h"

Building::Building(string inputFile)
{
	// Add all textures.
	vector<string> textureLocations;
	textureLocations.push_back("textures/wall1.jpg");
	textureLocations.push_back("textures/wall2.jpg");
	textureLocations.push_back("textures/roofCone.jpg");
	textureLocations.push_back("textures/window.jpg");
	textureLocations.push_back("textures/door.jpg");
	loadTextures(textureLocations);

	allShapes.push_back(Shape("Cube", vec3(6, 5, 6)));
//	allShapes.push_back(Shape("Cube", vec3(5, 10, 3)));
//	allShapes[allShapes.size() - 1].rotate(vec3(0,1,0), 3.14 * 0.5);
//	allShapes[allShapes.size() - 1].translate(vec3(20, 10, 0));

	//allShapes.push_back(Shape("Cylinder", vec3(3, 5, 3)));

	bool moreToDo = true;
	// Run loop until there is no more non-terminals.
	while (moreToDo)
	{
		moreToDo = false;

		int nShapes = allShapes.size();
		// Go through all objects and apply given rules.
		for (int i = 0; i < nShapes; i++)
		{
			Shape* shape = &allShapes[i];
			// If the shape is terminal, skip it.
			if (shape->isTerminal() || !shape->isActive())
				continue;


			if (shape->is("Cube"))
			{
				cout << "Cube!";
				subdivide('Y', *shape, { 2,-0.2f,-1,-1,-1 }, { "Bottom", "Ledge", "Floor2", "Floor1", "Floor2" });
				allShapes.push_back(Shape("PyramidRoof", (shape->getDimensions() * 1.1f) - vec3(0, 4, 0)));
				Shape* roof = &allShapes[allShapes.size() - 1];
				// Translate roof to top of cube.
				allShapes[allShapes.size() - 1].translate(-roof->getDimensions() / 2.0f + shape->getDimensions() / 2.0f + vec3(0, shape->getDimensions().y / 2.0f + roof->getDimensions().y / 2.0f, 0));
			}

			else if (shape->is("Cylinder"))
			{
				subdivide('X', *shape, { -1,-1,-1,-1, 2 }, { "Window", "Door", "Wall", "Door", "Wall" });
			}

			else if (shape->is("Floor1"))
				separateFaces(*shape, "Facade1");

			else if (shape->is("Floor2"))
				separateFaces(*shape, "Facade2");

			else if (shape->is("Bottom"))
				separateFaces(*shape, "BottomFace");

			else if (shape->is("BottomFace"))
				subdivide('X', *shape, {-2, 1.5, -1}, {"Wall", "Door", "Wall"});

			else if (shape->is("Facade1"))
				extrudeFace(*shape, 0.1f);

			else if (shape->is("Facade2"))
				subdivide('X', *shape, { -1,-1,-1,-1,-1 }, { "Wall", "Window", "Wall", "Window", "Wall" });

			else if (shape->is("Ledge"))
				separateFaces(*shape, "LedgeFace");

			else if (shape->is("LedgeFace"))
			{
				shape->setToTerminal();
				extrudeFace(*shape, 0.2f);
			}

			else if (shape->is("Door"))
			{
				shape->setTextureToDoor();
				extrudeFace(*shape, 0.1f);
				//cout << "DÖRR!!!" << endl;
			}
			
			else if (shape->is("Wall"))
				shape->setToTerminal();
			
			else if (shape->is("Window"))
			{
				shape->setTextureToWindow();
				extrudeFace(*shape, 0.05);
				//cout << "FÖNSTER!!!" << endl;
			}
			
			else
				cout << "No rule specified for: " << shape->getType() << endl;
		
		}

		// Check if there is non-terminal shapes left.
		for (int i = 0; i < allShapes.size(); i++)
		{
			// Remove any non-active shape as they will not be rendered anyways.
			if (!allShapes[i].isActive())
			{
				allShapes.erase(allShapes.begin() + i);
				i--;
				continue;
			}
			if (!allShapes[i].isTerminal())
				moreToDo = true;
		}


		//for (int i = 0; i < allShapes.size(); i++)
		//	cout << i << ": " <<allShapes[i].getType() << endl;
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
	
	for (auto &shape : allShapes)
	{
		if (!shape.isActive())
			continue;

		glBindTexture(GL_TEXTURE_2D, textures[shape.getTextureIndex()]);

		shape.drawShape();
	}
}

vector<int> Building::subdivide(const char &axis, Shape &shape, const vector<float> ratios, const vector<string> typeNames)
{
	int temp = allShapes.size();

	// Set the old shape as inactice, preventing it from getting drawn.
	vector<int> indices;
	// Depending on wether the shape is a single face, a cube or a cylinder, different functions needs to be called.
	if (shape.getVBOarray().size() == 8)
		indices = subdivideFace(axis, shape, createRatios(axis, ratios, shape));
	else if (shape.is("Cylinder") && (axis == 'X' || axis == 'x'))
		indices = subDivideCylinderHorizontal(shape, createRatios(axis, ratios, shape), typeNames);
	else
		indices = subdivide2(axis, shape, createRatios(axis, ratios, shape));

	// Unless the shape is a cylinder to be divided along the X-axis, set it to inactive.
	if (!(shape.is("Cylinder") && (axis == 'X' || axis == 'x')))
		shape.setInactive();
	else
		shape.setToTerminal();

	// Give the new shapes their newn names.
	for (int i = 0; i < indices.size(); i++)
		allShapes[indices[i]].setType(typeNames[i]);	

	// Return the indices of the new shapes in allShapes.
	return indices;

}

vector<int> Building::subDivideCylinderHorizontal(const Shape shape, const vector<float> ratios, const vector<string> typeNames)
{
	vector<int> indicesOfNewShapes;

	float circumreference = 2.0 * 3.14 * sqrt((pow(shape.getDimensions()[0], 2) + pow(shape.getDimensions()[2], 2)) / 2.0f);

	cout << "Circ: " << circumreference << endl;

	cout << "Ratios length: " << ratios.size() << ", typenames: " << typeNames.size() << endl;

	cout << "Ratios: ";
	for (int i = 0; i < ratios.size(); i++)
		cout << ratios[i] << ", ";
	cout << endl;

	for (int i = 0; i < typeNames.size(); i++)
	{
		if (typeNames[i].compare("Window") != 0 && typeNames[i].compare("Door") != 0)
			continue;

		// Calculate the position to place a possible Window or Door.
		float previous = 0, next = 1;
		if (i > 0)
			previous = ratios[i - 1];
		if (i < ratios.size())
			next = ratios[i];
		float angle = (next - previous) * 3.14 * 2.0;
		cout << "Angle: " << angle << endl;
		cout << "Prev: " << previous << ", next: " << next << endl;
		vec3 position = normalize(vec3(sin(angle) * shape.getDimensions().x, 0, cos(angle) * shape.getDimensions().z));
		//vec2 size = vec2(angle * sqrt((pow(shape.getDimensions()[0], 2) + pow(shape.getDimensions()[2], 2)) / 2.0f), shape.getDimensions().y);
		
		// Create vertices.
		float cylinderYmin = shape.getVBOarray()[0].y;
		float cylinderYmax = shape.getVBOarray()[2].y;

		float xStartPos = sin(previous * 3.14 * 2.0) * shape.getDimensions().x / 2.0 + shape.getDimensions().x / 2.0;
		float xEndPos = sin(next * 3.14 * 2.0) * shape.getDimensions().x / 2.0 + shape.getDimensions().x / 2.0;

		float zStartPos = cos(previous * 3.14 * 2.0) * shape.getDimensions().z / 2.0 + shape.getDimensions().z / 2.0;
		float zEndPos = cos(next * 3.14 * 2.0) * shape.getDimensions().z / 2.0 + shape.getDimensions().z / 2.0;

		float avgLength = (shape.getDimensions().x + shape.getDimensions().z) / 2.0;

		vector<vec3> vertices;
		vertices.push_back(vec3(xStartPos, cylinderYmin, zStartPos));
		vertices.push_back(vec3(0, 0, 0));
		vertices.push_back(vec3(xEndPos, cylinderYmin, zEndPos));
		vertices.push_back(vec3(1, 0, 0));
		vertices.push_back(vec3(xStartPos, cylinderYmax, zStartPos));
		vertices.push_back(vec3(0, 1, 0));
		vertices.push_back(vec3(xEndPos, cylinderYmax, zEndPos));
		vertices.push_back(vec3(1, 1, 0));

		vector<ivec3> indices;
		indices.push_back(ivec3(0, 2, 3));
		indices.push_back(ivec3(0, 3, 1));

		//vec3 normal = normalize(cross(vertices[0] - vertices[2], vertices[0] - vertices[4]));
		//float radius = sqrt(pow(sin(angle) * shape.getDimensions().x / 2.0, 2) + pow(cos(angle) * shape.getDimensions().z / 2.0, 2));
		//float difference = abs(radius - length((vertices[0] + vertices[2]) / 2.0f));
		//for (int i = 0; i < vertices.size(); i += 2)
		//{
		//	vertices[i] += normal * difference;
		//}

		int texture = -1;
		if (typeNames[i].compare("Window") == 0)
			texture = 3;
		else if (typeNames[i].compare("Door") == 0)
			texture = 4;

		//Create new object
		allShapes.push_back(Shape(vertices, indices, texture, vec3(0), typeNames[i]));
		indicesOfNewShapes.push_back(allShapes.size() - 1);
	}
	return indicesOfNewShapes;
}


vector<int> Building::subdivideFace(const char &axis, const Shape shape, const vector<float> ratios)
{
	vector<int> indicesOfNewShapes;
	string shapeType = shape.getType();
	vector<pair<vec3, float> > shapeRotations = shape.getRotations();
	
	for (int i = 0; i <= ratios.size(); i++)
	{
		vector<vec3> oldVertices = shape.getVBOarray();
		vector<vec3> newVertices;

		// Set shape start and end-points.
		float start = 0;
		float end = 1;
		if (i != 0)
			start = ratios[i - 1];
		if (i != ratios.size())
			end = ratios[i];

		if (axis == 'Y' || axis == 'y')
		{
			float yMin = oldVertices[0].y, yMax = oldVertices[4].y;
			for (int i = 0; i < oldVertices.size(); i++)
			{
				vec3 newVertex;
				if(i < 4)
					newVertex = vec3(oldVertices[i].x, yMin + start * (yMax - yMin), oldVertices[i].z);
				else
					newVertex = vec3(oldVertices[i].x, yMin + end * (yMax - yMin), oldVertices[i].z);

				newVertices.push_back(newVertex);
			}
		}
		else if (axis == 'X' || axis == 'x')
		{
			float xMin = oldVertices[0].x, xMax = oldVertices[2].x;
			float zMin = oldVertices[0].z, zMax = oldVertices[2].z;
			float texMin = oldVertices[1].x, texMax = oldVertices[3].x;
			for (int i = 0; i < oldVertices.size(); i++)
			{
				vec3 newVertex;
				if (i == 0 || i == 4)
					newVertex = vec3(xMin + start * (xMax- xMin), oldVertices[i].y, zMin + start * (zMax - zMin));
				else if(i == 1 || i == 5)
					newVertex = vec3(texMin + start * (texMax - texMin), oldVertices[i].y, 0);
				else if (i == 3 || i == 7)
					newVertex = vec3(texMin + end * (texMax - texMin), oldVertices[i].y, 0);
				else
					newVertex = vec3(xMin + end * (xMax - xMin), oldVertices[i].y, zMin + end * (zMax - zMin));

				newVertices.push_back(newVertex);
			}
		}

		allShapes.push_back(Shape(newVertices, shape.getIndexArray(), shape.getTextureIndex(), shape.getTranslatation(), shape.getType()));
		indicesOfNewShapes.push_back(allShapes.size() - 1);
	}

	return indicesOfNewShapes;

}

vector<int> Building::subdivide2(const char &axis, const Shape shape, const vector<float> ratios)
{
	vector<int> indicesOfNewShapes;

	string shapeType = shape.getType();
	vector<pair<vec3, float> > shapeRotations = shape.getRotations();

	for (int i = 0; i <= ratios.size(); i++)
	{
		float x1 = 0, y1 = 0, z1 = 0;
		float x2 = 1, y2 = 1, z2 = 1;

		// Set shape start and end-points.
		float start = 0;
		float end = 1;
		if (i != 0)
			start = ratios[i - 1];
		if (i != ratios.size())
			end = ratios[i];

		// Change the right axis accordingly to the start/end-points.
		if (axis == 'X' || axis == 'x')
		{
			x1 = start;
			x2 = end;
		}
		else if (axis == 'Y' || axis == 'y')
		{
			y1 = start;
			y2 = end;
		}
		else if (axis == 'Z' || axis == 'z')
		{
			z1 = start;
			z2 = end;
		}
		else
			std::cout << "ERROR: The given axis '" << axis << "' could not be recognized!" << endl;

		// Calculate the new dimensions for the sub-shape.
		vec3 newDimensions = vec3(shape.getDimensions().x * (x2 - x1), shape.getDimensions().y * (y2 - y1), shape.getDimensions().z * (z2 - z1));
		allShapes.push_back(Shape(shapeType, newDimensions));
		// Rotate the sub-shape according to its parent.
		for (int i = 0; i < shapeRotations.size(); i++)
			allShapes[allShapes.size() - 1].rotate(shapeRotations[i].first, shapeRotations[i].second);
		// Translate the sub-shape to it's correct position.
		allShapes[allShapes.size() - 1].translate(vec3(shape.getDimensions().x * x1, shape.getDimensions().y * y1, shape.getDimensions().z * z1));

		indicesOfNewShapes.push_back(allShapes.size() - 1);
	}
	return indicesOfNewShapes;
}

vector<int> Building::separateFaces(Shape &shape, const string &typeName)
{
	//if (shape.getType().compare("Cube") != 0)
	//	cout << "ERROR: Separation of faces for shapes other than Cubes not possible!" << endl;

	vector<int> newFacesIndices;

	shape.setInactive();
	vector<vec3> vboCopy = shape.getVBOarray();
	vec3 dim = shape.getDimensions();
	vec3 trans = shape.getTranslatation();
	string newName = typeName;

	vector<ivec3> newIndices;
	newIndices.push_back(ivec3(0, 2, 3));
	newIndices.push_back(ivec3(0, 3, 1));

	for (int i = 0; i < 6; i++)
	{
		vec3 dimensions(0);
		//if(i == 0)
		//	dimensions = vec3(dim.x)

		vector<vec3> newVbo(vboCopy.begin() + i * 8, vboCopy.begin() + (i + 1) * 8);
		allShapes.push_back(Shape(newVbo, newIndices, shape.getTextureIndex(), trans + dimensions, typeName));
		
		// If the extracted face is a floor, make it terminal since our subdivision-functions dont support them.
		if (i == 4 || i == 5)
			allShapes[allShapes.size() - 1].setToTerminal();

		newFacesIndices.push_back(allShapes.size() - 1);
	}

	return newFacesIndices;
}

vector<int> Building::extrudeFace(Shape &shape, float extrude)
{
	vector<int> newFacesIndices;
	shape.setInactive();

	vector<vec3> newVbo;
	vector<ivec3> newIndices;
	vector<vec3> oldVertices = shape.getVBOarray();

	vec3 normal = normalize(cross(oldVertices[2] - oldVertices[0], oldVertices[4] - oldVertices[0]));

	// Extrude the face in both directions of the face-normal with the given size
	for (int a = -1; a < 2; a += 2)
	{
		vec3 movement = normal * extrude * (float)a;
		for (int i = 0; i < oldVertices.size(); i++)
		{
			if (i % 2 == 1)
				newVbo.push_back(oldVertices[i]);
			else
				newVbo.push_back(oldVertices[i] + movement);
		}
	}

	newIndices.push_back(ivec3(0, 2, 3));
	newIndices.push_back(ivec3(0, 3, 1));
	
	newIndices.push_back(ivec3(4, 6, 7));
	newIndices.push_back(ivec3(4, 7, 5));

	newIndices.push_back(ivec3(5, 7, 3));
	newIndices.push_back(ivec3(5, 3, 1));

	newIndices.push_back(ivec3(4, 6, 2));
	newIndices.push_back(ivec3(4, 2, 0));

	newIndices.push_back(ivec3(6, 2, 7));
	newIndices.push_back(ivec3(2, 3, 7));

	newIndices.push_back(ivec3(5, 4, 0));
	newIndices.push_back(ivec3(5, 1, 0));

	vec3 temp = shape.getTranslatation() + shape.getDimensions() + normal * extrude * 2.0f;
	allShapes.push_back(Shape(newVbo, newIndices, shape.getTextureIndex(), temp, true));
	newFacesIndices.push_back(allShapes.size() - 1);
	return newFacesIndices;
}

vector<float> Building::createRatios(const char &axis, vector<float> input, const Shape &shape)
{
	vec3 axes = shape.getCoordinateAxes();
	vec3 dim = shape.getDimensions();

	// We need the length of the axis to divide by to determine the absolute ratios further down.
	float lengthOfAxis = 0;
	if (axis == 'X' || axis == 'x')
		lengthOfAxis = length(axes[0] * dim.x);
	else if (axis == 'Y' || axis == 'y')
		lengthOfAxis = length(axes[1] * dim.y);
	else if (axis == 'Z' || axis == 'z')
		lengthOfAxis = length(axes[2] * dim.z);

	// If the shape is just a face, calculate the length differently.
	if (shape.getVBOarray().size() == 8)
	{
		if (axis == 'X' || axis == 'x')
			lengthOfAxis = length(shape.getVBOarray()[0] - shape.getVBOarray()[2]);
		else if (axis == 'Y' || axis == 'y')
			lengthOfAxis = length(shape.getVBOarray()[0] - shape.getVBOarray()[4]);
		else
			cout << "ERROR: Cannot subdivide a 2D-face by Z-axis!" << endl;
	}

	// If the shape is a cylinder to be subdivided like a face, calculate the length differently.
	if (shape.is("Cylinder") && (axis == 'X' || axis == 'x'))
		// Calculate the cylinder circumference.
		lengthOfAxis = 2.0 * 3.14 * sqrt((pow(shape.getDimensions()[0],2) + pow(shape.getDimensions()[2],2)) / 2.0f);

	vector<float> relatives;
	vector<float> absolutes;
	for (int i = 0; i < input.size(); i++)
	{
		// Relative lenghts are done with negative values.
		if (input[i] < 0)
		{
			relatives.push_back(input[i]);
			continue;
		}
		// Since the ratios need to be between 0 and 1 we need to calculate how much of the total length the absolutes are.
		absolutes.push_back(input[i] / lengthOfAxis);
	}

	// Sum of all the absolute values needed to calculate the relative.
	float sumOfAbsolutes = 0;
	for (int i = 0; i < absolutes.size(); i++)
		sumOfAbsolutes += absolutes[i];

	if (sumOfAbsolutes > 1)
		cout << "ERROR: The given absolute values for the subdivision is larger than the length of the object!" << endl;

	int sumOfRelatives = 0;
	for (int i = 0; i < relatives.size(); i++)
		sumOfRelatives -= relatives[i];

	float ratioPerUnit = (1.0 - sumOfAbsolutes) / (float)sumOfRelatives;

	// Calculate the final ratios.
	vector<float> finalRatios;
	int absoluteIndex = 0, relativeIndex = 0;
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] < 0)
			finalRatios.push_back(-relatives[relativeIndex++] * ratioPerUnit);
		else
			finalRatios.push_back(absolutes[absoluteIndex++]);
	}

	// The final ratios needs to be formatted so that each new ratio is the sum of itself and the previous ones.
	// for example (0.3, 0.3, 0.4) would be (0.3, 0.6) which is the two points to split at to get (0.3, 0.3, 0.4).
	vector<float> formattedRatios;
	for (int i = 0; i < finalRatios.size() - 1; i++)
	{
		float sumOfRatios = 0.0;
		for (int j = 0; j <= i; j++)
			sumOfRatios += finalRatios[j];
		formattedRatios.push_back(sumOfRatios);
	}

	return formattedRatios;
}


//vector<float> ratios;
//ratios.push_back(0.3f);
//ratios.push_back(0.8f);

//allShapes.push_back(Shape("Window", vec3(2, 1.5, 0)));
//allShapes[0].translate(vec3(0, 0, -1));
////allShapes.push_back(Shape("Cylinder", vec3(5, 2, 5)));
////allShapes.push_back(Shape("ConeRoof", vec3(4, 5, 2)));

//allShapes.push_back(Shape("Cube", vec3(1, 2, 2.5)));

//vector<float> temp;
//temp.push_back(-3);
//temp.push_back(-3);
//temp.push_back(-1);
//temp.push_back(0.6);
//temp.push_back(0.6);
////temp.push_back(-1);
////temp.push_back(-2);
//vector<float> ratios2 = createRatios('Y', temp, allShapes[1]);

//allShapes[1].rotate(vec3(0, 1, 0), 3.14 * 0.05);

//separateFaces(allShapes[1]);
////allShapes[2].rotate(vec3(0, 1, 0), 3.14 * 0.9);
////allShapes[3].rotate(vec3(0, 1, 0), 3.14 * 0.9);
////allShapes[4].rotate(vec3(0, 1, 0), 3.14 * 0.9);
////allShapes[5].rotate(vec3(0, 1, 0), 3.14 * 0.9);
////allShapes[6].rotate(vec3(0, 1, 0), 3.14 * 0.9);
////allShapes[7].rotate(vec3(0, 1, 0), 3.14 * 0.9);

//subdivide('Y', allShapes[2], ratios2);
//subdivide('X', allShapes[3], ratios);
//subdivide('X', allShapes[4], ratios);
//subdivide('X', allShapes[5], ratios);

//allShapes[14].setTextureToWindow();
//extrudeFace(allShapes[14], 0.2f);
//extrudeFace(allShapes[12], 0.1f);

////allShapes.push_back(Shape("PyramidRoof", vec3(2, 2, 2)));
////allShapes[2].rotate(vec3(1,0,1), 3.14 / 3);

////// Testing of translation/rotation/scaling
////allShapes[0].translate(vec3(10, 3, -5));
////allShapes[0].rotate(vec3(0, 1, 0), 3.14f / 2.0f);
////allShapes[0].rotate(vec3(0, 1, 0), 3.14f);
////allShapes[0].scale(vec3(2, 1, 1));

////subdivide('Y', allShapes[0], ratios);
////subdivide('Y', allShapes[2], ratios);
////allShapes[0].setInactive();