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
	currentThreshold = /*(dataMaxMin.first - dataMaxMin.second) / 2.0f +*/ dataMaxMin.second;

	stepSize = (dataMaxMin.first - dataMaxMin.second) * wishedStepSize;

	changeThreshold(currentThreshold);

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


				// Get the triangle configuration for this data.
				vector<vector<int> > triangles;
				triangles = getTriangleConfig(triangleConfiguration);

				// Add vertices at the necessary edges, at the correct positions.
				for (int a = 0; a < triangles.size(); a++)
					createTriangles(triangles[a], i, j, k);

			}
		}
	}
}

void MarchingCubes::createTriangles(vector<int> edges, int i, int j, int k)
{
	int triangleIndexes[3] = { -1, -1, -1 };


	// Create one triangle
	for (int a = 0; a < 3; a++)
	{
		int surroundingEdges[3];
		int surroundingBoxesCoordinates[3][3] = {{i,j,k}, {i,j,k}, {i,j,k}};


		#pragma region Get surrounding cubes.
		if (i == 0 || j == 0 || k == 0 || i == dimensionSize - 1 || j == dimensionSize - 1 || k == dimensionSize - 1)
		{
			// Do nothing if its on the border of the volume.
		}
		else if (edges[a] == 0)
		{
			surroundingEdges[0] = 2;
			surroundingEdges[1] = 4;
			surroundingEdges[2] = 6;

			surroundingBoxesCoordinates[0][0]--;
			surroundingBoxesCoordinates[1][1]--;
			surroundingBoxesCoordinates[2][0]--;
			surroundingBoxesCoordinates[2][1]--;

		}
		else if (edges[a] == 1)
		{
			surroundingEdges[0] = 3;
			surroundingEdges[1] = 5;
			surroundingEdges[2] = 7;

			surroundingBoxesCoordinates[0][2]++;
			surroundingBoxesCoordinates[1][1]--;
			surroundingBoxesCoordinates[2][1]--;
			surroundingBoxesCoordinates[2][2]++;
		}
		else if (edges[a] == 2)
		{
			surroundingEdges[0] = 0;
			surroundingEdges[1] = 4;
			surroundingEdges[2] = 6;

			surroundingBoxesCoordinates[0][0]++;
			surroundingBoxesCoordinates[1][1]--;
			surroundingBoxesCoordinates[1][0]++;
			surroundingBoxesCoordinates[2][1]--;
		}
		else if (edges[a] == 3)
		{
			surroundingEdges[0] = 1;
			surroundingEdges[1] = 5;
			surroundingEdges[2] = 7;

			surroundingBoxesCoordinates[0][2]--;
			surroundingBoxesCoordinates[1][1]--;
			surroundingBoxesCoordinates[1][2]--;
			surroundingBoxesCoordinates[2][1]--;
		}
		else if (edges[a] == 4)
		{
			surroundingEdges[0] = 0;
			surroundingEdges[1] = 2;
			surroundingEdges[2] = 6;

			surroundingBoxesCoordinates[0][1]++;
			surroundingBoxesCoordinates[1][0]--;
			surroundingBoxesCoordinates[1][1]++;
			surroundingBoxesCoordinates[2][0]--;

		}
		else if (edges[a] == 5)
		{
			surroundingEdges[0] = 1;
			surroundingEdges[1] = 3;
			surroundingEdges[2] = 7;

			surroundingBoxesCoordinates[0][1]++;
			surroundingBoxesCoordinates[1][1]++;
			surroundingBoxesCoordinates[1][2]++;
			surroundingBoxesCoordinates[2][2]++;
		}
		else if (edges[a] == 6)
		{
			surroundingEdges[0] = 0;
			surroundingEdges[1] = 2;
			surroundingEdges[2] = 4;

			surroundingBoxesCoordinates[0][0]++;
			surroundingBoxesCoordinates[0][1]++;
			surroundingBoxesCoordinates[1][1]++;
			surroundingBoxesCoordinates[2][0]++;
		}
		else if (edges[a] == 7)
		{
			surroundingEdges[0] = 1;
			surroundingEdges[1] = 3;
			surroundingEdges[2] = 5;

			surroundingBoxesCoordinates[0][0]--;
			surroundingBoxesCoordinates[0][1]++;
			surroundingBoxesCoordinates[1][1]++;
			surroundingBoxesCoordinates[2][2]--;
		}
		else if (edges[a] == 8)
		{
			surroundingEdges[0] = 9;
			surroundingEdges[1] = 10;
			surroundingEdges[2] = 11;

			surroundingBoxesCoordinates[0][0]--;
			surroundingBoxesCoordinates[1][2]++;
			surroundingBoxesCoordinates[2][0]--;
			surroundingBoxesCoordinates[2][2]++;

		}
		else if (edges[a] == 9)
		{
			surroundingEdges[0] = 8;
			surroundingEdges[1] = 10;
			surroundingEdges[2] = 11;

			surroundingBoxesCoordinates[0][0]++;
			surroundingBoxesCoordinates[1][0]++;
			surroundingBoxesCoordinates[1][2]++;
			surroundingBoxesCoordinates[2][2]++;

		}
		else if (edges[a] == 10)
		{
			surroundingEdges[0] = 8;
			surroundingEdges[1] = 9;
			surroundingEdges[2] = 11;

			surroundingBoxesCoordinates[0][2]--;
			surroundingBoxesCoordinates[1][0]--;
			surroundingBoxesCoordinates[1][2]--;
			surroundingBoxesCoordinates[2][0]--;
		}
		else if (edges[a] == 11)
		{
			surroundingEdges[0] = 8;
			surroundingEdges[1] = 9;
			surroundingEdges[2] = 10;
			
			surroundingBoxesCoordinates[0][2]--;
			surroundingBoxesCoordinates[0][0]++;
			surroundingBoxesCoordinates[1][2]--;
			surroundingBoxesCoordinates[2][0]++;
		}
		#pragma endregion

		bool verticeExist = false;

		if (i == 0 || j == 0 || k == 0 || i == dimensionSize - 1 || j == dimensionSize - 1 || k == dimensionSize - 1)
		{
			// Do nothing if its on the border of the volume.
		}
		// Check if the vertice already have been generated in some of the three surrounding boxes.
		else
		{
			for (int b = 0; b < 3; b++)
			{
				int closeEdge = surroundingEdges[b];
				int close_i = surroundingBoxesCoordinates[b][0];
				int close_j = surroundingBoxesCoordinates[b][1];
				int close_k = surroundingBoxesCoordinates[b][2];

				auto it = verticePointers.find(hash(close_i, close_j, close_k, closeEdge));
				if (it != verticePointers.end())
				{
					// The vertice already exist.
					triangleIndexes[a] = it->second;
					verticeExist = true;
					break;
				}

			}
		}


		// The vertice does not exist yet and needs to be created.
		if (!verticeExist)
		{
			double point1, point2;
			float xPos = i;
			float yPos = j;
			float zPos = k;

			#pragma region Get the correct vertice position.
			if (edges[a] == 0) // Edge 0
			{
				point1 = getDataAtPoint(i, j, k);
				point2 = getDataAtPoint(i, j, k + 1);
				zPos += (currentThreshold - point1) / (point2 - point1);
			}
			else if (edges[a] == 1) // Edge 1
			{
				point1 = getDataAtPoint(i, j, k + 1);
				point2 = getDataAtPoint(i + 1, j, k + 1);
				xPos += (currentThreshold - point1) / (point2 - point1);
				zPos += 1;
			}
			else if (edges[a] == 2) // Edge 2
			{
				point1 = getDataAtPoint(i + 1, j, k);
				point2 = getDataAtPoint(i + 1, j, k + 1);
				zPos += (currentThreshold - point1) / (point2 - point1);
				xPos += 1;
			}
			else if (edges[a] == 3) // Edge 3
			{
				point1 = getDataAtPoint(i, j, k);
				point2 = getDataAtPoint(i + 1, j, k);
				xPos += (currentThreshold - point1) / (point2 - point1);
			}
			else if (edges[a] == 4) // Edge 4
			{
				point1 = getDataAtPoint(i, j + 1, k);
				point2 = getDataAtPoint(i, j + 1, k + 1);
				zPos += (currentThreshold - point1) / (point2 - point1);
				yPos += 1;
			}
			else if (edges[a] == 5) // Edge 5
			{
				point1 = getDataAtPoint(i, j + 1, k + 1);
				point2 = getDataAtPoint(i + 1, j + 1, k + 1);
				xPos += (currentThreshold - point1) / (point2 - point1);
				yPos += 1;
				zPos += 1;
			}
			else if (edges[a] == 6) // Edge 6
			{
				point1 = getDataAtPoint(i + 1, j + 1, k);
				point2 = getDataAtPoint(i + 1, j + 1, k + 1);
				zPos += (currentThreshold - point1) / (point2 - point1);
				yPos += 1;
				xPos += 1;
			}
			else if (edges[a] == 7) // Edge 7
			{
				point1 = getDataAtPoint(i, j + 1, k);
				point2 = getDataAtPoint(i + 1, j + 1, k);
				xPos += (currentThreshold - point1) / (point2 - point1);
				yPos += 1;
			}
			else if (edges[a] == 8) // Edge 8
			{
				point1 = getDataAtPoint(i, j, k + 1);
				point2 = getDataAtPoint(i, j + 1, k + 1);
				yPos += (currentThreshold - point1) / (point2 - point1);
				zPos += 1;
			}
			else if (edges[a] == 9) // Edge 9
			{
				point1 = getDataAtPoint(i + 1, j, k + 1);
				point2 = getDataAtPoint(i + 1, j + 1, k + 1);
				yPos += (currentThreshold - point1) / (point2 - point1);
				xPos += 1;
				zPos += 1;
			}
			else if (edges[a] == 10) // Edge 10
			{
				point1 = getDataAtPoint(i, j, k);
				point2 = getDataAtPoint(i, j + 1, k);
				yPos += (currentThreshold - point1) / (point2 - point1);
			}
			else if (edges[a] == 11) // Edge 11
			{
				point1 = getDataAtPoint(i + 1, j, k);
				point2 = getDataAtPoint(i + 1, j + 1, k);
				yPos += (currentThreshold - point1) / (point2 - point1);
				xPos += 1;
			}
			#pragma endregion

			if (xPos < 5 || yPos < 5 || zPos < 5)
				cout << "AJABAJA!!" << endl;

			// Add the vertice.
			vertices.push_back(glm::vec3(xPos, yPos, zPos));
			// Add its index to the hash-table.
			verticePointers[hash(i, j, k, edges[a])] = vertices.size() - 1;
			// Add its index to the current triangle.
			triangleIndexes[a] = vertices.size() - 1;

		}
	}

	//Debugging
	for (int h = 0; h < 3; h++)
		if (triangleIndexes[h] < 0)
			cout << "ERROR: Wrong index!!!" << endl;

	// Add the finished triangle to the indices.
	indices.push_back(glm::ivec3(triangleIndexes[0], triangleIndexes[1], triangleIndexes[2]));

}

