#include "MarchingCubes.h"

MarchingCubes::MarchingCubes(string inputFileName, GLuint shaderProgramID)
{
	
	#pragma region Read input-data from file.
	ifstream inputFile;
	inputFile.open(inputFileName);

	if (inputFile.is_open())
	{
		// Get the data size from the first row of the input.
		string line;
		getline(inputFile, line);
		stringstream ss;
		ss << line;
		ss >> dimensionSize;
		inputData.reserve(pow(dimensionSize,3));

		// Read the data into the vector.
		for (int i = 0; getline(inputFile, line); i++)
		{
			ss.clear();
			ss << line;

			float datapoint;
			ss >> datapoint;
			inputData.push_back(datapoint);

			// Get the minimum and maximum value of the data.
			if (i == 0)
				dataMaxMin = make_pair(datapoint, datapoint);
			else if (dataMaxMin.first < datapoint)
				dataMaxMin.first = datapoint;
			else if (dataMaxMin.second > datapoint)
				dataMaxMin.second = datapoint;
		}
		inputFile.close();
	}
	#pragma endregion

	// Set the initial threshold value right between the largest and smallest value.
	currentThreshold = (dataMaxMin.first - dataMaxMin.second) / 2.0f + dataMaxMin.second;

	stepSize = (dataMaxMin.first - dataMaxMin.second) * wishedStepSize;

	createTriangles();

	//cout << "Current threshold: " << currentThreshold << endl;

	//cout << endl << "Vertices: " << endl;
	//for (int i = 0; i < vertices.size(); i++)
	//	cout << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << endl;
	//cout << endl << endl;

	createVBOarray();
	computeNormals();
	createBuffers(shaderProgramID);
}

void MarchingCubes::createTriangles()
{
	// Create triangles from the data and the current threshold.
	for (int i = 0; i < dimensionSize - 1; i++)
	{
		for (int j = 0; j < dimensionSize - 1; j++)
		{
			for (int k = 0; k < dimensionSize - 1; k++)
			{
				int triangleConfiguration = 0;

				// Compare the datapoints in one cube to the threshold.
				if (getDataAtPoint(i, j, k) > currentThreshold)
					triangleConfiguration += pow(2, 0);
				if (getDataAtPoint(i + 1, j, k) > currentThreshold)
					triangleConfiguration += pow(2, 1);
				if (getDataAtPoint(i, j + 1, k) > currentThreshold)
					triangleConfiguration += pow(2, 2);
				if (getDataAtPoint(i + 1, j + 1, k) > currentThreshold)
					triangleConfiguration += pow(2, 3);

				if (getDataAtPoint(i, j, k + 1) > currentThreshold)
					triangleConfiguration += pow(2, 4);
				if (getDataAtPoint(i + 1, j, k + 1) > currentThreshold)
					triangleConfiguration += pow(2, 5);
				if (getDataAtPoint(i, j + 1, k + 1) > currentThreshold)
					triangleConfiguration += pow(2, 6);
				if (getDataAtPoint(i + 1, j + 1, k + 1) > currentThreshold)
					triangleConfiguration += pow(2, 7);

				//cout << "Configuration: " << triangleConfiguration << endl;

				// Get the triangle configuration for this data.
				vector<vector<int> > triangles;
				triangles = getTriangleConfig(triangleConfiguration);

				//cout << endl << "Triangles: ";
				//for (int a = 0; a < triangles.size(); a++)
				//	cout << triangles[a][0] << ", " << triangles[a][1] << ", " << triangles[a][2] << endl;
				//cout << endl << endl;

				// Add vertices at the necessary edges, at the correct positions.
				for (int a = 0; a < triangles.size(); a++)
				{
					vector<glm::vec3> temp = generateVertices(triangles[a], i, j, k);
					vertices.push_back(temp[0]);
					vertices.push_back(temp[1]);
					vertices.push_back(temp[2]);
					int index = vertices.size();
					//cout << "vert:" << i << "." << j << "." << k << endl;
					//cout << temp[0].x << "," << temp[0].y << "," << temp[0].z << endl;
					//cout << temp[1].x << "," << temp[1].y << "," << temp[1].z << endl;
					//cout << temp[2].x << "," << temp[2].y << "," << temp[2].z << endl;
					//cout << endl;
					indices.push_back(glm::ivec3(index, index + 1, index + 2));
				}

				// Make sure that the final size of the volume is withing 1x1x1 (normalized). - DONE?



				//TODO: this is not correct! The result gives edges, not vertices.
				// Add all the given triangles to the indice-list.
				//for (int a = 0; a < triangles.size(); a++)
				//indices.push_back(glm::ivec3(triangles[i][0], triangles[i][1], triangles[i][2]));


			}
		}
	}
}

