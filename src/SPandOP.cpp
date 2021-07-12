#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "SPandOP.hpp"

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                                boost::no_property,boost:: property <boost:: edge_component_t, std::size_t > >graph_t;
typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_t;
using Tree_ptr = std::shared_ptr<Tree>;

//return the certificate as a string.
// string certSPandOP(graph_t graph,int size) {
// 	return runSPandOP (graph,size).print();
// }

// //returns a boolean as to weather or not the graph is outer planar
// bool isOP(graph_t graph,int size) {
// 	return runSPandOP (graph,size).OP;
// }

// //returns a boolean as to weather or not the graph is series parallel.
// bool isSP(graph_t graph,int size) {
// 	return runSPandOP (graph,size).SP;
// }


// //takes in one of our own graph objects, a certifite object - which will be altered.
//it needs the source, and the sink node.  This allows us to force a particular edge to be first.
//This is needed to make the construction graph.  For graphs where no edge is required, -1 is used instead.
void run (Graph &g, Certificate &cert, int source, int sink,  std::map<int,  Tree_ptr>&  cut_point_tree_map) {

    //add the required edge so it is the first to be used if the edge does not actually exist.  If both are defined we can do it like so.
    if (source != -1 && sink != -1  && !g.Verify_Edge(source, sink) ) {
        g.addEdge(source, sink);
        g.addedForcedEdge = true;   //make a note in the graph that we added an edge. If we just adjust the order - it doesn't matter to the end result.
        g.forcedSink = sink;
    }

    //if the sink is defined and not the source - get a random source to use to get this sink.
    if (source == -1 && sink != -1) {
        source = g.vertices[sink].adjacencyList.head->next->vertexNumber;  //get one we know has an edge
    }

    //if both the source and sink are not defined - the only possibility left - then set the source to 0
    if (source == -1) source = 0;

    //these move edge the adjacencyList so that it if first - if the sink was defined.
    if (sink != -1 ) {
        g.setupSourceAndSink(source, sink);
        g.setupSourceAndSink(sink, source);
    }

    g.forcedSink = sink;  //let g know for later that the sink was defined.


    //make sure all verticies are unvisited.
    for(int i = 0; i < g.numV; ++i) { /* for all the vertices in the graph */
        g.vertices[i].dfsNumber = -1; /* mark vertice as unvisited */
        g.vertices[i].stk.clear();		/* clear the stack */
    }

    // set up the root node.
    int n = g.vertices[source].adjacencyList.head->next->vertexNumber;
    Node* current;
    for(current=g.vertices[n].adjacencyList.head->next;
        current!=nullptr &&current->vertexNumber != source;
        current = current->next);

    g.r_node = current;
    g.r = current->vertexNumber; //which will be our root.



    auto seq = std::make_shared<Tree>();	// Tree to record the decomposition sequence
    vector<pair<int,int>> boundary;  // the boundry edges
    bool k23found = false;			//if we have found a k23.

    //if we are dealing with a bridge, add an extra edge so that there is an ear.
    if (g.checkTwoVertexStructure() ) {
        if(sink==-1)
            sink = g.vertices[source].adjacencyList.head->next->vertexNumber;
        g.addEdge(source, sink);
        g.addedForcedEdge = true;   //make a note in the graph that we added an edge. If we just adjust the order - it doesn't matter to the end result.
        g.forcedSink = sink;
    }

    try {
        g.GenCS(g.r_node, nullptr, seq, boundary, k23found, cut_point_tree_map);
        // generate the construction sequence
        if(!(k23found)){
            cert.OP = true;
            cert.type = cert.SP_AND_OP;
        } else {
            cert.type = cert.K23GRAPH;
            cout << "An k23 was found!"<< endl;
            for (int k = 0; k < 6; ++k) cert.certPaths[k] = g.certPaths[k];
        }
        cert.SP = true;
        // if an error wasn't thrown, the graph is SP

        if(!(k23found)){
            cert.boundary = boundary;
        }
        //seq->print_tree(); // Print the tree decomposition
        cert.constructionTree = seq;

    } catch(const char* e) {
        cert.type = cert.K4GRAPH;
        for (int k = 0; k < 6; ++k) cert.certPaths[k] = g.certPaths[k];
        // if there was an error, catch it
    }
}

