#pragma once

template<class T>
class CircularLinkedList
{
public:
	CircularLinkedList();
	~CircularLinkedList();

	bool add_front(T value);
	bool add(int position, T value);
	Node<T> remove(int position);
	bool replace(int position, T value);
	int find(T value);
	Node<T> find(int position);
	int size();
	bool empty();

private:
	Node<T> *header;
	Node<T> *tail;
};

template<class T>
CircularLinkedList<T>::CircularLinkedList() : header(nullptr), tail(nullptr)
{
}

template<class T>
CircularLinkedList<T>::~CircularLinkedList()
{
	if (empty())
		return;

	for (int i = 0; i < size(); i++)
	{
		Node<T> *current_node = remove(1);
		if (current_node)
			delete current_node;
	}
}

template<class T>
bool CircularLinkedList<T>::add_front(T value)
{
	Node<T> *new_node = new Node<T>;
	if (header == nullptr)
	{
		new_node->value = value;
		new_node->Next = tail;
		header = new_node;
		tail = new_node;
	}
	else
	{
		new_node->Next = header;
		new_node->value = value;
		header = new_node;
		tail->Next = header;
	}

	return true;
}

template<class T>
bool CircularLinkedList<T>::add(int position, T value)
{
	if (position == 1 || header == nullptr)
		return add_front(value);

	Node<T> *new_node = new Node<T>;
	Node<T> *current_node = header;

	for (int i = 0; i < position - 1 && current_node != nullptr ; i++)
		current_node = current_node->Next;

	if (current_node == nullptr)
		return false;

	new_node->value = value;
	new_node->Next = current_node->Next;
	current_node->Next = new_node;

	return true;
}

template<class T>
Node<T> CircularLinkedList<T>::remove(int position)
{
	if (empty())
		return nullptr;

	Node<T> *temp_node = nullptr;
	Node<T> *current_node = header;
	
	for (int i = 1; i < position && cuurent_node != nullptr; i++)
	{
		if (i == (position - 1))
			temp_node = current_node;

		current_node = current_node->Next;
	}

	if (cuurent_node == nullptr)
		return nullptr;

	temp_node->Next = current_node->Next;

	return current_node;
}

template<class T>
bool CircularLinkedList<T>::replace(int position, T value)
{
	Node<T> * current_node = header;

	for (int i = 1; i < position && current_node == nullptr; i++)
		current_node = current_node->Next;

	if (current_node == nullptr)
		return false;

	current_node->value = value;

	return true;
}

template<class T>
int CircularLinkedList<T>::find(T value)
{
	Node<T> *current_node = header;

	for (int i = 1; current_node != nullptr; i++)
	{
		if (current_node->value == value)
			return i;

		current_node = current_node->Next;
	}

	if (current_node == nullptr)
		return 0;
}

template<class T>
Node<T> CircularLinkedList<T>::find(int position)
{
	Node<T> *current_node = header;

	for (int i = 1; i < position && current_node != nullptr; i++)
		current_node = current_node->Next;

	if (current == nullptr)
		return nullptr;

	return current_node;
}

template<class T>
int CircularLinkedList<T>::size()
{
	Node<T> *current_node = header;

	int count = 1;
	for (; current_node != nullptr; count++)
		current_node = current_node->Next;

	return count;
}

template<class T>
bool CircularLinkedList<T>::empty()
{
	if (header == nullptr)
		return true;

	return false;
}

