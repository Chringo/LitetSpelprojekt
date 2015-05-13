#pragma once

#include "LinkedList.h"

template <typename T>
class LQueue
{
public:
	LQueue();
	void Enqueue(const T &element);
	T Dequeue();
	void ReDequeue();
	int Size() const;

private:
	LinkedList<T> list;
};

template<typename T>
LQueue<T>::LQueue()
{
	list = LinkedList<T>();
}

template<typename T>
void LQueue<T>::Enqueue(const T &element)
{
	list.insertLast(element);
}

template<typename T>
T LQueue<T>::Dequeue()
{
	return list.removeFirst();
}

template<typename T>
void LQueue<T>::ReDequeue()
{
	list.insertLast(list.removeFirst());
}

template<typename T>
int LQueue<T>::Size() const
{
	return list.size();
}