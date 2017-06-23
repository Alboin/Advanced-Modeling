#include "Building.h"

Building::Building(const int config)
	:currentRandom(1), currentShape(0), configuration(config)
{
	// Add all textures.
	vector<string> textureLocations;
	textureLocations.push_back("textures/wall1.jpg");
	textureLocations.push_back("textures/wall2.jpg");
	textureLocations.push_back("textures/roofCone.jpg");
	textureLocations.push_back("textures/window.jpg");
	textureLocations.push_back("textures/door.jpg");
	loadTextures(textureLocations);

	// Initialize the stack.
	transformations.push(queue<mat4>());
	transformations.top().push(mat4());
	currentSize.push(vec3(0));

	// Initialize with a starting shape
	create("Lot");

	// Loop to iteratively apply the grammars.
	bool onlyTerminals = false;
	int iterations = 0, iterationLimit = 100;
	while (!onlyTerminals && iterations < iterationLimit)
	{
		onlyTerminals = true;
		iterations++;
		cout << "Iterations: " << iterations << endl;

		int nShapes = allShapes.size();
		int test = 0;
		for (int i = 0; i < nShapes; i++)
		{
			test = i;
			updateRandom();
			currentShape = i;
			currShape = &allShapes[currentShape];

			// Apply the grammars.
			if(!currShape->isTerminal())
			grammars(allShapes[currentShape]);
		}
		//cout << "grammar iterations this iteration: " << test << endl;

		// Remove inactive shapes and check if there is any more non-terminals.
		for (int i = allShapes.size() - 1; i >= 0; i--)
		{
			if (!allShapes[i].isActive())
				allShapes.erase(allShapes.begin() + i);
			else if (!allShapes[i].isTerminal())
				onlyTerminals = false;
		}
	}

	cout << "Total number of shapes: " << allShapes.size() << endl;
}

void Building::grammars(Shape &shape)
{
	// Run the chosen grammar configuration.
	if (configuration == 1)
		return config1(shape);
	else if (configuration == 2)
		return config2(shape);
	else if (configuration == 3)
		return config3(shape);
}

void Building::translate(const vec3 translation)
{
	transformations.top().push(glm::translate(mat4(), translation));
}

void Building::rotate(const vec3 rotationAxis, const float angle)
{
	transformations.top().push(glm::rotate(mat4(), angle, rotationAxis));
}

void Building::scale(const vec3 scaleSize)
{
	currentSize.top() += scaleSize;
}

void Building::create(const string &type)
{
	create(type, currentSize.top());
	currShape = &allShapes[currentShape];
}

void Building::create(const string &type, const vec3 dimensions)
{
	allShapes.push_back(Shape(type, dimensions));

	// Apply current transformations to the new shape.
	queue<mat4> temp;
	while (!transformations.top().empty())
	{
		allShapes[allShapes.size() - 1].transform(transformations.top().front());
		temp.push(transformations.top().front());
		transformations.top().pop();
	}
	while (!temp.empty())
	{
		transformations.top().push(temp.front());
		temp.pop();
	}
	currShape = &allShapes[currentShape];
}

void Building::separateFaces(Shape &shape, const string &newName)
{
	shape.setInactive();

	vector<vec3> vboCopy = shape.getVBOarray();
	int texture = shape.getTextureIndex();

	vector<ivec3> newIndices;
	newIndices.push_back(ivec3(0, 2, 3));
	newIndices.push_back(ivec3(0, 3, 1));

	for (int i = 0; i < 6; i++)
	{
		vector<vec3> newVbo(vboCopy.begin() + i * 8, vboCopy.begin() + (i + 1) * 8);

		allShapes.push_back(Shape(newName, newVbo, newIndices, texture));

		// If the extracted face is a floor, make it terminal since our subdivision-functions dont support them.
		if (i == 4 || i == 5)
			allShapes[allShapes.size() - 1].setToTerminal();
	}

	currShape = &allShapes[currentShape];
}