vector<glm::vec3> MarchingCubes::generateVertices(vector<int> edges, int i, int j, int k)
{
	vector<glm::vec3> resultingVertices;

	// Create the vertices for one triangle.
	for (int a = 0; a < 3; a++)
	{
		double point1, point2;
		float xPos = float(i) / float(dimensionSize - 1);
		float yPos = float(j) / float(dimensionSize - 1);
		float zPos = float(k) / float(dimensionSize - 1);

		//cout << endl << xPos << ", " << yPos << ", " << zPos;

		if (edges[a] == 0) // Edge 0
		{
			point1 = getDataAtPoint(i, j, k);
			point2 = getDataAtPoint(i, j, k + 1);
			zPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
		}
		else if (edges[a] == 1) // Edge 1
		{
			point1 = getDataAtPoint(i, j, k + 1);
			point2 = getDataAtPoint(i + 1, j, k + 1);
			xPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			zPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 2) // Edge 2
		{
			point1 = getDataAtPoint(i + 1, j, k + 1);
			point2 = getDataAtPoint(i + 1, j, k);
			zPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			xPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 3) // Edge 3
		{
			point1 = getDataAtPoint(i, j, k);
			point2 = getDataAtPoint(i + 1, j, k);
			xPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
		}
		else if (edges[a] == 4) // Edge 4
		{
			point1 = getDataAtPoint(i, j + 1, k);
			point2 = getDataAtPoint(i, j + 1, k + 1);
			zPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			yPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 5) // Edge 5
		{
			point1 = getDataAtPoint(i, j + 1, k + 1);
			point2 = getDataAtPoint(i + 1, j + 1, k + 1);
			xPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			yPos += 1.0f / float(dimensionSize - 1);
			zPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 6) // Edge 6
		{
			point1 = getDataAtPoint(i + 1, j + 1, k);
			point2 = getDataAtPoint(i + 1, j + 1, k + 1);
			zPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			yPos += 1.0f / float(dimensionSize - 1);
			xPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 7) // Edge 7
		{
			point1 = getDataAtPoint(i, j + 1, k);
			point2 = getDataAtPoint(i + 1, j + 1, k);
			xPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			yPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 8) // Edge 8
		{
			point1 = getDataAtPoint(i, j, k + 1);
			point2 = getDataAtPoint(i, j + 1, k + 1);
			yPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			zPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 9) // Edge 9
		{
			point1 = getDataAtPoint(i + 1, j, k + 1);
			point2 = getDataAtPoint(i + 1, j + 1, k + 1);
			yPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			xPos += 1.0f / float(dimensionSize - 1);
			zPos += 1.0f / float(dimensionSize - 1);
		}
		else if (edges[a] == 10) // Edge 10
		{
			point1 = getDataAtPoint(i, j, k);
			point2 = getDataAtPoint(i, j + 1, k);
			yPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
		}
		else if (edges[a] == 11) // Edge 11
		{
			point1 = getDataAtPoint(i + 1, j, k);
			point2 = getDataAtPoint(i + 1, j + 1, k);
			yPos += (currentThreshold - min(point1, point2)) / (max(point1, point2) - min(point1, point2)) * (1.0f / float(dimensionSize - 1));
			xPos += 1.0f / float(dimensionSize - 1);
		}
		else
		{
			cout << "ERROR: Wrong edge input!" << endl;
		}

		resultingVertices.push_back(glm::vec3(xPos, yPos, zPos));
	}
	
	return resultingVertices;
}


double MarchingCubes::getDataAtPoint(int i, int j, int k)
{
	return inputData[i * pow(dimensionSize, 2) + j * dimensionSize + k];
}

void MarchingCubes::computeNormals()
{
	// Calculate several normals for every vertex and add together.
	for (int i = 0; i < indices.size(); i++)
	{
		glm::vec3 vertexA = vboArray[indices[i].x * 3];
		glm::vec3 vertexB = vboArray[indices[i].y * 3];
		glm::vec3 vertexC = vboArray[indices[i].z * 3];

		glm::vec3 vecA = vertexC - vertexA;
		glm::vec3 vecB = vertexC - vertexB;

		glm::vec3 n = normalize(cross(vecB, vecA));

		vboArray[indices[i].x * 3 + 1] += n;
		vboArray[indices[i].y * 3 + 1] += n;
		vboArray[indices[i].z * 3 + 1] += n;
	}
	// Normalize all normals.
	for (int i = 1; i < vboArray.size(); i += 3)
	{
		vboArray[i] = normalize(vboArray[i]);
	}

}

void MarchingCubes::createVBOarray()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		// Add vertice position
		vboArray.push_back(vertices[i]);
		// Add temporary vertice normal
		vboArray.push_back(glm::vec3(0, 0, 0));
		// Add vertice color
		vboArray.push_back(glm::vec3(0.8f, 0.8f, 0.8f));
	}
}

void MarchingCubes::changeThreshold(float newThreshold)
{
	if (newThreshold > dataMaxMin.first)
	{
		cout << "ERROR: Threshhold to large." << endl;
		return;
	}
	else if (newThreshold < dataMaxMin.second)
	{
		cout << "ERROR: Threshold to small." << endl;
		return;
	}

	vertices.clear();
	indices.clear();
	vboArray.clear();

	currentThreshold = newThreshold;
	createTriangles();
	createVBOarray();
	computeNormals();
	createBuffers(shaderID);
}

void MarchingCubes::mergeVerticeDuplicates()
{
	for (int i = 0; i < dimensionSize - 2; i++)
	{
		for (int j = 0; j < dimensionSize - 2; j++)
		{
			for (int k = 0; k < dimensionSize - 2; k++)
			{
			}
		}
	}
}

void MarchingCubes::draw(GLuint shaderProgramID)
{

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vboArray.size(), &vboArray[0], GL_STATIC_DRAW);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(glm::ivec3) * indices.size(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void MarchingCubes::createBuffers(GLuint shaderProgramID)
{

	shaderID = shaderProgramID;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vboArray.size(), &vboArray[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//Vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	//Vertex color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}