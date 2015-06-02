#pragma once

#include "LinkedList.h"

namespace PF
{
	// Make PF map
	struct Map
	{
		int arrSize;
		//int** baseArr;
		bool** obstacleArr;
		Map(/*int** baseArr, */bool** obstacleArr, int arrSize)
		{
			this->arrSize = arrSize;
			//this->baseArr = baseArr;
			this->obstacleArr = obstacleArr;
		}
		~Map()
		{
			for (int i = 0; i < arrSize; i++)
			{
				//delete[] baseArr[i];
				delete[] obstacleArr[i];
			}
			//delete[] baseArr;
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
			~Coordinate() {}

			bool operator==(const Coordinate& right)
			{
				bool identical = false;
				if (x == right.x && z == right.z)
				{
					identical = true;
				}
				return identical;
			}
			bool operator!=(const Coordinate& right)
			{
				bool identical = true;
				if (x == right.x && z == right.z)
				{
					identical = false;
				}
				return identical;
			}
		};

		struct Emitter
		{
			int x;
			int z;
			int fallOff;
			int emitValue;

			Emitter(){}
			Emitter(int x, int z, int fallOff, int emitValue)
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

		static inline int** influenceMap(Emitter emitter, const Map& map);
		static inline LinkedList<Coordinate> Astar(Coordinate start, Coordinate goal, const Map& map);

	private:
		struct Node
		{
			Coordinate* self;
			Coordinate* parent;

			int gCost;
			int hCost;
			int fCost;

			Node() {}
			Node(Coordinate* self, int hCost)
			{
				this->self = new Coordinate(self->x, self->z);
				this->parent = nullptr;
				gCost = 0;
				this->hCost = hCost;
				this->fCost = hCost;
			}
			Node(Coordinate* self, Coordinate* parent, int hCost)
			{
				this->self = new Coordinate(self->x, self->z);
				if (parent != nullptr)
				{

					this->parent = new Coordinate(parent->x, self->z);
				}
				else
				{
					this->parent = nullptr;
				}
				this->gCost = 0;
				this->hCost = hCost;
				this->fCost = hCost;
			}
			Node(const Node& copy)
			{
				self = new Coordinate(copy.self->x, copy.self->z);
				if (copy.parent != nullptr)
				{
					parent = new Coordinate(copy.parent->x, copy.parent->z);
				}
				else
				{
					parent = nullptr;
				}
				gCost = copy.gCost;
				hCost = copy.hCost;
				fCost = copy.fCost;
			}
			~Node()
			{
				if (self != nullptr)
				{
					delete self;
					self = nullptr;
				}
				if (parent != nullptr)
				{
					delete parent;
					parent = nullptr;
				}
			}
		};

		static inline LinkedList<Coordinate> adjecentLinearNodes(const Coordinate* node, bool** disable, int amount);
		static inline LinkedList<Emitter> adjecentLinearNodes(const Emitter* node, bool** disable, int amount);
		static inline int limit(int check, int maxValue);


	};