void Building::extrudeFace(Shape &shape, const float extrude)
{
	shape.setInactive();

	vector<vec3> newVbo;
	vector<ivec3> newIndices;
	vector<vec3> oldVertices = shape.getVBOarray();

	vec3 normal = normalize(cross(oldVertices[2] - oldVertices[0], oldVertices[4] - oldVertices[0]));
	float extrusion = extrude;

	if (length(normal - vec3(0, 1, 0)) < 0.1)
		extrusion = 0;

	// Extrude the face in both directions of the face-normal with the given size
	for (int a = -1; a < 2; a += 2)
	{
		vec3 movement = normal * extrusion * (float)a;
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

	//vec3 temp = shape.getTranslatation() + shape.getDimensions() + normal * extrude * 2.0f;
	allShapes.push_back(Shape("ExtrudedFace", newVbo, newIndices, shape.getTextureIndex()));
	allShapes[allShapes.size() - 1].setToTerminal();
	currShape = &allShapes[currentShape];
}

void Building::subdivide(Shape &shape, const char &axis, const vector<double> ratios, const vector<string> shapeNames)
{
	// Set the old shape as inactice, preventing it from getting drawn.
	shape.setInactive();

	vector<int> indices;
	// Depending on wether the shape is a single face, a cube or a cylinder, different functions needs to be called.
	if (shape.getVBOarray().size() == 8)
		indices = subdivideFace(axis, shape, createRatios(axis, ratios, shape));
	else
		indices = subdivideShape(axis, shape, createRatios(axis, ratios, shape));

	// Give the new shapes their newn names.
	for (int i = 0; i < indices.size(); i++)
		allShapes[indices[i]].setName(shapeNames[i]);
	currShape = &allShapes[currentShape];
}

vector<int> Building::subdivideShape(const char &axis, const Shape shape, const vector<double> ratios)
{
	vector<int> indicesOfNewShapes;

	string name = shape.getName();

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
		vec3 translation = vec3(shape.getDimensions().x * x1, shape.getDimensions().y * y1, shape.getDimensions().z * z1);
		allShapes.push_back(Shape(name, newDimensions));

		// Transform the new shape to its correct position and rotation.
		allShapes[allShapes.size() - 1].transform(shape.getTransformations());
		allShapes[allShapes.size() - 1].transform(glm::translate(translation));
		
		indicesOfNewShapes.push_back(allShapes.size() - 1);
	}
	currShape = &allShapes[currentShape];
	return indicesOfNewShapes;
}

vector<int> Building::subdivideFace(const char &axis, const Shape shape, const vector<double> ratios)
{
	vector<int> indicesOfNewShapes;
	string name = shape.getName();

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
				if (i < 4)
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
					newVertex = vec3(xMin + start * (xMax - xMin), oldVertices[i].y, zMin + start * (zMax - zMin));
				else if (i == 1 || i == 5)
					newVertex = vec3(texMin + start * (texMax - texMin), oldVertices[i].y, 0);
				else if (i == 3 || i == 7)
					newVertex = vec3(texMin + end * (texMax - texMin), oldVertices[i].y, 0);
				else
					newVertex = vec3(xMin + end * (xMax - xMin), oldVertices[i].y, zMin + end * (zMax - zMin));

				newVertices.push_back(newVertex);
			}
		}

		allShapes.push_back(Shape(name, newVertices, shape.getIndexArray(), shape.getTextureIndex()));
		indicesOfNewShapes.push_back(allShapes.size() - 1);
	}

	currShape = &allShapes[currentShape];
	return indicesOfNewShapes;

}

void Building::push()
{
	transformations.push(transformations.top());
	currentSize.push(currentSize.top());
}

