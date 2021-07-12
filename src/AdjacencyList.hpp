#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H
#include <iostream>
#include <list>

/*
  This represents the set of adjacent nodes to the current node.
  It uses a doubly linked list format - where the head and tail are specialized nodes which should never be removed.
*/
using namespace std;

struct Node{
	Node* prev;
	Node* next;
    int vertexNumber;
	Node* partner;

    void tryDelete() {
		if(prev == nullptr) {
			throw "invalid";
		}
		if(next == nullptr) {
			throw "invalid";
		}
		//delete this;
	}

	~Node(){

		tryDelete();
		prev->next = next;
		next->prev = prev;
		prev = nullptr;
		next = nullptr;
	}
};

struct AdjacencyList{
	Node* head;
    Node* tail;

	AdjacencyList(){
		head = new Node();
		tail = new Node();
		head->prev = nullptr;
		head->next = tail;
		head->vertexNumber = -1;
		head->partner = nullptr;
		tail->prev = head;
		tail->next = nullptr;
		tail->vertexNumber = -1;
		tail->partner = nullptr;
	}

    void add_back(Node* newNode){
        newNode->prev = tail->prev;
        newNode->next = tail;
        tail->prev->next = newNode;
        tail->prev = newNode;

	}

	void add_front(Node* newNode){
		newNode->prev = head;
		newNode->next = head->next;
		head->next->prev = newNode;
		head->next = newNode;
	}

	void clear(){
		while(head->next != tail){
			delete head->next;
		}
	}

	bool isEmpty(){
		if(head->next == tail){
			return true;
		}
		return false;
	}


    void print(){
        for(Node* current = head->next; current != tail;
            current = current->next){
            cout<<current->vertexNumber<<" ";
		}
    }

};


#endif