/*
  This function takes in a boost graph, its size, and the program arugments (For the out file) and creates a certificate.  Returning a certificate.
*/
// Certificate runSPandOP (graph_t g2, int size) {
// 	Certificate mainCert;  //the certificate object for the entire graph.

// 	Graph g(size);  //one of our own graphs - used to confirm the output - whcih will be the full graph.

// 	//needed for the boost graph.
// 	boost:: property_map <graph_t, boost::edge_component_t>::type
//             component = get(boost::edge_component_t(), g2);

// 	//get back the number of components.
// 	std::size_t num_comps = biconnected_components(g2, component);

// 	//create a internal graph object and certificate for each component to later use.
// 	// Graph comp [num_comps](size);
// 	// Certificate certs[num_comps];
//     std::vector<Graph> comp;
//     for(size_t i=0;i<num_comps;i++)
//     {
//         comp.emplace_back(size);
//     }
//     std::vector<Certificate> certs(num_comps);
// 	std::cerr << "Found " << num_comps << " biconnected components.\n";

// 	//get the articulation points.
// 	std::vector<vertex_t> art_points;
// 	articulation_points(g2, std::back_inserter(art_points));
// 	std::cerr << "Found " << art_points.size() << " articulation points.\n";


// 	map<int, list<int> > cutPoints, containsCuts;  //cutPoints maps the articulation points to the biconnected components they are a part of.
// 													//contains cuts maps the biconnected components to the articulation points inside of them.
// 	map<int, bool > isCutPoint;						//isCutPoint is set to true if the vertex is an articulation point.  The hash map is typically faster then searching.

// 	//move the list of cut points into the hash map
// 	for (std::size_t i = 0; i < art_points.size(); ++i) {
// 		isCutPoint[art_points[i]] = true;
// 	}

// 	boost:: graph_traits < graph_t >::edge_iterator ei, ei_end;	//set up an iterator to go through all edges.

// 	for (boost::tie(ei, ei_end) = edges(g2); ei != ei_end; ++ei) {

// 		comp[component[*ei]].addEdge(source(*ei, g2), target(*ei, g2));  //add the edge to the component it is a part of.
// 		g.addEdge(source(*ei, g2), target(*ei, g2));						//add the edge to the full graph we are also creating.

// 		//for both vertexes, add the details to the mapping for later.
// 		if ( isCutPoint[source(*ei, g2)] ) {
// 			cutPoints[source(*ei, g2) ].push_front(component[*ei]);
// 			containsCuts[component[*ei]].push_front(source(*ei, g2));
// 		}
// 		if (isCutPoint[target(*ei, g2)] ) {
// 			cutPoints[target(*ei, g2) ].push_front(component[*ei]);
// 			containsCuts[component[*ei]].push_front(target(*ei, g2));
// 		}

// 	}

// 	//go through the articulation points again.
// 	for (std::size_t i = 0; i < art_points.size(); ++i) {
// 		cutPoints[art_points[i]].sort(); //sort the attached list.
// 		cutPoints[art_points[i]].unique();	//remove duplicates - for this to work the list must be sorted.

// 		if (cutPoints[art_points[i]].size() > 2) {  //if it fails the max of 2 test - set up the certificate

// 			mainCert.type = mainCert.VERTEX_IN_THREE_BICONNECTED;
// 			mainCert.single = art_points[i];
// 			int number = cutPoints[art_points[i]].size();
// 			for(int kl = 0; kl < number; kl++ ) {  //move the data into the certificate.
// 				mainCert.others.push_back( cutPoints[art_points[i]].front() );
// 				cutPoints[art_points[i]].pop_front();
// 			}
// 			break;
// 		}
// 	}
// 	int first = -1, last = -1; //there can only be 2 components with 1 in each if it is SP.  The first and last in the chain.

// 	//run through all of the components.
// 	for (std::size_t i = 0; i < num_comps; ++i) {
// 		int size;

