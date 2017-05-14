#pragma once

template<class T>
class Queue
{
public:
	Queue();
	~Queue();

	bool empty();
	void enqueue(T value);
	Node<T> dequeue();
	int size();

private:
	Node<T> *header;
	Node<T> *tail;
	Node<T> *pre_tail;
};

template<class T>
Queue<T>::Queue() : header(nullptr), tail(nullptr), pre_tail(nullptr)
{
}

template<class T>
Queue<T>::~Queue()
{
	if (empty())
		return;

	for (Node<T> *current_node = dequeue(); current_node != nullptr;)
		delete current_node;
}

template<class T>
bool Queue<T>::empty()
{
	if (header == nullptr)
		return true;

	return false;
}

template<class T>
void Queue<T>::enqueue(T value)
{
	Node<T> *new_node = new Node<T>;
	if (empty())
	{
		new_node->value = value;
		new_node->Next = nullptr;
		header = new_node;
		tail = new_node;
	}
	else
	{
		if (pre_tail == nullptr)
		{
			new_node->Next = header;
			new_node->value = value;
			pre_tail = new_node;
			header = new_node;
		}
		else
		{
			new_node->Next = header;
			new_node->value = value;
			header = new_node;
		}
	}
}

template<class T>
Node<T> Queue<T>::dequeue()
{
	if (empty())
		return nullptr;

	Node<T> *temp_node = nullptr;
	Node<T> *remove_node = nullptr;
	Node<T> *current_node = header;

	for (;current_node != pre_tail;)
	{
		temp_node = current_node;
		current_node = current_node->Next;
	}

	remove_node = tail;
	pre_tail = temp_node;
	tail = temp_node->Next;
	tail->Next = nullptr;
	
	return remove_node;
}

template<class T>
int Queue<T>::size()
{
	if (empty())
		return 0;

	Node<T> *current_node = header;
	for (;current_node != nullptr;)
	{
		count++;
		current_node = current_node->Next;
	}

	return count;
}