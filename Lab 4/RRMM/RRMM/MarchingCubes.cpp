#include "MarchingCubes.h"

MarchingCubes::MarchingCubes(GLuint shaderProgramID)
{
	
	#pragma region Read input-data from file.
	ifstream inputFile;
	inputFile.open("prova.txt");

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


	/*struct MyTraits {
		typedef Vec3f  Point;
		typedef Vec3f  Normal;
	
		VertexTraits{};
		HalfedgeTraits{};
		EdgeTraits{};
		FaceTraits{};

		VertexAttributes(0);
		HalfedgeAttributes(Attributes::PrevHalfedge);
		EdgeAttributes(0);
		FaceAttributes(0);

	};*/



	vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1, 1));
	vhandle[1] = mesh.add_vertex(MyMesh::Point(1, -1, 1));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(1, 1, 1));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(-1, 1, 1));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
	vhandle[5] = mesh.add_vertex(MyMesh::Point(1, -1, -1));
	vhandle[6] = mesh.add_vertex(MyMesh::Point(1, 1, -1));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(-1, 1, -1));
	

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	mesh.add_face(face_vhandles);

	createBuffers(shaderProgramID);

}

double MarchingCubes::getDataAtPoint(int i, int j, int k)
{
	return inputData[i * pow(dimensionSize, 2) + j * dimensionSize + k];
}

void MarchingCubes::print()
{
	cout << "Start print:" << endl;
	MyMesh::VertexIter it, vBegin, vEnd;
	cout << "vBegin: " << *vBegin << ", vEnd: " << *vEnd << endl;
	for (it = vBegin; it != vEnd; it++)
	{
		cout << *it;
	}

	

	cout << face_vhandles[3].idx() << endl;
	cout << face_vhandles[3] << endl;

	MyMesh::FaceIter face, fBegin, fEnd;
	cout << "fBegin: " << *fBegin << ", fEnd: " << *fEnd << endl;
	for (face = fBegin; face != fEnd; face++)
	{
		cout << *face;
	}

	cout << endl << "finished printing!" << endl;
}

void MarchingCubes::draw(GLuint shaderProgramID)
{

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vboArray.size(), &vboArray[0], GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::VertexHandle) * 8, &vhandle[0], GL_STATIC_DRAW);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(MyMesh::VertexHandle)*face_vhandles.size(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void MarchingCubes::createBuffers(GLuint shaderProgramID)
{
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::VertexHandle) * 8, &vhandle[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MyMesh::VertexHandle)*face_vhandles.size(), &face_vhandles[0], GL_STATIC_DRAW);

	//Vertex position attribute
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex normal attribute
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (GLvoid*)(sizeof(glm::vec3)));
	//glEnableVertexAttribArray(1);
	//Vertex color attribute
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	//glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}