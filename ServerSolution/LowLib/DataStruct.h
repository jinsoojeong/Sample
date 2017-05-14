#pragma once

template<class T>
class Node
{
public:
	T value;
	Node<T> *Next;
};

template<class T>
class DNode
{
public:
	T value;
	DNode<T> *Prev;
	DNode<T> *Next;
};

#include "Stack.h"
#include "Queue.h"
#include "Deque.h"
#include "LinkedList.h"
#include "CircularLinkedList.h"