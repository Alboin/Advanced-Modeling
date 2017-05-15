#include <iostream>
#include <fstream>
#include <sstream>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace OpenMesh;

//typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;

struct MyTraits : public OpenMesh::DefaultTraits
{
	VertexAttributes(OpenMesh::Attributes::Normal);
	FaceAttributes(OpenMesh::Attributes::Normal);

	//Store previous half-edge
	//HalfedgeAttributes(OpenMesh::DefaultAttributer::PrevHalfedge);

	/*EdgeTraits
	{
		typename Base::Refs::FaceHandle my_face_handle;
	};*/
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MarchingCubes
{
public:
	MarchingCubes(GLuint shaderProgramID);

	void draw(GLuint shaderProgramID);
	void print();

private:

	GLuint VBO, VAO, EBO;
	void createBuffers(GLuint shaderProgramID);

	MyMesh mesh;
	// Mesh vertices.
	MyMesh::VertexHandle vhandle[8];
	// Mesh faces.
	std::vector<MyMesh::VertexHandle>  face_vhandles;

	int dimensionSize;
	vector<float> inputData;
	pair<float, float> dataMaxMin;
	double getDataAtPoint(int i, int j, int k);

	MCcases triangleConfig;

};