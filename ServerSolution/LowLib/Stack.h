#pragma once

template<class T>
class Stack
{
public:
	Stack();
	~Stack();

	bool empty();
	void push(T value);
	Node<T> pop();
	int size();

private:
	Node<T> *header;
};

template<class T>
Stack<T>::Stack() : header(nullptr)
{
}

template<class T>
Stack<T>::~Stack()
{
	if (empty())
		return;

	for (Node<T> *current_node = pop(); current_node != nullptr;)
		delete current_node;
}

template<class T>
bool Stack<T>::empty()
{
	if (header == nullptr)
		return true;
	
	return false;
}

template<class T>
void Stack<T>::push(T value)
{
	Node<T> *new_node = new Node<T>;
	if (empty())
	{
		header = new_node;
		new_node->value = value;
		new_node->Next = nullpt;
	}
	else
	{
		new_node->value = value;
		new_node->Next = header;
		header = new_node;
	}
}

template<class T>
Node<T> Stack<T>::pop()
{
	if (empty())
		return nullptr;

	Node<T> *current_node = header;
	header = current_node->Next;
	
	return current_node;
}

template<class T>
int Stack<T>::size()
{
	if (empty())
		return 0

	Node<T> *current_node = header;
	int count = 0;
	for (; current_node != nullptr;)
	{
		count++;
		current_node = current_node->Next;
	}

	return count;
}