void Building::pop()
{
	transformations.pop();
	currentSize.pop();
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

vector<double> Building::createRatios(const char &axis, vector<double> input, const Shape &shape)
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
		lengthOfAxis = 2.0 * 3.14 * sqrt((pow(shape.getDimensions()[0], 2) + pow(shape.getDimensions()[2], 2)) / 2.0f);

	vector<double> relatives;
	vector<double> absolutes;
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

	float ratioPerUnit = (1.0 - sumOfAbsolutes) / (double)sumOfRelatives;

	// Calculate the final ratios.
	vector<double> finalRatios;
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
	vector<double> formattedRatios;
	for (int i = 0; i < finalRatios.size() - 1; i++)
	{
		float sumOfRatios = 0.0;
		for (int j = 0; j <= i; j++)
			sumOfRatios += finalRatios[j];
		formattedRatios.push_back(sumOfRatios);
	}

	return formattedRatios;
}

void Building::config1(Shape &shape)
{
	if (shape.is("Lot"))
	{
		scale(vec3(random(5, 10), random(5, 7), random(5, 10)));
		create("Cube");
	}
	else if (shape.is("Cube") && probability(0.3))
	{
		shape.setToTerminal();
		translate(vec3(0, shape.getDimensions().y, 0));
		create("PyramidRoof", vec3(shape.getDimensions().x, random(2, 5), shape.getDimensions().z));
		subdivide('Y', { -3, -0.3, -2, -0.3,-2 }, { "Ground", "Ledge", "Floor", "Ledge", "Floor" });
	}
	else if (shape.is("Cube") && probability(0.7))
	{
		push();
		translate(vec3(0, shape.getDimensions().y, 0));
		translate(vec3(shape.getDimensions().x / 2.0 - 3 / 2.0, 0, shape.getDimensions().z / 2.0 - 3 / 2.0));
		push();
		int temp = random(4, 7);
		translate(vec3(0, temp, 0));
		if (probability(0.5))
			create("ConeRoof", vec3(3, 1, 3));
		else
			create("PyramidRoof", vec3(3, 1, 3));
		pop();
		create("Cylinder", vec3(3, temp, 3));
		pop();
	}
	else if (shape.is("Ground"))
	{
		separateFaces("GroundFace");
	}
	else if (shape.is("Floor"))
	{
		separateFaces("FloorFace");
	}
	else if (shape.is("FloorFace") && probability(0.3))
	{
		subdivide('X', { -2, 1,-1, 1, -1, 1, -2 }, { "Wall", "Window", "Wall", "Window", "Wall", "Window", "Wall" });
	}
	else if (shape.is("FloorFace") && probability(0.3))
	{
		subdivide('X', { -2, 1, -1, 1, -2 }, { "Wall", "Window", "Wall", "Window", "Wall" });
	}
	else if (shape.is("GroundFace"))
	{
		subdivide('X', { -1, 1.5, -4 }, { "Wall", "Door", "Wall" });
	}
	else if (shape.is("Wall"))
	{
		shape.setToTerminal();
	}
	else if (shape.is("Door"))
	{
		shape.setTextureToDoor();
		extrudeFace(0.2);
	}
	else if (shape.is("Window"))
	{
		shape.setTextureToWindow();
		extrudeFace(0.1);
	}
	else if (shape.is("Ledge"))
	{
		separateFaces("LedgeFace");
	}
	else if (shape.is("LedgeFace"))
	{
		extrudeFace(0.1);
		shape.setToTerminal();
	}
	else if (shape.is("Cylinder"))
	{
		shape.setToTerminal();
	}
}

