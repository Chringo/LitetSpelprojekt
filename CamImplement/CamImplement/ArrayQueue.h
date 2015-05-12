#pragma once

template <typename T>
class ArrayQueue
{
private:
	int front;
	int back;
	int capacity;
	T** arr;

public:
	ArrayQueue();
	ArrayQueue(const ArrayQueue& orig);
	~ArrayQueue();

	void enqueue(const T& element);
	T dequeue();
	T peek();
	bool isEmpty();
	int size();
};

template<typename T>
ArrayQueue<T>::ArrayQueue()
{
	front = 0;
	back = 0;
	capacity = 10;
	arr = new T*[capacity];
}

template<typename T>
ArrayQueue<T>::ArrayQueue(const ArrayQueue& orig)
{
	front = orig.front;
	back = orig.back;

	capacity = orig.capacity;
	arr = new T*[capacity];
	for (int i = 0; i < size(); i++)
	{
		*arr[i] = *orig.arr[i];
	}
}

template<typename T>
ArrayQueue<T>::~ArrayQueue()
{
	if (arr != nullptr)
	{
		while (front != back)
		{
			delete arr[front];
			arr[front] = nullptr;
			front++;
			front = front%capacity;
		}

		delete [] arr;
		arr = nullptr;
	}
}

template<typename T>
void ArrayQueue<T>::enqueue(const T& element) // Needs work
{
	if (size() == capacity)
	{
		T** temp = arr;

		arr = new T*[capacity];

		int sorting = front;
		for (int i = 0; i < size(); i++)
		{
			*arr[i] = *temp[sorting];
			sorting++;
		}
	}

	arr[back] = new T(element);
	back++;
	back = back%capacity;
}

template<typename T>
T ArrayQueue<T>::dequeue()
{
	if (size() == 0)
	{
		throw string("The queue is empty! Can´t dequeue.");
	}

	T toReturn = *arr[front];

	front++;
	front = front%capacity;

	return toReturn;
}

template<typename T>
T ArrayQueue<T>::peek()
{
	if (size() == 0)
	{
		throw string("The queue is empty! Can´t peek.");
	}
	return *arr[front];
}

template <typename T>
bool ArrayQueue<T>::isEmpty()
{
	if (size() > 0) return false;
	else return true;
}

template <typename T>
int ArrayQueue<T>::size()
{
	int size = back - front;
	if (size < 0)
	{
		size += capacity;
	}
	return size;
}