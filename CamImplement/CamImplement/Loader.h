#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <d3d11.h>
#include "WICTextureLoader.h"

#pragma comment (lib, "d3d11.lib")

#define MAX_OBJECT_COUNT 8
#define TEXTURE_COUNT 8

enum Object
{
	Player,
	Enemy,
	Obstacle,
	Tile,
	Menu,
	Arrow,
	Won,
	Lost
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

struct FrameType		//For one frame
{
	VertexType* vertex;	//Every frame needs x,y,z coords
};						//so we use our VertexType struct

struct ObjectType
{
	VertexType* vertices;
	TextureCoordType* texCoords;
	NormalType* normals;
	FaceType* faces;
	FrameType* frames;
	int nFrames;

	void Delete()
	{
		delete[] vertices;
		delete[] texCoords;
		delete[] normals;
		delete[] faces;
		delete[] frames;
	}
};

struct FileCountType
{
	int nVertices, nTextures, nNormals, nFaces, nFrames;
};

class Loader
{
private:
	ObjectType** m_objects;
	FileCountType* m_fileCounts;
	ID3D11ShaderResourceView** m_textures;

	int nObjectsTotal;
	int nObjectsCurrent;

private:
	void FindModelFilename(Object object, char** filename);
	bool ReadFileCounts(char* filename);
	bool LoadDataStructures(char* filename);
	void LoadTextures(ID3D11Device* device);

public:
	Loader();
	Loader(const Loader& obj);
	~Loader();

	void Initialize(ID3D11Device* device, Object* objects, int nObjects);

	ObjectType* getObject(Object obj) const;
	ID3D11ShaderResourceView* getTexture(int index) const;
	int getVertexCount(Object index) const;
	int getIndexCount(Object index) const;
	int getTextureCoordCount(Object index) const;
	int getNormalCount(Object index) const;
	int getFrameCount(Object index) const;

	void ReleaseCOM();
};

#endif