void Building::config2(Shape &shape)
{
	if (currShape->is("Lot"))
	{
		push();
		scale(vec3(3, 3, 3));
		create("Cylinder");
		translate(vec3(0, 3, 0));
		scale(vec3(0, -2, 0));
		create("ConeRoof");
		pop();
	}
	else if (currShape->is("Cylinder") && probability(0.5))
	{
		push();
		float temp = random(3, 6);
		float temp2 = random(1, 2.5);
		scale(vec3(temp, temp2, 2));
		translate(vec3(-temp, 0, -1));
		rotate(vec3(0, 1, 0), random(0, 3.14 * 2));
		translate(vec3(1.5, 0, 1.5));
		create("Cube");
		scale(vec3(0, -temp2 / 2, 0));
		translate(vec3(0, temp2, 0));
		create("PyramidRoof");
		pop();
	}
	else if (currShape->is("Cylinder"))
	{
		currShape->setToTerminal();
	}
	else if (currShape->is("Cube"))
	{
		separateFaces("CubeFace");
		currShape->setToTerminal();
	}
	else if (currShape->is("CubeFace"))
	{
		subdivide('Y', { 0.8, -1, 0.1,-1 }, { "GroundLevel", "Floor", "Ledge", "Floor" });
	}
	else if (currShape->is("GroundLevel") && probability(0.3))
	{
		subdivide('X', { -1, 0.5, -3 }, { "Wall", "Door", "Wall" });
	}
	else if (currShape->is("GroundLevel"))
	{
		currShape->setName("Wall");
	}
	else if (currShape->is("Floor"))
	{
		subdivide('X', { -1, 0.4,-1, 0.4, -1 }, { "Wall", "Window", "Wall", "Window", "Wall" });
	}
	else if (currShape->is("Wall"))
	{
		currShape->setToTerminal();
	}
	else if (currShape->is("Window"))
	{
		currShape->setTextureToWindow();
		extrudeFace(0.03);
	}
	else if (currShape->is("Door"))
	{
		currShape->setTextureToDoor();
		extrudeFace(0.1);
	}
	else if (currShape->is("Ledge"))
	{
		extrudeFace(0.05);
	}
}

void Building::config3(Shape &shape)
{
	if (currShape->is("Lot"))
	{
		push();
		scale(vec3(6,3,3));
		create("Cube");
		allShapes[allShapes.size() - 1].setName("MainBuilding");
		push();
		translate(vec3(0, 3, 0));
		scale(vec3(0, -1.5, 0));
		create("PyramidRoof");
		pop();
		if (probability(0.5))
		{
			push();
			translate(vec3(0, 0, 3));
			scale(vec3(-4, -1, 0));
			create("Cube");
			translate(vec3(0, 2, 0));
			scale(vec3(0, -1.5, 0));
			create("PyramidRoof");
			pop();
		}
		if (probability(0.5))
		{
			push();
			translate(vec3(4, 0, 3));
			scale(vec3(-4, -1, 0));
			create("Cube");
			translate(vec3(0, 2, 0));
			scale(vec3(0, -1.5, 0));
			create("PyramidRoof");
			pop();
		}
	}
	else if (currShape->is("Cube"))
	{
		separateFaces("CubeFace");
		//currShape->setToTerminal();
	}
	else if (currShape->is("MainBuilding"))
	{
		separateFaces("MainBuildingFace");
	}
	else if (currShape->is("MainBuildingFace"))
	{
		subdivide('Y', { 1, -1, 0.1,-1, 0.1, -1 }, { "GroundLevel", "Floor", "Ledge", "Floor", "Ledge", "Floor" });
	}
	else if (currShape->is("CubeFace"))
	{
		subdivide('Y', { 1, -1, 0.1,-1 }, { "GroundLevel", "Floor", "Ledge", "Floor" });
	}
	else if (currShape->is("GroundLevel") && probability(0.5))
	{
		subdivide('X', { -1, 0.5, -3 }, { "Wall", "Door", "Wall" });
	}
	else if (currShape->is("GroundLevel"))
	{
		currShape->setName("Wall");
	}
	else if (currShape->is("Door"))
	{
		currShape->setTextureToDoor();
		extrudeFace(0.1);
	}
	else if (currShape->is("Wall"))
	{
		currShape->setToTerminal();
	}
	else if (currShape->is("Ledge"))
	{
		extrudeFace(0.1f);
	}
	else if (currShape->is("Floor") && probability(0.5))
	{
		subdivide('X', { -1, 0.3, -1, 0.3, -1 }, { "Wall", "Window", "Wall", "Window", "Wall" });
	}
	else if (currShape->is("Floor"))
	{
		subdivide('X', { -1, 0.3, -1, 0.3, -1, 0.3, -1 }, { "Wall", "Window", "Wall", "Window", "Wall", "Window", "Wall" });
	}
	else if (currShape->is("Window"))
	{
		currShape->setTextureToWindow();
	}
}