// 		containsCuts[i].sort();  //sort out the list.
//     	containsCuts[i].unique();//remove duplicates - needs to be sorted to work.
// 		if ( (size = containsCuts[i].size() ) == 1 ) {  //if there is only one articulation points then it is either first or last.
// 			if (first == -1) first = i;	//if first is already choosen then it must be last.
// 			else last = i;
// 		} else if (size > 2) { //if it fails set up the certificate - as there is a max of 2

// 			mainCert.type = mainCert.THREE_VERTEX_IN_BICONNECTED;
// 			mainCert.single = i;
// 			int number = containsCuts[i].size();
// 			for(int kl = 0; kl < number; kl++ ) {
// 				mainCert.others.push_back( containsCuts[i].front() );
// 				containsCuts[i].pop_front();
// 			}
// 			break;
// 		}
// 	}



// 	if (mainCert.type != -1 ){  //print out the certificat and end - as if it were changed then we are done.
// 		return mainCert;
// 	}


// 	//sorting the components
// 	int currentComp = first, lastVertex = -1, numberInList = 1;  //the one we are currently on and the last vertex we did a switch on - so that we can ensure it is ok.
// 	int compsSorted[num_comps];	//a mapping from the sorted order to the actual order.
// 	compsSorted[0] = first;  //assume that first is the fist we will use.
// 	lastVertex = containsCuts[first].front(); //last vertex is the sink of the last component.  Start with the 1.
// 	while (currentComp != last) { //while we have more to go.'
// 		//move to the next item based on the last sink.  It is the only other component with this cut vertex.
// 		if (cutPoints[lastVertex].front() != currentComp )
//             currentComp = cutPoints[lastVertex].front();  //if the first of 2 components related to this are new then we move forward
// 		else
//             currentComp = cutPoints[lastVertex].back();  //since it must be the other one.

// 		if (containsCuts[currentComp].front() != lastVertex ) containsCuts[currentComp].reverse();  //if the front vertex was the last sink - reverse it to the correct order.
// 		lastVertex = containsCuts[currentComp].back();  //if it was not the front - then it must be the back as the other one.


// 		compsSorted[numberInList] = currentComp;  //set this up as the next item.
// 		numberInList++;//increment.
// 	}

// 	//if we only have one component - then that certificate is all we need.
// 	if (num_comps == 1) {
// 		run(comp[0], mainCert, -1, -1 );
// 		if (!mainCert.confirmCert(g) ) cout << "NOT VALID\n";
// 		else cout << "VALID\n";
// 		return mainCert;
// 	}

// 	//from here we follow the algorithm almost exactly
// 	bool k4Found = false;
// 	bool k23Found = false;		//weather or not one has bean found
// 	bool sp = true;			//we assume it is unless we find otherwise.
// 	size_t k = 0; //start at 0 and move forward.
// 	int k4at = -1, k23at = -1; //the component we found the subgraph in.

// 	//go through all of the num_comps
// 	while (k < num_comps  ) {
// 		if (k == 0 || k == num_comps - 1)
//         {  //if this is the first or last
//             if (k == 0)
//             {
//                 run(comp[compsSorted[k]], certs[k], -1,
//                     containsCuts[compsSorted[k]].front()); // tells run we care about the sink
//             }
//             else {
//                 run(comp[compsSorted[k]], certs[k],
//                     containsCuts[compsSorted[k]].front(),
//                     -1); // tells run to start with root.
//             }

//             if (certs[k].type ==
//                 certs[k].K23GRAPH) { // automatically precludes a k4 as
//                 // a k4 replaces k23.
//                 k23Found = true;
//                 k23at = k;
//             } else if (certs[k].type == certs[k].K4GRAPH) {
//                 k4at = k;
//                 k4Found = true;
//                 sp = false;
//                 break; // since a k4 is found we stop searching.
//             }
//         }
//         else
//         {
//             run(comp[compsSorted[k]], certs[k],
//                 containsCuts[compsSorted[k]].front(),
//                 containsCuts[compsSorted[k]].back()); // we run the component with the needed
//             // graph.
//             if (certs[k].type ==
//                 certs[k].K23GRAPH) { // automatically precludes a k4 as
//                 // a k4 replaces k23.

