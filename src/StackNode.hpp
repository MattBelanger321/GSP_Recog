#include "Tree.hpp"
#ifndef STACKNODE_H
#define STACKNODE_H

using Tree_ptr = std::shared_ptr<Tree>;
struct StackNode {
	int end;											/* vertex at the end of the sequence */
	Tree_ptr sp;
    Tree_ptr tail; /* construction sequence for parallel ear and tail of that ear */

	StackNode() : end(-1) {		/* constructor clears the data */
		sp = std::make_shared<Tree>();
		tail = std::make_shared<Tree>();
	}
};
#endif
