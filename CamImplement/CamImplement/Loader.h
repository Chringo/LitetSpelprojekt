#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <d3d11.h>
//#include <d3dx11.h>

//#pragma comment (lib, "d3dx11.lib")

enum Object
{
	Player,
	Block
};

struct VertexType
{
	VertexType(){}
	VertexType(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	float x, y, z;
	float nx, ny, nz;
};

struct TextureCoordType
{
	float u, v;
};

struct NormalType
{
	float x, y, z;
};

struct FaceType
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
};

struct ObjectType
{
	VertexType* vertices;
	TextureCoordType* texCoords;
	NormalType* normals;
	FaceType* faces;
	ID3D11Texture2D* texture;
};

struct FileCountType
{
	int nVertices, nTextures, nNormals, nFaces;
};

class Loader
{
private:
	ObjectType* m_objects;
	FileCountType* m_fileCounts;

	int nObjectsTotal;
	int nObjectsCurrent;

private:
	void FindModelFilename(Object object, char* filename);
	bool ReadFileCounts(char* filename);
	bool LoadDataStructures(char* filename);
	bool LoadTextures(ID3D11Device* device, char* filename);

public:
	Loader();
	Loader(const Loader& obj);
	~Loader();

	void Initialize(ID3D11Device* device, int nObjects);

	ObjectType& getObject(Object obj) const;
	int getVertexCount(Object index) const;
	int getIndexCount(Object index) const;
	int getNormalCount(Object index) const;
};

#endif