//                 if (comp[compsSorted[k]].containsForcedEdge23 !=
//                     -1) { // if ( the K2,3-subdivision, ˜K2,3, returned by
//                     // Report(K2,3) contains (ci−1, ci)) - then we
//                     // must change it
//                     int min = comp[compsSorted[k]].numV +
//                               1; // higher then is possible - so we must get
//                     // lower.
//                     int choosen = -1; // the vertex we are going to choose
//                     // to replace it.
//                     int starting = containsCuts[compsSorted[k]].back();
//                     auto vertices =
//                             comp[compsSorted[k]]
//                             .vertices; // get all fo the vertices
//                     Vertex e = vertices[containsCuts[compsSorted[k]]
//                                         .back()]; // get the vertex.

//                     // change out the incorrect spot with the next closest
//                     // ear.  Find that ear.
//                     for (Node *u_node = e.adjacencyList.head->next;
//                          u_node != e.adjacencyList.tail;
//                          u_node = u_node->next) {
//                         int toTest = u_node->vertexNumber;
//                         if (!vertices[toTest]
//                             .k23visit) { // if the edge is not part of
//                             // the k23.
//                             if (vertices[toTest].ear->root->t != -1 &&
//                                 vertices[vertices[toTest].ear->root->t]
//                                 .dfsNumber < min) {
//                                 choosen = toTest;
//                                 min = vertices[vertices[toTest].ear->root->t]
//                                       .dfsNumber;
//                             }
//                         }
//                     }
//                     // we now have the minimum ear - based on the adjacency
//                     // list.

//                     certs[k]
//                             .certPaths[comp[compsSorted[k]]
//                                        .containsForcedEdge23]
//                             .pop_back(); // remvove the edge we are missing
//                     certs[k]
//                             .certPaths[comp[compsSorted[k]]
//                                        .containsForcedEdge23]
//                             .push_back(make_pair(
//                                 starting,
//                                 choosen)); // add the edge to get to the ear.

//                     // add the ear to the path. passing in the path we want.
//                     // we use 23 so that it confirms also.
//                     comp[compsSorted[k]].Check_Ear(
//                         vertices[choosen].ear->root,
//                         comp[compsSorted[k]].containsForcedEdge23,
//                         (comp[compsSorted[k]].endpoints), 23,
//                         certs[k].certPaths[comp[compsSorted[k]]
//                                            .containsForcedEdge23]);
//                 }
//                 k23Found = true; // since this was a k23 we found.
//                 k23at = k;
//             }
//             else if (certs[k].type ==
//                        certs[k].K4GRAPH)
//             { // if it is a k4 - we just
//                 // need to keep going.  We
//                 // remove the edge if needed
//                 // during certification.
//                 k4Found = true;
//                 k4at = k;
//                 sp = false;
//                 break;
//             }
//         }
//         k++;
// 	}


// 	if (k4Found) //since i must be an element
// 		if (comp[compsSorted[k]].containsForcedEdge4 != -1) { //if the edge was invovlved.
// 				k4at = -1;
// 				k4Found = false;
// 				sp = false;
// 				//replace the k4 subdivision
// 				certs[k].ignore = comp[compsSorted[k]].containsForcedEdge4;

// 				//go though the rest to test for oupterplanarity.
// 				while ((k < num_comps) && !k23Found ) { // continue to check outerplanarity
// 					run(comp[compsSorted[k]], certs[k], -1, -1);
// 					if (certs[k].type == certs[k].K23GRAPH ) {  //automatically precludes a k4 as a k4 replaces k23.
// 						k23Found = true;
// 						k23at = k;
// 					} else if (certs[k].type == certs[k].K4GRAPH ) {
// 						k4Found = true;
// 						k4at = k;
// 						break;
// 					}
// 				}
// 		}

// 	if (!k4Found) {
// 		if (sp) {
// 			//remve the edge if needed - as one of the edges may be faked to force the tree.
// 			if (comp[compsSorted[0]].addedForcedEdge && certs[0].constructionTree->root->l->comp == "EDGE")
//                 mainCert.constructionTree->root = certs[0].constructionTree->root->r;
// 			else if (comp[compsSorted[0]].addedForcedEdge )
//                 mainCert.constructionTree->root = certs[0].constructionTree->root->l;
// 			else
//                 mainCert.constructionTree = certs[0].constructionTree;
// 		}
// 		if (!k23Found)
//             mainCert.combinePaths(mainCert.boundary, certs[0].boundary);  //combine the boundary paths.

