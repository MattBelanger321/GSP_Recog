#include "AdjacencyList.hpp"
#include "StackNode.hpp"
#include <list>
#ifndef VERTEX_H
#define VERTEX_H
/*
  This is a class which represents a vertex.
*/
using Tree_ptr = std::shared_ptr<Tree>;

struct Vertex {	/* class used to keep track of information of a vertex */

	int dfsNumber;	/* number(position) given by the depth first serach */
	int parent;	/* parent of the vertex */
	int numberOfChildren; /* number of children of the vertex in the DFS tree*/
	std::list<StackNode> stk;	/* stack of parallel ears, to be reduced when dfs returns to the current vertex */
	Tree_ptr ear;	/* ear of the parent edge that the vertex belongs to */
	bool parentEdge; /* set to false after initial backtrack to parent vertex */
	bool balert; /* boundary error alert */
	Tree_ptr b; /*To memorize the B path when test K23*/
	int k4visit; /*To validate negative cercificate*/
	int k23visit; /*To validate negative cercificate*/
    AdjacencyList adjacencyList;	/* list of adjacent vertices */


    Vertex() : dfsNumber(-1), parent(-1), numberOfChildren(0), parentEdge(true), balert(false), b(nullptr), k4visit(0), k23visit(0) {
        /* constructor clears the data */
		adjacencyList.clear();
		stk.clear();
		ear = std::make_shared<Tree>();
	}

	void clear() {	/* reset all the data */
		adjacencyList.clear();
		dfsNumber = -1;
		parent = -1;
		stk.clear();
		//ear = new Tree();
        ear = std::make_shared<Tree>();
		parentEdge = true;
		balert = false;
		numberOfChildren = 0;
		k4visit = 0;
		k23visit=0;
	}

	~Vertex() {/* destructor deletes the ear tree and clears the stack */
		//delete ear;
		stk.clear();
	}

};

#endif
