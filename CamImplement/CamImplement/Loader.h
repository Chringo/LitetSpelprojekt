#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <string>

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
};

struct FileCountType
{
	int nVertices, nTextures, nNormals, nFaces;
};

class Loader
{
private:
	void FindModelFilename(Object object, char* filename);
	bool ReadFileCounts(char*);
	bool LoadDataStructures(char* filename);

private:
	ObjectType* m_objects;
	FileCountType* m_fileCounts;

	int nObjectsTotal;
	int nObjectsCurrent;

public:
	Loader();
	Loader(const Loader& obj);
	~Loader();

	void Initialize(int nObjects = 1);

	ObjectType& getObject(Object obj) const;
	int getVertexCount(Object index) const;
	int getIndexCount(Object index) const;
};

#endif