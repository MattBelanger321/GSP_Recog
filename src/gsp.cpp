#include "gsp.hpp"

typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS, boost::no_property,
    boost::property<boost::edge_component_t, std::size_t>>
graph_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
typedef boost::graph_traits<graph_t>::adjacency_iterator AdjacencyIterator;
using Tree_ptr = std::shared_ptr<Tree>;

//Update k4graphFound and k23graphfound flags
void update_graphs_found(const Certificate &cert, bool & k4, bool & k23) {
    switch (cert.type)
    {
        case 3:
            k4 = true;
            break;
        case 4:
            k23 = true;

    }
}

/// return a pair of (first, second)
/// first = index of components in correct order
/// second = index of cutpoints in correct order
auto sort_compoentents( map<int, list<int>> &containsCuts,
                        map<int, list<int>> &cutPoints,
                        const int first,
                        const int last,
                        const size_t num_comps)
{
    // sorting the components
    int currentComp = first, lastVertex = -1,
       numberInList = 1; // the one we are currently on and the last vertex we
    // did a switch on - so that we can ensure it is ok.
    vector<int> compsSorted(num_comps); // a mapping from the sorted order to the actual
    vector<int> cut_points_sorted(num_comps);


    compsSorted[0] = first;         // assume that first is the fist we will use.
    lastVertex =containsCuts[first].front(); // last vertex is the sink of the last
    cut_points_sorted[0] =  lastVertex;
    // component.    Start with the 1.
    while (currentComp != last)
    { // while we have more to go.'
        // move to the next item based on the last sink.    It is the only other
        // component with this cut vertex.
        if (cutPoints[lastVertex].front() != currentComp)
            currentComp = cutPoints[lastVertex]
                          .front(); // if the first of 2 components related
        // to this are new then we move forward
        else
            currentComp = cutPoints[lastVertex].back(); // since it must be the other one.

        if (containsCuts[currentComp].front() != lastVertex)
            containsCuts[currentComp].reverse(); // if the front vertex was the
        // last sink - reverse it to
        // the correct order.
        lastVertex = containsCuts[currentComp]
                     .back(); // if it was not the front - then it
        // must be the back as the other one.

        compsSorted[numberInList] =
                currentComp; // set this up as the next item.
        cut_points_sorted[numberInList]=lastVertex;
        numberInList++;  // increment.
    }
    return std::make_pair(compsSorted, cut_points_sorted);
}

