#include "Loader.h"

Loader::Loader()
{
	nObjectsTotal = 0;
	nObjectsCurrent = 0;
}

Loader::Loader(const Loader& obj)
{

}

Loader::~Loader()
{
	for (int i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		m_objects[i]->Delete();
	}
	delete[] m_objects;
	delete[] m_fileCounts;
}

void Loader::Initialize(Object* objects, int nObjects)
{
	nObjectsTotal = nObjects;
	m_fileCounts = new FileCountType[MAX_OBJECT_COUNT];
	m_objects = new ObjectType*[MAX_OBJECT_COUNT];
	for (int i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		m_fileCounts[i].nVertices = 0;
		m_fileCounts[i].nTextures = 0;
		m_fileCounts[i].nNormals = 0;
		m_fileCounts[i].nFaces = 0;
		m_objects[i] = nullptr;
	}

	char* filename = nullptr;

	for (int i = 0; i < nObjectsTotal; i++)
	{
		if (!m_objects[objects[i]])
		{
			nObjectsCurrent = objects[i];
			m_objects[objects[i]] = new ObjectType();
			FindModelFilename(objects[i], &filename);
			if (ReadFileCounts(filename))
			{
				LoadDataStructures(filename);
			}
		}
	}

	//filename = "dummyTexture.png";
	//LoadTextures(device, filename);
}

ObjectType& Loader::getObject(Object obj) const
{
	return *m_objects[obj];
}

int Loader::getVertexCount(Object index) const
{
	return m_fileCounts[index].nVertices;
}

int Loader::getIndexCount(Object index) const
{
	return m_fileCounts[index].nFaces * 3;
}

int Loader::getNormalCount(Object index) const
{
	return m_fileCounts[index].nNormals;
}

void Loader::FindModelFilename(Object object, char** filename)
{
	char* localFilename;
	switch (object)
	{
		case Player:
		{
			*filename = "Meshes/dummyMan.obj";
			break;
		}
		case Enemy:
		{
			*filename = "Meshes/dummyCube2.obj";
			break;
		}
		case Obstacle:
		{
			*filename = "Meshes/dummyPlane.obj";
			break;
		}
		case Tile:
		{
			*filename = "Meshes/dummyCube2.obj";
			break;
		}
		default:
			break;
	}
}

bool Loader::ReadFileCounts(char* filename)
{
	//if (nObjectsCurrent == nObjectsTotal) { return false; }

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
	//if (nObjectsCurrent == nObjectsTotal) { return false; }

	std::ifstream fin;
	std::ofstream fout;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	int vIndex, tIndex, nIndex;
	char input;
	char temp;

	m_objects[nObjectsCurrent]->vertices = new VertexType[m_fileCounts[nObjectsCurrent].nVertices];
	m_objects[nObjectsCurrent]->texCoords = new TextureCoordType[m_fileCounts[nObjectsCurrent].nTextures];
	m_objects[nObjectsCurrent]->normals = new NormalType[m_fileCounts[nObjectsCurrent].nNormals];
	m_objects[nObjectsCurrent]->faces = new FaceType[m_fileCounts[nObjectsCurrent].nFaces];

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
				fin >> m_objects[nObjectsCurrent]->vertices[vertexIndex].x
					>> m_objects[nObjectsCurrent]->vertices[vertexIndex].y
					>> m_objects[nObjectsCurrent]->vertices[vertexIndex].z;

				m_objects[nObjectsCurrent]->vertices[vertexIndex].z *= -1.0f;
				vertexIndex++;
			}

			if (input == 't')
			{
				fin >> m_objects[nObjectsCurrent]->texCoords[texcoordIndex].u
					>> m_objects[nObjectsCurrent]->texCoords[texcoordIndex].v;

				m_objects[nObjectsCurrent]->texCoords[texcoordIndex].v = 1.0f - m_objects[nObjectsCurrent]->texCoords[texcoordIndex].v;
				texcoordIndex++;
			}

			if (input == 'n')
			{
				fin >> m_objects[nObjectsCurrent]->normals[normalIndex].x
					>> m_objects[nObjectsCurrent]->normals[normalIndex].y
					>> m_objects[nObjectsCurrent]->normals[normalIndex].z;

				m_objects[nObjectsCurrent]->normals[normalIndex].z *= -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> m_objects[nObjectsCurrent]->faces[faceIndex].vIndex3 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].tIndex3 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].nIndex3
					>> m_objects[nObjectsCurrent]->faces[faceIndex].vIndex2 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].tIndex2 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].nIndex2
					>> m_objects[nObjectsCurrent]->faces[faceIndex].vIndex1 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].tIndex1 >> temp
					>> m_objects[nObjectsCurrent]->faces[faceIndex].nIndex1;

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
		vIndex = m_objects[nObjectsCurrent]->faces[i].vIndex1 - 1;
		tIndex = m_objects[nObjectsCurrent]->faces[i].tIndex1 - 1;
		nIndex = m_objects[nObjectsCurrent]->faces[i].nIndex1 - 1;

		fout << m_objects[nObjectsCurrent]->vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].z << "\n";

		vIndex = m_objects[nObjectsCurrent]->faces[i].vIndex2 - 1;
		tIndex = m_objects[nObjectsCurrent]->faces[i].tIndex2 - 1;
		nIndex = m_objects[nObjectsCurrent]->faces[i].nIndex2 - 1;

		fout << m_objects[nObjectsCurrent]->vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].z << "\n";

		vIndex = m_objects[nObjectsCurrent]->faces[i].vIndex3 - 1;
		tIndex = m_objects[nObjectsCurrent]->faces[i].tIndex3 - 1;
		nIndex = m_objects[nObjectsCurrent]->faces[i].nIndex3 - 1;

		fout << m_objects[nObjectsCurrent]->vertices[vIndex].x << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].y << ' '
			<< m_objects[nObjectsCurrent]->vertices[vIndex].z << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].u << ' '
			<< m_objects[nObjectsCurrent]->texCoords[tIndex].v << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].x << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].y << ' '
			<< m_objects[nObjectsCurrent]->normals[nIndex].z << "\n";
	}

	fout.close();

	return true;
}

bool Loader::LoadTextures(ID3D11Device* device, char* filename)
{


	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 800;
	desc.Height = 600;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	return false;
}