#include "Sequence.hpp"
#include "AdjacencyList.hpp"
#include <fstream>
#include <list>
#include <string>

#ifndef TREE_H
#define TREE_H

/*
  This is always the root of a construction tree
  It also contains functions geared towards saving the tree later.
*/
using Sequence_ptr = std::shared_ptr<Sequence>;
struct Tree;
using Tree_ptr = std::shared_ptr<Tree>;
struct Tree { /* Decomposition Tree, consisting of Sequences */
	Sequence_ptr root;	/* root of the Tree, the SP graph it represents */

	Tree() {	/* default constructor establishes a nulled root */
		root = std::make_shared<Sequence>();
	}

	Tree(int s, int t, std::string comp, Node* sourceNode = nullptr, Node* sinkNode = nullptr, std::string direction = "") {
        /* constructor to fill in information for a tree */
		root =std::make_shared<Sequence>(s, t, comp, sourceNode, sinkNode, direction);
	}

	void print_tree() { /* Print calls the recursive helper function to print the tree */
		print_tree(root);
	}

	void print_tree(Sequence_ptr leaf) { /* recursively prints the tree composition */
		if(leaf->l == nullptr)	{/* if no children, print self */
			//outfile << "("<< leaf->s << "," << leaf->t << ")";
		}
        else {
			//outfile << leaf->ttcomp << "( "; /* else print self, then children */
			print_tree(leaf->l);
			//outfile << " , ";
			print_tree(leaf->r);
			//outfile << " )";
		}
	}

	string get_tree() { /* Print calls the recursive helper function to print the tree */
		return get_tree(root);
	}

	string get_tree(Sequence_ptr leaf) { /* recursively prints the tree composition */
		string toReturn = "";
        if (leaf== nullptr)
            return "";
		if(leaf->l == nullptr) {	/* if no children, print self */
			toReturn +=  "(" ;
			toReturn +=  to_string(leaf->s) ;
			toReturn +=  ",";
			toReturn +=  to_string(leaf->t);
			toReturn +=  ")";
		} else {
			toReturn += leaf->comp ; /* else print self, then children */
			toReturn += "( ";
			toReturn += get_tree(leaf->l);
			toReturn += " , ";
			toReturn += get_tree(leaf->r);
			toReturn += " )";
		}
		return toReturn;
	}


	void destroy_tree() { /* function to call the helper function to destroy the tree */
		destroy_tree(root);
	}

	void destroy_tree(Sequence_ptr leaf) { /* Helper function to free the memory of the Tree */
		if(leaf != nullptr) {
			destroy_tree(leaf->l);		/* destroy children, then self */
			destroy_tree(leaf->r);
            leaf = nullptr;
		}
	}

	~Tree() {											/* Destructor calls the recursive helper function to destroy the Tree */
		//destroy_tree();
        root = nullptr;
	}
};

inline Tree_ptr SC(Tree_ptr left, Tree_ptr right) { /* function to create a Series Composition of two SP graphs */
    if(!left)
    {
        return right;
    }
    if(!right)
    {
        return left;
    }
    Tree_ptr t = std::make_shared<Tree>(left->root->s, right->root->t, "SC"); /* create the new tree */
	//cout << "SC " << left->root->t << " -> " << right->root->s << endl;  //#DEGUG_PARALLEL
	t->root->l = left->root;																 /* assign subtrees */
	t->root->r = right->root;

	return t;
}

inline Tree_ptr PC(Tree_ptr left, Tree_ptr right) { /* function to create a Parallel composotion of two SP graphs*/

    if(!left )
    {
        return right;
    }
    if(!right )
    {
        return left;
    }
    Tree_ptr t =  std::make_shared<Tree>(left->root->s, left->root->t, "PC"); /* create the new tree */
	//cout << "PC " << left->root->s << " AND " << right->root->s << "  ->  ";//#DEGUG_PARALLEL
	//cout << "PC " << left->root->t << " AND " << right->root->t << endl;//#DEGUG_PARALLEL

    t->root->l = left->root;																/* assign subtrees */
	t->root->r = right->root;



	return t;
}

inline Tree_ptr DC(Tree_ptr left, Tree_ptr right) { /* function to create a dangling composotion of two SP graphs*/
    if(!left)
    {
        return right;
    }
    if(!right )
    {
        return left;
    }
    Tree_ptr t =  std::make_shared<Tree>(left->root->s, left->root->t, "DC", left->root->s_node, left->root->t_node); /* create the new tree */

	t->root->l = left->root;																/* assign subtrees */
	t->root->r = right->root;

	return t;
}
#endif
