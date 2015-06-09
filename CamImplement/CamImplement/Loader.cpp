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
	delete[] m_objects;
	delete[] m_fileCounts;
	delete[] m_textures;
}

void Loader::FindModelFilename(Object object, char** filename)
{
	switch (object)
	{
		case Player:
		{
			*filename = "Meshes/dummyMan_UV.obj";
			break;
		}
		case Enemy:
		{
			*filename = "Meshes/dummyMan_UV.obj";
			break;
		}
		case Obstacle:
		{
			*filename = "Meshes/newTree.obj";
			break;
		}
		case objMap:
		{
			*filename = "Meshes/groundTile01.obj";
			break;
		}
		case Menu:
		{
			*filename = "Meshes/menu.obj";
			break;
		}
		case Arrow:
		{
			*filename = "Meshes/menuArrow.obj";
			break;
		}
		case Won:
		{
			*filename = "Meshes/menu.obj";
			break;
		}
		case Lost:
		{
			*filename = "Meshes/menu.obj";
			break;
		}
		default:
			break;
	}
}

ObjectType* Loader::getObject(Object obj) const
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

int Loader::getNormalCount(Object index) const
{
	return m_fileCounts[index].nNormals;
}

ID3D11ShaderResourceView* Loader::getTexture(int index) const
{
	return m_textures[index];
}

int Loader::getTextureCoordCount(Object index) const
{
	return m_fileCounts[index].nTextures;
}

int Loader::getFrameCount(Object index) const
{
	return m_fileCounts[index].nFrames;
}


bool Loader::ReadFileCounts(char* filename)
{
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

		if (input == 'a')
		{
			fin.get (input);
			if (input == ' ')
			{
				fin >> m_fileCounts[nObjectsCurrent].nFrames;
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
	std::ifstream fin;
	std::ofstream fout;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, frameIndex, frameCount, frameVertex;
	char input;
	char temp;

	m_objects[nObjectsCurrent]->vertices	= new VertexType[m_fileCounts[nObjectsCurrent].nVertices];
	m_objects[nObjectsCurrent]->texCoords	= new TextureCoordType[m_fileCounts[nObjectsCurrent].nTextures];
	m_objects[nObjectsCurrent]->normals		= new NormalType[m_fileCounts[nObjectsCurrent].nNormals];
	m_objects[nObjectsCurrent]->faces		= new FaceType[m_fileCounts[nObjectsCurrent].nFaces];
	m_objects[nObjectsCurrent]->frames		= new FrameType[m_fileCounts[nObjectsCurrent].nFrames];

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;
	frameIndex = -1;
	frameCount = 0;
	frameVertex = 0;

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

		if (input == 'a')
		{
			fin.get (input);
			if (input == ' ')
			{
				fin >> m_objects[nObjectsCurrent]->nFrames;
					   m_objects[nObjectsCurrent]->frames = new FrameType[m_objects[nObjectsCurrent]->nFrames];

			}

			if (input == 'f')
			{
				frameIndex++;
				m_objects[nObjectsCurrent]->frames[frameIndex].vertex = new VertexType[vertexIndex];
				frameVertex = 0;
			}

			if (input == 'v')
			{
				fin >> m_objects[nObjectsCurrent]->frames[frameIndex].vertex[frameVertex].x
					>> m_objects[nObjectsCurrent]->frames[frameIndex].vertex[frameVertex].y
					>> m_objects[nObjectsCurrent]->frames[frameIndex].vertex[frameVertex].z;

				m_objects[nObjectsCurrent]->frames[frameIndex].vertex[frameVertex].z *= -1.0f;
				frameVertex++;
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

void Loader::LoadTextures(ID3D11Device* device)
{
	CoInitialize(NULL);
	WCHAR* filename[TEXTURE_COUNT] = 
	{ 
		L"Meshes/Textures/playerTexture.png",
		L"Meshes/Textures/dummyEnemyTexture.png",
		L"Meshes/Textures/newTreeTexture.png",
		L"Meshes/Textures/groundTile.png",
		L"Meshes/Textures/titlescreen.png",
		L"Meshes/Textures/arrow.png",
		L"Meshes/Textures/won.png",
		L"Meshes/Textures/lost.png",
		L"Meshes/Textures/titlescreen.png"
	};
	m_textures = new ID3D11ShaderResourceView*[TEXTURE_COUNT];

	for (int i = 0; i < TEXTURE_COUNT; i++)
	{
		DirectX::CreateWICTextureFromFile(device, filename[i], nullptr, &m_textures[i]);
	}
}

void Loader::Initialize(ID3D11Device* device, Object* objects, int nObjects)
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
		m_fileCounts[i].nFrames = 0;
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

	LoadTextures(device);
}

void Loader::ReleaseCOM()
{
	for (int i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		if (m_objects[i]) { m_objects[i]->Delete(); }
	}

	for (int i = 0; i < TEXTURE_COUNT; i++)
	{
		if (m_textures[i]) { m_textures[i]->Release(); }
	}
}