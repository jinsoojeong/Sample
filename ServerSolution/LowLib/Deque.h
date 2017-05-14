#pragma once

template<class T>
class Deque
{
public:
	Deque();
	~Deque();

	bool empty();
	void push_front(T value);
	void push_back(T value);
	DNode<T> pop_front();
	DNode<T> pop_back();
	int size();

private:
	DNode<T> *header;
	DNode<T> *tail;
};

template<class T>
Deque<T>::Deque() : header(nullptr), tail(nullptr)
{
}

template<class T>
Deque<T>::~Deque()
{
	if (empty())
		return;

	for (DNode<T> *current_node = pop_back(); current_node != nullptr;)
		delete current_node;
}

template<class T>
bool Deque<T>::empty()
{
	if (header == nullptr)
		return true;

	return false;
}

template<class T>
void Deque<T>::push_front(T value)
{
	DNode<T> *new_node = new DNode<T>;
	if (header == nullptr)
	{
		new_node->value = value;
		new_node->Next = nullptr;
		new_node->Prev = nullptr;

		header = new_node;
		tail = new_node;
	}
	else
	{
		header->Prev = new_node;
		new_node->Next = header;
		header = new_node;
		new_node->value = value;
		new_node->Prev = nullptr;
	}
}

template<class T>
void Deque<T>::push_back(T value)
{
	DNode<T> *new_node = new DNode<T>;
	if (tail == nullptr)
	{
		header = new_node;
		tail = new_node;
		new_node->value = value;
		new_node->Next = nullptr;
		new_node->Prev = nullptr;
	}
	else
	{
		new_node->value = value;
		tail->Next = new_node;
		new_node->Prev = tail;
		tail = new_node;
		new_node->Next = nullptr;
	}
}

template<class T>
DNode<T> Deque<T>::pop_front()
{
	if (empty())
		return nullptr;

	DNode<T> *current_node = nullptr;
	current_node = header;

	if (header->Next)
	{
		header->Next->Prev = nullptr;
		header = header->Next;
	}
	else
	{
		header = nullptr;
		taile = nullptr;
	}

	return current_node;
}

template<class T>
DNode<T> Deque<T>::pop_back()
{
	if (empty())
		return nullptr;

	DNode<T> *current_node = nullptr;
	current_node = tail;

	if (tail->Prev)
	{
		tail->Prev->Next = nullptr;
		tail = tail->Prev;
	}
	else
	{
		header = nullptr;
		taile = nullptr;
	}

	return current_node;
}

template<class T>
int Deque<T>::size()
{
	if (empty())
		return 0;

	DNode<T> *current_node = header;
	for (;current_node != nullptr;)
	{
		count++;
		current_node = current_node->Next;
	}

	return count;
}