Certificate GSP_verify(graph_t &g2, const size_t size) {

    /*******************************************************************************
     * Setup/Init for main GSP algorithm
     *******************************************************************************/
    Certificate mainCert; // the certificate object for the entire graph.

    bool sort_comps = true;

    Graph g(size); // one of our own graphs - used to confirm the output - whcih
    // will be the full graph.

    // needed for the boost graph.
    boost::property_map<graph_t, boost::edge_component_t>::type component = get(boost::edge_component_t(), g2);

    // articulation points vector
    std::vector<vertex_t> art_points;

    // get back the number of components.
    auto comp_size_iter_pair =
            biconnected_components(g2, component, std::back_inserter(art_points));
    const size_t num_comps = comp_size_iter_pair.first;

    // create a internal graph object and certificate for each component to later use.

    std::vector<Graph> comp;
    for (size_t i=0;i<num_comps;i++)
    {
        comp.emplace_back(size);
    }
    std::vector<Certificate> certs(num_comps);
    std::cerr << "Found " << num_comps << " biconnected components.\n";

    //vertex ->list of biconnected comps
    // cutPoints maps the articulation points to the biconnected components they are a part of.
    map<int, list<int>> cutPoints;

    // biconnected comp -> list of cut vertices
    // contains cuts maps the biconnected components to the articulation points inside of them.
    map<int, list<int>>  containsCuts;


    unordered_map<int, bool>isCutPoint; // isCutPoint is set to true if the vertex is an articulation point.

    // move the list of cut points into the hash map
    for (std::size_t i = 0; i < art_points.size(); ++i) {
        isCutPoint[art_points[i]] = true;
    }
    //Set map in main get object
    g.isCutPoint = isCutPoint;

    boost::graph_traits<graph_t>::edge_iterator ei,  ei_end; // set up an iterator to go through all edges.
    //
    for (boost::tie(ei, ei_end) = edges(g2); ei != ei_end; ++ei) {

        comp[component[*ei]].addEdge(source(*ei, g2), target(*ei, g2));
        // add the edge to the component it is a part of.
        g.addEdge(
            source(*ei, g2),
            target(*ei, g2)); // add the edge to the full graph we are also creating.

        // for both vertexes, add the details to the mapping for later.
        if (isCutPoint[source(*ei, g2)]) {
            cutPoints[source(*ei, g2)].push_front(component[*ei]);
            containsCuts[component[*ei]].push_front(source(*ei, g2));
        }
        if (isCutPoint[target(*ei, g2)]) {
            cutPoints[target(*ei, g2)].push_front(component[*ei]);
            containsCuts[component[*ei]].push_front(target(*ei, g2));
        }
    }

    // go through the articulation points again.
    for (std::size_t i = 0; i < art_points.size(); ++i)
    {
        cutPoints[art_points[i]].sort();     // sort the attached list.
        cutPoints[art_points[i]].unique(); // remove duplicates - for this to work
        // the list must be sorted.
        comp[i].isCutPoint = isCutPoint;
        if (cutPoints[art_points[i]].size() >= 3)
        { // if it fails the max of 2 test - set up the certificate

            mainCert.type = mainCert.VERTEX_IN_THREE_BICONNECTED;
            mainCert.SP =false;
            mainCert.single = art_points[i];
            int number = cutPoints[art_points[i]].size();
            for (int kl = 0; kl < number; kl++)
            { // move the data into the certificate.
                mainCert.others.push_back(cutPoints[art_points[i]].front());
                cutPoints[art_points[i]].pop_front();
            }
            sort_comps=false;
            break;
        }
    }
    int first = -1, last = -1; // there can only be 2 components with 1 in each if
    // it is SP.    The first and last in the chain.

    // run through all of the components.
    for (std::size_t i = 0; i < num_comps; ++i)
    {
        int size;

        containsCuts[i].sort();     // sort out the list.
        containsCuts[i].unique(); // remove duplicates - needs to be sorted to work.
        if ((size = containsCuts[i].size()) == 1)
        {
            // if there is only one articulation points then it is either first
            // or last.
            if (first == -1)
                first = i; // if first is already choosen then it must be last.
            else
                last = i;
        }
        else if (size >= 3)
        { // if it fails set up the certificate - as there is a
            // max of 2
            mainCert.SP=false;
            mainCert.type = mainCert.THREE_VERTEX_IN_BICONNECTED;
            mainCert.single = i;
            int number = containsCuts[i].size();
            for (int kl = 0; kl < number; kl++) {
                mainCert.others.push_back(containsCuts[i].front());
                containsCuts[i].pop_front();
            }
            sort_comps=false;
            break;
        }
    }

    // sorting the components
    std::vector<int> compsSorted(num_comps); // a mapping from the sorted order to the actual
    std::vector<int> cut_points_sorted(num_comps);
    std::generate(compsSorted.begin(), compsSorted.end(), [n = 0] () mutable { return n++; });
    std::generate(cut_points_sorted.begin(), cut_points_sorted.end(), [n=0,  containsCuts]  () mutable { return containsCuts[n++].front(); });


    if (sort_comps && num_comps >1)
    {
        std::tie(compsSorted, cut_points_sorted) = sort_compoentents(containsCuts, cutPoints, first, last, num_comps);
    }

    std::vector<Certificate> component_certs(num_comps);//certification for each component

    std::map<int, Tree_ptr>  comp_trees; //map of cut_point ci and it corresponding Tree
    for(size_t i=0; i< num_comps; i++)
    {
        comp_trees[cut_points_sorted[i]] = nullptr;
    }


    //bools
    bool k4found = false;
    bool k23found = false;
    size_t k23comp = num_comps;//component which contains the k23 graph if it exist
    size_t thethaComp = num_comps;

    /*******************************************************************************
     * Main algorithm starts here, follows the psudo code on page 32
     *******************************************************************************/
    size_t i =0;
    for(; i < num_comps; ++i)
    {
        auto &Bi = comp[compsSorted[i]];
        const auto ci = cut_points_sorted[i];
        const auto ci_1 = i>0?cut_points_sorted[i-1]:ci;

        if (mainCert.SP == false)
        {
            //auto x = Bi.vertices[ci].adjacencyList.head->next->vertexNumber;
            //Bi.addEdge(ci, x);
            run(Bi, component_certs[i], ci, -1, comp_trees);
            update_graphs_found(component_certs[i], k4found, k23found);
        }
        else
        {
            if(i==0)//if at 1st component
            {
                //auto x = Bi.vertices[ci].adjacencyList.head->next->vertexNumber;
                //Bi.addEdge(ci, x);;
                run(Bi, component_certs[i], ci, -1, comp_trees);
                update_graphs_found(component_certs[i], k4found, k23found);
            }
            else if(i==num_comps-1) //last compoenent
            {
                //auto x = Bi.vertices[ci_1].adjacencyList.head->next->vertexNumber;
                //Bi.addEdge( x, ci_1);;
                run(Bi, component_certs[i], -1, ci_1, comp_trees);
                update_graphs_found(component_certs[i], k4found, k23found);
            }
            else //middle components
            {
                //add ci and ci-1 if does not exist
                auto added_edge=false;
                if(!Bi.Verify_Edge(ci, ci_1))
                {
                    Bi.addEdge( ci, ci_1);
                    added_edge=true;
                }
                run(Bi, component_certs[i], ci, ci_1, comp_trees);
                update_graphs_found(component_certs[i], k4found, k23found);
                //we remove the added edge
                if(added_edge)
                {
                    Bi.Check_Edge_remove(ci,ci_1);
                }
            }
        }
        //
        //if k23 found and ~k4 found
        if (k23found && !k4found)
        {
            k23comp = i;
            if(contains_pair(component_certs[i].certPaths, ci, ci_1))
            {
                if(!g.Verify_Edge(ci, ci_1))
                {
                    //TODO implement this
                    // replace (ci, ci1) with pear(e) where (ci, e ) is not in k23
                }
            }
        }
        if(k4found)
        {
            //check for thetha graph
            if(i!=0 &&
               i!=num_comps-1 &&
               mainCert.SP)
            {
                if(contains_pair(component_certs[i].certPaths, ci, ci_1)
                   && !Bi.Verify_Edge(ci, ci_1))
                {
                    //setup thetha graph
                    remove_pair(component_certs[i].certPaths, ci, ci_1);
                    mainCert.SP=false;
                    k4found = false;
                    thethaComp = i;
                    --i;
                }
            }
        }
        //update composition trees
        else if (std::find(art_points.begin(), art_points.end(), ci)!=art_points.end()
                 && comp_trees[ci]!=nullptr)
        {
            comp_trees[ci] = DC(comp_trees[ci], component_certs[i].constructionTree);
        }
        else
        {
            comp_trees[ci] = component_certs[i].constructionTree;
        }
        if(k4found)
        {
            break;
        }
    }
    //update main cert tree
    mainCert.constructionTree = i<num_comps? comp_trees[cut_points_sorted[i]]: comp_trees[cut_points_sorted[i-1]];

    //update exterior boundary
    if(!(k23found || k4found))
    {
        for(auto& cert_temp: component_certs)
        {
            mainCert.combinePaths(mainCert.boundary, cert_temp.boundary);
        }
    }

    //update main cert
    if(k4found) // !GSP, !OP !SP
    {
        mainCert.GSP=false;
        mainCert.OP=false;
        mainCert.SP=false;
        mainCert.type = mainCert.K4GRAPH;
        mainCert.certPaths = component_certs[i].certPaths;
    }
    else if (mainCert.SP && !k23found) //GSP OP SP
    {
        return mainCert;
    }
    else if (mainCert.SP && k23found) //GSP, !OP, and SP
    {
        mainCert.OP = false;
        mainCert.certPaths = component_certs[k23comp].certPaths;
        mainCert.type= mainCert.K23GRAPH;
    }
    else if(!mainCert.SP && !k23found) //GSP, OP, ! SP
    {
        if (mainCert.type != mainCert.VERTEX_IN_THREE_BICONNECTED  &&
            mainCert.type != mainCert.THREE_VERTEX_IN_BICONNECTED)
        {

            mainCert.type= mainCert.THETHA_GRAPH;
            mainCert.thetha_paths = component_certs[thethaComp].certPaths;
        }
    }
    else //GSP !OP !SP
    {
        if (mainCert.type != mainCert.VERTEX_IN_THREE_BICONNECTED  &&
            mainCert.type != mainCert.THREE_VERTEX_IN_BICONNECTED)
        {

            mainCert.type= mainCert.THETHA_GRAPH;
            mainCert.thetha_paths = component_certs[thethaComp].certPaths;
            mainCert.certPaths = component_certs[k23comp].certPaths;
        }
    }


    return mainCert;
}
