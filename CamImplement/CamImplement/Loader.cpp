#include "Loader.h"

Loader::Loader()
{
	nObjectsCurrent = 0;
}

Loader::Loader(const Loader& obj)
{

}

Loader::~Loader()
{
	for (int i = 0; i < nObjectsCurrent; i++)
	{
		delete[] m_objects[i].vertices;
		delete[] m_objects[i].texCoords;
		delete[] m_objects[i].normals;
		delete[] m_objects[i].faces;
	}
	delete[] m_objects;
	delete[] m_fileCounts;
}

void Loader::Initialize(int nObjects)
{
	if (nObjects > 0)
	{
		nObjectsTotal = nObjects;
		m_fileCounts = new FileCountType[nObjectsTotal];
		m_objects = new ObjectType[nObjectsTotal];
		for (int i = 0; i < nObjectsTotal; i++)
		{
			m_fileCounts[i].nVertices = 0;
			m_fileCounts[i].nTextures = 0;
			m_fileCounts[i].nNormals = 0;
			m_fileCounts[i].nFaces = 0;
		}
	}
	else { return; }

	char* filename = "dummyMan.obj";
	
	if (ReadFileCounts(filename))
	{
		LoadDataStructures(filename);
		nObjectsCurrent++;
	}
}

ObjectType& Loader::getObject(Object obj) const
{
	return m_objects[obj];
}

int Loader::getVertexCount(Object index) const
{
	return m_fileCounts[index].nVertices;
}

int Loader::getIndexCount(Object index) const
{
	return m_fileCounts[index].nFaces * 3;
}

void Loader::FindModelFilename(Object object, char* filename)
{
	char* localFilename;
	switch (object)
	{
		case Player:
		{
			filename = "dummyMan.obj";
			break;
		}
		case Block:
		{
			filename = "dummyPlane.obj";
			break;
		}
		default:
			break;
	}
}

bool Loader::ReadFileCounts(char* filename)
{
	if (nObjectsCurrent == nObjectsTotal) { return false; }

	std::ifstream fin;
	char input;

	fin.open(filename);
	if (fin.fail()) { return false; }

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ')
			{
				m_fileCounts[nObjectsCurrent].nVertices++;
			}
			if (input == 't')
			{
				m_fileCounts[nObjectsCurrent].nTextures++;
			}
			if (input == 'n')
			{
				m_fileCounts[nObjectsCurrent].nNormals++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				m_fileCounts[nObjectsCurrent].nFaces++;
			}
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	return true;
}

bool Loader::LoadDataStructures(char* filename)
{
	if (nObjectsCurrent == nObjectsTotal) { return false; }

	std::ifstream fin;
	std::ofstream fout;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	int vIndex, tIndex, nIndex;
	char input;
	char temp;

	m_objects[nObjectsCurrent].vertices = new VertexType[m_fileCounts[nObjectsCurrent].nVertices];
	m_objects[nObjectsCurrent].texCoords = new TextureCoordType[m_fileCounts[nObjectsCurrent].nTextures];
	m_objects[nObjectsCurrent].normals = new NormalType[m_fileCounts[nObjectsCurrent].nNormals];
	m_objects[nObjectsCurrent].faces = new FaceType[m_fileCounts[nObjectsCurrent].nFaces];

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);
	if (fin.fail()) { return false; }

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			if (input == ' ')
			{
				fin >> m_objects[nObjectsCurrent].vertices[vertexIndex].x
					>> m_objects[nObjectsCurrent].vertices[vertexIndex].y
					>> m_objects[nObjectsCurrent].vertices[vertexIndex].z;

				m_objects[nObjectsCurrent].vertices[vertexIndex].z *= -1.0f;
				vertexIndex++;
			}

			if (input == 't')
			{
				fin >> m_objects[nObjectsCurrent].texCoords[texcoordIndex].u
					>> m_objects[nObjectsCurrent].texCoords[texcoordIndex].v;

				m_objects[nObjectsCurrent].texCoords[texcoordIndex].v = 1.0f - m_objects[nObjectsCurrent].texCoords[texcoordIndex].v;
				texcoordIndex++;
			}

			if (input == 'n')
			{
				fin >> m_objects[nObjectsCurrent].normals[normalIndex].x
					>> m_objects[nObjectsCurrent].normals[normalIndex].y
					>> m_objects[nObjectsCurrent].normals[normalIndex].z;

				m_objects[nObjectsCurrent].normals[normalIndex].z *= -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> m_objects[nObjectsCurrent].faces[faceIndex].vIndex3 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].tIndex3 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].nIndex3
					>> m_objects[nObjectsCurrent].faces[faceIndex].vIndex2 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].tIndex2 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].nIndex2
					>> m_objects[nObjectsCurrent].faces[faceIndex].vIndex1 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].tIndex1 >> temp
					>> m_objects[nObjectsCurrent].faces[faceIndex].nIndex1;

				faceIndex++;
			}
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	fout.open("path.txt");
	if (fout.fail()) { return false; }

	fout << "Vertex Count: " << m_fileCounts[nObjectsCurrent].nFaces * 3 << "\n\n";
	fout << "Data: \n\n";

	for (int i = 0; i < faceIndex; i++)
	{
		vIndex = m_objects[nObjectsCurrent].faces[i].vIndex1 - 1;
		tIndex = m_objects[nObjectsCurrent].faces[i].tIndex1 - 1;
		nIndex = m_objects[nObjectsCurrent].faces[i].nIndex1 - 1;

		fout << m_objects[nObjectsCurrent].vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].z << "\n";

		vIndex = m_objects[nObjectsCurrent].faces[i].vIndex2 - 1;
		tIndex = m_objects[nObjectsCurrent].faces[i].tIndex2 - 1;
		nIndex = m_objects[nObjectsCurrent].faces[i].nIndex2 - 1;

		fout << m_objects[nObjectsCurrent].vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].z << "\n";

		vIndex = m_objects[nObjectsCurrent].faces[i].vIndex3 - 1;
		tIndex = m_objects[nObjectsCurrent].faces[i].tIndex3 - 1;
		nIndex = m_objects[nObjectsCurrent].faces[i].nIndex3 - 1;

		fout << m_objects[nObjectsCurrent].vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent].vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent].texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent].normals[nIndex].z << "\n";
	}

	fout.close();

	return true;
}