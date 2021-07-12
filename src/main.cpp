#include "gsp.hpp"
#include <iostream>
#include <fstream>
#

using namespace std;
//the driver of this system.
int main(int argc, char* argv[]) {


	int size, w, v;
	string answer;

	//check that we have the correct number of arguments.
	if(argc != 2){
		cout<<"Usage: "<<argv[0]<<" input_filename "<<endl;
		return 0;
	}

	//try and open the input file
	ifstream infile(argv[1]);
	if(!(infile.is_open())){
		cout<<"Can't open file "<<argv[1]<<endl;
		cout<<"Exiting"<<endl;
		return 0;
	}

	//getline(infile,line);  //read a single line
    //	istringstream iss1(line);  //a stream to represent a line.

	infile >> size;	// read in the size of the graph
	graph_t g2(size);  //the boost graph to decompose with

	while(infile >> w >>v) {	// read in the edges of the graph
        if(w != size && v != size){ //as long as both items have changed we are good.
			add_edge(w, v, g2);  //add them to the graph
		}
	}

	infile.close();


	cout << GSP_verify(g2,size).print() << endl;  //run the main function and output the certificate.
}