	int** Pathfinding::influenceMap(Emitter emitter, const Map& map)
	{
		int** iMap = new int*[map.arrSize];
		for (int i = 0; i < map.arrSize; i++)
		{
			iMap[i] = new int[map.arrSize];
			for (int j = 0; j < map.arrSize; j++)
			{
				iMap[i][j] = 0;
			}
		}

		LinkedList<Emitter> toVisit = LinkedList<Emitter>();
		toVisit.insertLast(emitter);

		bool** disable = new bool*[map.arrSize];
		for (int i = 0; i < map.arrSize; i++)
		{
			disable[i] = new bool[map.arrSize];
			for (int j = 0; j < map.arrSize; j++)
			{
				disable[i][j] = map.obstacleArr[i][j];
			}
		}

		while (toVisit.size() != 0)
		{
			Emitter toProcess = toVisit.removeFirst();

			disable[toProcess.x][toProcess.z] = true;

			int asdf = iMap[toProcess.x][toProcess.z];

			if (iMap[toProcess.x][toProcess.z] < toProcess.emitValue)
			{
				iMap[toProcess.x][toProcess.z] = toProcess.emitValue;
			}

			Emitter newEmitterBase = Emitter(toProcess);
			newEmitterBase.emitValue -= toProcess.fallOff;

			if (newEmitterBase.emitValue <= 255 && newEmitterBase.emitValue > 0)
			{
				LinkedList<Emitter> toAdd = adjecentLinearNodes(&newEmitterBase, disable, map.arrSize);
				for (int j = 0; j < toAdd.size(); j++)
				{
					toVisit.insertLast(toAdd.elementAt(j));
				}
			}
		}

		for (int i = 0; i < map.arrSize; i++)
		{
			delete[] disable[i];
		}
		delete[] disable;

		return iMap;
	}
	LinkedList<Pathfinding::Coordinate> Pathfinding::Astar(Coordinate start, Coordinate goal, const Map& map)
	{
		int moveCost = 15;

		Emitter forIM = Emitter(goal.x, goal.z, -10, -0);
		
		int** iM = influenceMap(forIM, map);

		int amount = map.arrSize;

		Node*** nodeMap = new Node**[amount];
		for (int x = 0; x < amount; x++)
		{
			nodeMap[x] = new Node*[amount];
			for (int z = 0; z < amount; z++)
			{
				Coordinate* c = new Coordinate(x, z);
				nodeMap[x][z] = new Node(c, iM[x][z]);
			}
		}

		bool** disable = new bool*[map.arrSize];
		for (int i = 0; i < map.arrSize; i++)
		{
			disable[i] = new bool[map.arrSize];
			for (int j = 0; j < map.arrSize; j++)
			{
				disable[i][j] = map.obstacleArr[i][j];
			}
		}

		LinkedList<Coordinate*> openList = LinkedList<Coordinate*>();
		LinkedList<Coordinate*> closedList = LinkedList<Coordinate*>();

		Node* currentNode = nodeMap[start.x][start.z];
		openList.insertLast(currentNode->self);

		bool pathFound = false;
		int count = 0;
		while (openList.size() > 0)
		{
			count++;
			// Pick the node with the lowest F cost
			int index = 0;
			currentNode = nodeMap[openList.elementAt(0)->x][openList.elementAt(0)->z];
			for (int i = 1; i < openList.size(); i++)
			{
				Coordinate* c = openList.elementAt(i);
				if (nodeMap[c->x][c->z]->fCost < currentNode->fCost)
				{
					currentNode = nodeMap[c->x][c->z];
					index = i;
				}
			}

			openList.removeAt(index);
			closedList.insertLast(currentNode->self);
			//

			// Check if goal was reached
			if (currentNode->self->x == goal.x && currentNode->self->z == goal.z)
			{
				pathFound = true;
				break;
			}
			//

			// disable the position so it won´t show as adjacent
			disable[currentNode->self->x][currentNode->self->z] = true;
			//

			// Get adjecent coordinates of Nodes
			LinkedList<Coordinate> adj = adjecentLinearNodes(currentNode->self, disable, amount);
			//

			// Process adjecent nodes
			for (int i = 0; i < adj.size(); i++)
			{
				Coordinate* c = nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->self;

				if (nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->parent == nullptr)
				{
					nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->parent = new Coordinate( *currentNode->self );
					nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->gCost = currentNode->gCost + moveCost;
					nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->fCost = nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->gCost + nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->hCost;

					// Add to open list
					openList.insertLast(nodeMap[adj.elementAt(i).x][adj.elementAt(i).z]->self);
				}
				else if ((currentNode->gCost + moveCost) < nodeMap[c->x][c->z]->gCost)
				{
					nodeMap[c->x][c->z]->parent = new Coordinate( *currentNode->self );
					nodeMap[c->x][c->z]->gCost = currentNode->gCost + moveCost;
					nodeMap[c->x][c->z]->fCost = nodeMap[c->x][c->z]->gCost + nodeMap[c->x][c->z]->hCost;
				}
			}
		}

		// Save the path
		LinkedList<Coordinate> path = LinkedList<Coordinate>();
		if (pathFound) // Check if a path exists
		{
			while (currentNode->parent != nullptr)
			{
				path.insertFirst(*currentNode->self);
				currentNode = nodeMap[currentNode->parent->x][currentNode->parent->z];
			}
		}
		//

		for (int i = 0; i < amount; i++)
		{
			delete[] disable[i];
			delete[] iM[i];
			for (int j = 0; j < amount; j++)
			{
				delete nodeMap[i][j];
			}
			delete[] nodeMap[i];
		}
		delete[] disable;
		delete[] nodeMap;
		delete[] iM;

		return path;
	}

	LinkedList<Pathfinding::Coordinate> Pathfinding::adjecentLinearNodes(const Coordinate* node, bool** disable, int amount)
	{
		LinkedList<Coordinate> adjecentNodes = LinkedList<Coordinate>();

		Coordinate newNode = Coordinate(*node);
		newNode.x--; // linear
		if (newNode.x >= 0 && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			*disable[newNode.x, newNode.z] = true;
		}

		newNode = Coordinate(*node);
		newNode.x++; // linear
		if (newNode.x < amount && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		newNode = Coordinate(*node);
		newNode.z--; // Linear
		if (newNode.z >= 0 && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		newNode = Coordinate(*node);
		newNode.z++; // Linear
		if (newNode.z < amount && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		return adjecentNodes;
	}
	LinkedList<Pathfinding::Emitter> Pathfinding::adjecentLinearNodes(const Emitter* node, bool** disable, int amount)
	{
		LinkedList<Emitter> adjecentNodes = LinkedList<Emitter>();

		Emitter newNode = Emitter(*node);
		newNode.x--; // linear
		if (newNode.x >= 0 && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			*disable[newNode.x, newNode.z] = true;
		}

		newNode = Emitter(*node);
		newNode.x++; // linear
		if (newNode.x < amount && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		newNode = Emitter(*node);
		newNode.z--; // Linear
		if (newNode.z >= 0 && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		newNode = Emitter(*node);
		newNode.z++; // Linear
		if (newNode.z < amount && !disable[newNode.x][newNode.z])
		{
			adjecentNodes.insertLast(newNode);
			disable[newNode.x][newNode.z] = true;
		}

		return adjecentNodes;
	}
	int Pathfinding::limit(int check, int maxValue)
	{
		if (check < 0) check = 0;
		else if (check > maxValue) check = maxValue;
		return check;
	}

}