long long int MarchingCubes::hash(int i, int j, int k, int e)
{
	// Hashing the vertice position as long as the dimension size is less than 100.
	long long int number = (i * 1000000) + (j * 10000) + (k * 100) + e;
	return number;
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

	// Remove old vertices.
	vertices.clear();
	indices.clear();
	vboArray.clear();
	verticePointers.clear();
	verticeNeighbours.clear();
	edges.clear();

	// Create new vertices with modified threshold.
	currentThreshold = newThreshold;
	createTriangles();

	//mergeVerticeDuplicates();
	createVBOarray();
	computeNumberOfShells();
	computeNormals(); 
	computeEdges();
	createBuffers(shaderID);

	cout << endl <<"Threshold: " << currentThreshold << endl;

	int genus = 0;
	genus += vertices.size();
	genus += indices.size();
	genus -= edges.size();
	genus /= 2;
	genus = -genus;
	genus += nShells;

	for (int i = 0; i < vertices.size(); i++)
		if (glm::distance(vertices[i], glm::vec3(0, 0, 0)) < 5)
			cout << "AJAJ!" << endl;


	//int genus = ((vertices.size() + indices.size() - edges.size())) * 0.5f;
	cout << "V: " << vertices.size() << ", F: " << indices.size() << ", E: " << edges.size() << ", S: " << nShells << ", H: " << genus << endl;
}