//         //do this again for the rest - now that we have more.
// 		for(size_t j = 1; j < num_comps; j++) {
// 			if (sp) {
// 				mainCert.SP = true;

// 				Tree *t;
//                 if (certs[j].constructionTree)
//                       t  = new Tree(mainCert.constructionTree->root->s, certs[j].constructionTree->root->t, "SC"); /* create the new tree */
//                 else
//                     continue; /* create the new tree */

//                 t->root->l = mainCert.constructionTree->root;				/* assign subtrees */

//                 if (comp[compsSorted[j]].addedForcedEdge && certs[j].constructionTree->root->l->comp == "EDGE")
//                     t->root->r = certs[j].constructionTree->root->r;
// 				else if (comp[compsSorted[j]].addedForcedEdge )
//                     t->root->r = certs[j].constructionTree->root->l;
// 				else
//                     t->root->r = certs[j].constructionTree->root;


// 				mainCert.constructionTree = t;
// 			}
// 			if (!k23Found)
//                 mainCert.combinePaths(mainCert.boundary, certs[j].boundary);
// 		}
// 	}

// 	//return the certificates.
// 	if (k4Found)
//     {
// 		certs[k4at].SP = false;
// 		certs[k4at].OP = false;
// 		if (!certs[k4at].confirmCert(g) )
//             cout << "NOT VALID1\n";
// 		else cout << "VALID\n";
// 		return certs[k4at];
//     }
//     else if (k23Found)
//     {
//         certs[k23at].OP = false;
//         certs[k23at].constructionTree = mainCert.constructionTree;
//         if (!certs[k23at].confirmCert(g))
//             cout << "NOT VALID2\n";
//         else
//             cout << "VALID\n";
//         return certs[k23at];
//     }
//     else
//     {
//         mainCert.type = mainCert.SP_AND_OP;
//         mainCert.OP = true;
//         if (!mainCert.confirmCert(g))
//             cout << "NOT VALID3\n";
//         else
//             cout << "VALID\n";

//         return mainCert;
//         // confirm the tree
//     }
// }


// //the driver of this system.
// int main(int argc, char* argv[]) {


// 	int size, w, v;
// 	string answer;
// 	string line;

// 	//check that we have the correct number of arguments.
// 	if(argc != 2){
// 		cout<<"Usage: "<<argv[0]<<" input_filename "<<endl;
// 		return 0;
// 	}
// 	// cout<<"Input File: "<<argv[1]<<endl;
// 	// cout<<"Is this correct?[yes/no]"<<endl;
// 	// cin>>answer;
// 	// if(!(answer[0] == 'Y' || answer[0] == 'y')){
// 	// 	cout<<"Exiting"<<endl;
// 	// 	return 0;
// 	// }

// 	//try and open the input file
// 	ifstream infile(argv[1]);
// 	if(!(infile.is_open())){
// 		cout<<"Can't open file "<<argv[1]<<endl;
// 		cout<<"Exiting"<<endl;
// 		return 0;
// 	}

// 	getline(infile,line);  //read a single line
// 	istringstream iss1(line);  //a stream to represent a line.

// 	iss1 >> size;	// read in the size of the graph
// 	graph_t g2(size);  //the boost graph to decompose with


// 	while(getline(infile,line)) {	// read in the edges of the graph
// 		w = size;  //we set them both to size - if one of them remains as size then something has gone wrong when reading it in.
// 		v = size;
// 		istringstream iss2(line); //read in one line
// 		iss2 >> w >> v;  			//read in the 2 item
//         if(w != size && v != size){ //as long as both items have changed we are good.
// 			add_edge(w, v, g2);  //add them to the graph
// 		}
// 	}

// 	infile.close();


// 	cout << certSPandOP(g2,size) << endl;  //run the main function and output the certificate.
// }
#endif
