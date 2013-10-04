//MPI Triad Census Implementation
//Anthony Cope and Joseph Fields

// On RC, use module add compilers/gcc/4.4.7
//             module add mpi/openmpi/1.6.1
// mpic++ -o triadmpi triadmpi.cpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "mpi.h"

using namespace std;

//key is the node, value is the vector of neighbors
map<int, vector <int> > links;


//function from: http://www.openframeworks.cc/files/005-rc/005-rc2-xcode-for-zach/addons/ofXmlSettings/src/ofXmlSettings.cpp
vector<string> tokenize(const string & str, const string & delim)
{
  vector<string> tokens;
  size_t p0 = 0, p1 = string::npos;

  while(p0 != string::npos)
  {
    p1 = str.find_first_of(delim, p0);
    if(p1 != p0)
    {
      string token = str.substr(p0, p1 - p0);
      tokens.push_back(token);
    }
    p0 = str.find_first_not_of(delim, p1);
 }
return tokens;

}

// parses the data from one line of input
vector <int>  getInput(string line)
{
    stringstream ss(line); 
    string num;
    string temp;
    int i;

    getline(ss, num, ' ');

    vector<string> stringNodes = tokenize(num, "\t");
    vector<int> intNodes;            

    for (int i = 0; i < stringNodes.size(); i++)
    {
        intNodes.push_back(atoi(stringNodes[i].c_str()));
    }

    return intNodes;
}


int main (int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    fstream inputFile;
    string line;

    int numProcessors;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, & rank);



    map<int, vector<int> > links;
    int triadCounts[4] = {0, 0, 0, 0}, total = 0;
    int ret=0;
    double count = 0; // counts number of nodes

    if(rank == 0)
    {
        inputFile.open("testinput.egonets");
        if(inputFile.is_open())
        {
            while(!inputFile.eof())
            {
                // get the line from the file
                getline(inputFile, line);
                // get the nodes on that line
                vector <int> nodes = getInput(line);
                // get the first node
                int key = nodes[0];
                //remove the first node from links of neighbors
                nodes.erase(nodes.begin());
                //assign neighbors to map with node as key
                links[key] = nodes;
            }

            inputFile.close();
        }
    }

    count = links.size();
    for(int i = 0; i < links.size(); i++)
    {

        int size;
        int key;       
        vector <int> row; 

        if(rank == 0)
        {   

            row = links[i];
            size = row.size();
            //int[size] row;
            //for( int x = 0; x < size; x++)
            //{
             //   row[x] = vectorRow[x];
            //}           
           //row = &vectorRow[0];           
           key = i;
        }
       //int 
       MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
       MPI_Bcast(&row, size, MPI_INT, 0, MPI_COMM_WORLD);
       MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
       //links[i].assign(row, row.size());
       if(rank != 0)
       {
           cout << "Key: " << key << " | ";
           for( int j = 0; j < size; j++)
           {
                //vectorRow = links[i];
                cout << row[j] << " ";;
                }
                cout << endl;
            }
        }
    MPI_Finalize();

    return 0;

    //cout << "Rank : " << rank << " Size: " << links.size() << endl;



    /*

    map<int, vector <int> >::iterator it;

    for (it=links.begin(); it!=links.end(); ++it)
    {
	   // row u
        int u = it -> first; 
        //cout << endl << "Node: " << u << endl;
        //cout << "Neighbors: ";
        vector<int> nodes = it -> second;
        sort(nodes.begin(), nodes.end());
	    // for each neighbor v
        for (int x = 0; x < nodes.size(); x++)
            {           
                int v = nodes[x];                 
                if(u < v)
                {
                    //cout << v << " "; 
                    vector <int> vNodes = links[v];
                    
                    //if(std::count(S.begin(), S.end(), key ) == 1)
                      //  triadCounts[1] += 1; // every neighbor is 1 connection because it's undirected
                   
                    vector<int> S (nodes);
                    
                    vector<int> nodes2 = links[nodes[x]];
                    sort(nodes2.begin(), nodes2.end());
                    vector<int> :: iterator it2;


                    // S is the union of the node and its neighbors
                    S.insert(S.end(), nodes2.begin(), nodes2.end() );
                    sort(S.begin(), S.end());
                    unique(S.begin(), S.end());
                    
                    triadCounts[1] += count- S.size();            		
        			
        			for (int y=0; y < vNodes.size(); y++) 
                    {
                        int w = vNodes[y];     
                        triadCounts[2] +=1; // if its in nodes, it's already connected once.  every neighbor of this neighbor then forms a 2 connection triad.                   
        				if (u < w) 
                        {           
                            ///this one is only off by a little bit                 
                            
                            if ( v < w)
                            {
                                if(std::count(nodes.begin(), nodes.end(), vNodes[y] ) == 1) // if its in nodes, it's connected to original node, and if it's in S, it's a neighbor of the neighbor node, make it a 3 way triad.
                                {
                                    triadCounts[3] +=1; // this comes out to 2X the correct answer
                                }   
                            }
        				}
        			}                                	
                    

                }   
                //cout << endl;
        }
    }

    if(rank != 0)
    {
        MPI_Send(&triadCounts,4,MPI_DOUBLE, 
    }

        

    for (int i = 0; i < 4; i++) 
    {
 		total += triadCounts[i];		
    }

    double numZero = (double) ( count * (count -1) * (count -2))/6.0 - total;
    triadCounts[0] = (int) numZero;
    total += triadCounts[0];
    cout << endl << "Triad Counts: " << endl;
    for (int i = 0; i < 4; i++) 
    {
        cout << i << ": " << triadCounts[i] << endl;
    }

    cout << "Count: " << count << endl;

    cout << "Total: " << total << endl;

    cout << endl;
    */


}


// int n = num nodes;
//intialize an empty array to hold count for each type of triadmpi
// counts[4];
//take first element of row (u)
    //for each "neighbor" v of that row
        // if u is less than v
                //S = all neighbors of first element, all neighbors of the selected neighbor (have to find them)
                //add counts[1] += counts[1] + n - |S| -2
                //for each element w in S
                    // if ( u < w || (v <w && w < u &&  v is not in the neighbor links of w)) // might have to modify this, because we are doing undirected graph, so we don't want to check different configurations, only one. maybe just u <w
                            //Type = Tricode (v,u,w);
                            // counts[Type] ++

//int total = 0;
// for(int x =0; x < 4; x++)
//      total += counts[x];

//Counts[0] = 1/6(n) (n-1) (n-2) - sum;


//Tricode(v, u, w)
// Figure out if they are a 2 or 3 connection triad

