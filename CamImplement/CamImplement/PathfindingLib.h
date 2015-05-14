#pragma once

#include "LinkedList.h"

namespace PF
{
	// Make PF map
	struct Map
	{
		int arrSize;
		int** baseArr;
		bool** obstacleArr;
		Map(int** baseArr, bool** obstacleArr, int arrSize)
		{
			this->arrSize = arrSize;
			this->baseArr = baseArr;
			this->obstacleArr = obstacleArr;
		}
		~Map()
		{
			for (int i = 0; i < arrSize; i++)
			{
				delete[] baseArr[i];
				delete[] obstacleArr[i];
			}
			delete[] baseArr;
			delete[] obstacleArr;
		}
	};

	class Pathfinding
	{
	public:
		struct Coordinate
		{
			int x;
			int z;
			Coordinate() {}
			Coordinate(int x, int z)
			{
				this->x = x;
				this->z = z;
			}
			Coordinate(const Coordinate& copy)
			{
				this->x = copy.x;
				this->z = copy.z;
			}
		};

		struct Emitter
		{
			int x;
			int z;
			int fallOff;
			int emitValue;

			Emitter(){}
			Emitter(int x, int z, int fallOff, int emitvalue)
			{
				this->x = x;
				this->z = z;
				this->fallOff = fallOff;
				this->emitValue = emitValue;
			}
			Emitter(const Emitter& copy)
			{
				this->x = copy.x;
				this->z = copy.z;
				this->fallOff = copy.fallOff;
				this->emitValue = copy.emitValue;
			}
		};

		static int** influenceMap(Emitter emitter, int** baseArr, bool** disableArr, int arrSize);
		static LinkedList<Coordinate> Astar(Coordinate start, Coordinate goal, int** baseArr, bool** disableArr, int arrSize);

	private:
		struct Node
		{
			Node() {}
			Node(Coordinate self, Coordinate parent, int hCost)
			{
				this->self = self;
				this->parent = parent;
				this->hCost = hCost;
			}
			Node(const Node& copy)
			{
				self = copy.self;
				parent = copy.parent;
				hCost = copy.hCost;
			}

			Coordinate self;
			Coordinate parent;

			int gCost;
			int hCost;
			int fCost;

		};

		static LinkedList<Coordinate> adjecentLinearNodes(Coordinate node, bool** disable, int amount);
		static int limit(int check, int maxValue);


	};
}