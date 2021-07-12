#include "AdjacencyList.hpp"
#include "Graph.hpp"
#include "Certificate.hpp"
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/config.hpp>
#include <list>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iterator>
#include <iostream>


#ifndef SPANDOP_H
#define SPANDOP_H
using namespace std;
//the output file

//the boost objects we need
namespace boost
{
  struct edge_component_t
  {
    enum
    { num = 555 };
    typedef edge_property_tag kind;
  };
      //edge_component;
}

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
		boost::no_property,boost:: property <boost:: edge_component_t, std::size_t > >graph_t;
typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_t;
using Tree_ptr = std::shared_ptr<Tree>;


void run (Graph &g, Certificate &cert, int source, int second,std::map<int,Tree_ptr>& cut_point_tree_map);  //a function to run the algorithim on a biconnected component.
Certificate runSPandOP (graph_t graph,int size);  //a function to run on a boost graph which returns the entire certificate
string certSPandOP(graph_t graph,int size);			//returns the string of the certificate.
bool isOP(graph_t graph,int size);					//returns a boolean as to weather or not the graph is outerplaner
bool isSP(graph_t graph,int size);					//returns a boolean as to weather or not the graph is series parallel
#endif
