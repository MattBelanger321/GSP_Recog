#include "AdjacencyList.hpp"
#include <string>
#include <iostream>
#include <memory>
/*
The strucure is used to store a sequence of in a tree structure.
It is the way that ears is stored.
*/
#ifndef SEQ_H
#define SEQ_H
struct Sequence;
using Sequence_ptr = std::shared_ptr<Sequence>;
struct Sequence {
	int s; /* source(s) and sink(t) vertex numbers of the Sequence */
    int t;
    Node *s_node;
    Node *t_node; /* source and sink nodes in adjacencyLists */
	Sequence_ptr l;
    Sequence_ptr r; /* left and right Sequence for tree structure */
	string comp; /* string for composition of the Sequence */
    string orientation; /* either forward edge or back edge */

	Sequence():s(-1),
               t (-1),
               s_node(nullptr),
               t_node(nullptr),
               l (nullptr),
               r(nullptr),
               comp (""),
               orientation(""){}

	Sequence(int source, int sink, string composition, Node* sourceNode = nullptr, Node* sinkNode = nullptr, string direction = "")
            :  s(source), t(sink), s_node(sourceNode), t_node(sinkNode), l(nullptr), r(nullptr), comp(composition), orientation(direction)
	{}

	void clear() {								/* function to reset the data of a sequence */
		s = -1;
		t = -1;
		s_node = nullptr;
		t_node = nullptr;
		l = nullptr;
		r = nullptr;
		comp = "";
        orientation = "";
	}

};

#endif