void MarchingCubes::computeNumberOfShells()
{
	// Compute each vertex neighbouring vertices.
	verticeNeighbours.resize(vertices.size());
	for (int i = 0; i < indices.size(); i++)
	{
		verticeNeighbours[indices[i].x].insert(indices[i].y);
		verticeNeighbours[indices[i].x].insert(indices[i].z);
		verticeNeighbours[indices[i].y].insert(indices[i].x);
		verticeNeighbours[indices[i].y].insert(indices[i].z);
		verticeNeighbours[indices[i].z].insert(indices[i].y);
		verticeNeighbours[indices[i].z].insert(indices[i].x);
	}

	//Debugging
	/*float averageConnectivity = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		//if (verticeNeighbours[i].size() > 40)
			//cout << "ojdå: " << verticeNeighbours[i].size() << endl;
			averageConnectivity += verticeNeighbours[i].size();
	}
	averageConnectivity /= verticeNeighbours.size();
	cout << "Average connectivity: " << averageConnectivity << endl;*/



	nShells = 0;
	set<int> verticesLeft;
	// Create a list with all vertices to explore.
	for (int i = 0; i < vertices.size(); i++)
		verticesLeft.insert(i);


	// Loop until all vertices has been explored.
	while (!verticesLeft.empty())
	{
		nShells++;
		
		// Gives the vertex a red color if it's not in the first shell.
		glm::vec3 vertexColor = glm::vec3(1, 1, 1);
		//glm::vec3 vertexColor = glm::vec3(0.8f, 0.8f, 0.8f);
		if (nShells > 1)
		{
			float col1 = ((double)rand() / (RAND_MAX));
			float col2 = ((double)rand() / (RAND_MAX));
			float col3 = ((double)rand() / (RAND_MAX));
			vertexColor = glm::vec3(col1, col2, col3);
			//vertexColor = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		
		// Create a set for the current shell and add a vertex to it.
		set<int> currentShell;
		int startingVertex = *verticesLeft.begin();
		currentShell.insert(startingVertex);
		// Remove the vertex from verticesLeft.
		verticesLeft.erase(startingVertex);

		int verticesInCurrentShell = 0;

		while (!currentShell.empty())
		{
			verticesInCurrentShell++;

			// Get the first vertex
			int vertex = *currentShell.begin();
			vboArray[vertex * 3 + 2] = vertexColor;
			// Add the neighbours that still needs to be explored
			for (set<int>::iterator it = verticeNeighbours[vertex].begin(); it != verticeNeighbours[vertex].end(); it++)
			{
				int neigbourVertex = *it;
				// If the neighbour is still in verticesLeft, add it.
				set<int>::iterator element = verticesLeft.find(neigbourVertex);
				if (element != verticesLeft.end())
				{
					currentShell.insert(neigbourVertex);
					verticesLeft.erase(element);

				}
			}
			// Remove current vertex from currentShell.
			currentShell.erase(currentShell.begin());
		}
		// If the current shell has less than 1% of the total number of vertices in it, discard it as a shell.
		if (verticesInCurrentShell < (float)vertices.size() / 100.0f)
			nShells--;
	}


}

void MarchingCubes::computeEdges()
{
	// Add a pair with the vertex indices for two connected vertices, sorted (to not create duplicates).
	for (int i = 0; i < indices.size(); i++)
	{
		edges.insert(make_pair(min(indices[i].x, indices[i].y), max(indices[i].x, indices[i].y)));
		edges.insert(make_pair(min(indices[i].x, indices[i].z), max(indices[i].x, indices[i].z)));
		edges.insert(make_pair(min(indices[i].y, indices[i].z), max(indices[i].y, indices[i].z)));
	}
}

void MarchingCubes::draw(GLuint shaderProgramID, GLuint diffuseTexID, GLuint specularTexID)
{

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vboArray.size(), &vboArray[0], GL_STATIC_DRAW);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	GLuint tex0 = glGetUniformLocation(shaderProgramID, "diffuseCube");
	glUniform1i(tex0, 0);
	GLuint tex1 = glGetUniformLocation(shaderProgramID, "specularCube");
	glUniform1i(tex1, 1);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseTexID);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, specularTexID);

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