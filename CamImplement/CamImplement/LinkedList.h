#pragma once

#include <string>

template<typename T>
class LinkedList 
{
public:
		LinkedList();
		LinkedList(const LinkedList& orig);
		virtual ~LinkedList();

		LinkedList& operator=(const LinkedList& orig);

		void insertLast(const T& value);
		void insertAt(int pos, const T& value);
		void insertFirst(const T& value);
		T elementAt(int pos) const;
		T removeFirst();
		T removeLast();
		T removeAt(int pos);
		int size() const;

	private:
		class Node
		{
			public:
				T value;
				Node *next = nullptr;

				Node(const T& value, Node* next = nullptr);
				~Node();
		};

		Node *first;
};


// Node constructor and destructor
template<typename T>
LinkedList<T>::Node::Node(const T& value, Node* next)
{
	this->value = value;
	this->next = next;
}
template<typename T>
LinkedList<T>::Node::~Node(){}


// Constructors and destructor
template<typename T>
LinkedList<T>::LinkedList()
{
	first = nullptr;
}
template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& orig)
{
	nrOfNodes = orig.nrOfNodes;
	if (orig.first == nullptr)
	{
		first = nullptr;
	}
	else
	{
		first = new Node(orig.first->value);

		Node* origIterator(orig.first->next);
		Node* thisIterator(first);

		while(origIterator != nullptr)
		{
			thisIterator->next = new Node(origIterator->value);
			thisIterator = thisIterator->next;
			origIterator = origIterator->next;
		}
	}
}
template<typename T>
LinkedList<T>::~LinkedList()
{
	Node* iterator = first;

	while (iterator != nullptr)
	{
		first = iterator->next;
		delete iterator;
		iterator = first;
	}
}


// Operator
template<typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& orig)
{
	Node* iterator = first;
	while (iterator != nullptr)
	{
		first = iterator->next;
		delete iterator;
		iterator = first;
	}

	if (orig.first == nullptr)
	{
		first = nullptr;
	}
	else
	{
		first = new Node(orig.first->value);

		Node* origIterator(orig.first->next);
		Node* thisIterator(first);

		while(origIterator != nullptr)
		{
			thisIterator->next = new Node(origIterator->value);
			thisIterator = thisIterator->next;
			origIterator = origIterator->next;
		}
	}

	return *this;
}


// Insert
template<typename T>
void LinkedList<T>::insertLast(const T& value)
{
	if (first == nullptr)
	{
		first = new Node(value);
	}
	else
	{
		Node* iterator = first;
		while (iterator->next != nullptr)
		{
			iterator = iterator->next;
		}
		iterator->next = new Node(value);
	}
}
template<typename T>
void LinkedList<T>::insertAt(int pos, const T& value)
{
	if(pos < 0 ||pos >= nrOfNodes)
	{
		throw string ("Out of bonds!");
	}
	Node* iterator = first;
	int counter = 0;
	while (counter < pos)
	{
		iterator = iterator->next;
		counter ++;
	}

	iterator->value = value;
}
template<typename T>
void LinkedList<T>::insertFirst(const T& value)
{
	first = new Node(value, first);
	nrOfNodes++;
}


// Element at
template<typename T>
T LinkedList<T>::elementAt(int pos) const
{
	if(pos < 0 ||pos >= size())
	{
		throw std::string ("Out of bonds!");
	}
	Node* iterator = first;
	int counter = 0;
	while (counter < pos)
	{
		iterator = iterator->next;
		counter ++;
	}

	return iterator->value;
}


// Remove
template<typename T>
T LinkedList<T>::removeFirst()
{
	if (first == nullptr)
	{
		throw std::string("List is empty!");
	}
	Node* temp = first;
	first = first->next;
	
	T value = temp->value;
	
	delete temp;
	
	return value;
}
template<typename T>
T LinkedList<T>::removeLast()
{
	if (first == nullptr)
	{
		throw std::string("List is empty!");
	}

	T value = -1;
	if (nrOfNodes == 1)
	{
		value = removeFirst();
	}
	else
	{
		Node* iterator = first;
		for (int i = 0; i < nrOfNodes-2; i++)
		{
			iterator = iterator->next;
		}
		Node* temp = iterator->next;
		iterator->next = nullptr;
		value = temp->value;
		delete temp;
	}
	nrOfNodes--;
	return value;
}
template<typename T>
T LinkedList<T>::removeAt(int pos)
{
	if (first == nullptr)
	{
		throw std::string("List is empty!");
	}
	if(pos < 0 ||pos >= nrOfNodes)
	{
		throw std::string ("Out of bonds!");
	}

	T value;
	if (nrOfNodes == 1)
	{
		value = removeFirst();
	}
	else
	{
		Node* iterator(first);
		int counter = 0;
		while (counter < pos-1)
		{
			iterator = iterator->next;
			counter ++;
		}
		Node* temp(iterator->next);
		iterator->next = temp->next;
		T value = temp->value;
		delete temp;
	}
	return value;
}


// Size
template<typename T>
int LinkedList<T>::size() const
{
	if (first == nullptr)
	{
		return 0;
	}
	

	int count = 0;
	Node* iterator = first;
	while (iterator->next != nullptr)
	{
		iterator = iterator->next;
		count++;
	}
	return count;
}