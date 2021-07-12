#include <string>
#include <array>
#include <vector>
#include <iostream>

#include <list>
#include "Tree.hpp"
#include "Graph.hpp"

#ifndef CERT_H
#define CERT_H

using Tree_ptr = std::shared_ptr<Tree>;
using namespace std;
struct Certificate {
	int type;  //the type of the certifiacet - represents what must be looked at.
	int single;  //the one item talked abuot - may be an node or biconnected components
	int remove1 = -1, remove2 = -1;
	list<int> others; //the other three items talked abuot - may be nodes or biconnected components
	static const int VERTEX_IN_THREE_BICONNECTED = 1,
        THREE_VERTEX_IN_BICONNECTED = 2,
        K4GRAPH = 3,
        K23GRAPH = 4,
        SP_AND_OP = 0,
        THETHA_GRAPH = 5,
        T_AND_K23=6; //a constant for each type.
	int ignore = -1;

	Tree_ptr constructionTree;  //the construction sequence for SP
    std::array<vector<pair<int,int>>, 6> certPaths;  //the set of sequences that make up the forbidden structure
    std::array<vector<pair<int,int>>, 6> thetha_paths; // paths for the thetha4 graph // same for certPaths, but some cases require to output 2 different graphs

	bool SP = true, OP = true, GSP = true;

	vector<pair<int,int>> boundary;

	//basic constructor
	Certificate() : type(-1), single(-1),constructionTree(nullptr) {}

	//set up some basic variables
	void clear() {
		type = -1;
		single = -1;
	}

	//a function to print out the certificate
	inline string print() {
		string ret = "";
		if (type == VERTEX_IN_THREE_BICONNECTED) {
			//if it is one node inside of the 3 seperate biconnected components -
			ret += "Node ";
			ret += to_string(single);  //the one node
			ret += " is in ";
			for (std::list<int>::iterator it=others.begin(); it != others.end(); ++it) ret += " " + to_string(*it);  //the ones it is in.
		}
        else if (type == THREE_VERTEX_IN_BICONNECTED) {
			//if it is three articulation points in one biconnected component
			ret += "Biconnected Component ";
			ret += to_string(single);  //the compoenent
			ret += " has cut points ";
			for (std::list<int>::iterator it=others.begin(); it != others.end(); ++it) ret += " " + to_string(*it);  //the vertex
		}
        else if (type == K4GRAPH) {
			ret += "K4 Graph Found\n";
		}
        else if (type == THETHA_GRAPH) {
			ret += "Thetha Graph Found\n";
		}
        else if (type == K23GRAPH) {
			ret += "K23 Graph Found\n";
        }
        else if (type == SP_AND_OP) {
			ret += "The graph is both SP and OP\n";
		}
        else {
			//ret += "FAILED - CERT CHECK\n";  //if this happens then something went wrong
            ret+= "\n";
		}

		if (GSP) {
			ret += "The graph is GSP\n";
		}
        if (SP)
        {
            ret += "The graph is SP\n";
        }
		if (OP) {
			ret += "The graph is OP\n";
			ret += "Boundary Edges as Vertex Number Pairs\n";
			for(pair<int,int>& p : boundary){
				ret+= "(" +to_string(p.first) + " " + to_string(p.second) + ") ";
			}
            ret+="\n";
		}
        if (GSP) {
			ret += "SP construction sequence\n";
            ret += constructionTree->get_tree() + "\n";
        }
		//if the issue was a forbidden structure - output it
		if (type == K23GRAPH || type == K4GRAPH)
        {
			if (type == K23GRAPH) ret += "The k23 graph\n";
			else ret += "The k4 graph\n";
			for (int k = 0; k < 6; ++k) {
				ret += to_string(k);
				for (auto n : certPaths[k] ) {
					if (k != ignore) {
						ret += " (";
						ret += to_string(n.first);
						ret += ",";
						ret += to_string(n.second);
						ret += ") ";
					}
				}
				ret += "\n";
			}
		}
        if (type == THETHA_GRAPH)
        {
            ret += "The Thetha4 graph\n";
			for (int k = 0; k < 6; ++k) {
				ret += to_string(k);
				for (auto n : thetha_paths[k] ) {
					if (k != ignore) {
						ret += " (";
						ret += to_string(n.first);
						ret += ",";
						ret += to_string(n.second);
						ret += ") ";
					}
				}
				ret += "\n";
			}
		}

		return ret;
	}

	//a function to oombine 2 paths into one longer path.
	inline void combinePaths (vector<pair<int,int>> &a, vector<pair<int,int>> &b ) {
		auto it = a.begin();
		for( auto c : b) {
			it = a.insert(it,c);
		}
	}


	//a function to confirm a certificate against a new graph.
	inline bool confirmCert( Graph& full) {

		bool ok = true;
		if (type == -1) {
			cout << "INCORRECT CERTIFICATE TYPE" << endl;  //if it is -1 still there is a problem
			return false;
		}


		//if a k4 or k23 was detected.
		if (type == K4GRAPH || type == K23GRAPH )
        {
			int endpoints[12];
			//for all of the edges, set up an array for the endpoints to check.
			for (int k =0; k < 6; k++)
            {
                if (certPaths.size() > 0)
                {
                    endpoints[k * 2] = certPaths[k].front().first;
                    endpoints[(k * 2) + 1] = certPaths[k].back().second;
                }
            }
			try
            {
				if (type == K4GRAPH) full.Verify_K4_Structure(endpoints);  //if it is k4 we should be able to verify the endpoints
				else full.Verify_K23_Structure(endpoints); //if it is k23 we should be able to verify the endpoints
			}
            catch (...) {
				cout << "THE PROGRAM HAS A BUG";  //if it throws that is a problem.
				return false;
			}
			for (int k =0; k < 6; k++) {  //run through and confirm the paths on the original.
				if (k == ignore) continue;
				if (!full.confirmPath(certPaths[k], k, endpoints) ) cout << k << "FAILED" << endl;
				ok = ok & full.confirmPath(certPaths[k], k, endpoints );
			}
		}
		if (type != K4GRAPH ) {
			if (!full.isValidCertificateFull (constructionTree) ) {
				ok = false;
			}
		}

		return ok;
	}
};

#endif
