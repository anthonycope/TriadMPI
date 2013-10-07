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
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "mpi.h"

using namespace std;

//key is the node, value is the vector of neighbors
map<int, vector <int> > list;


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

int link(int v1, int v2) {
	vector<int> v1_list = list[v1];
	vector<int> v2_list = list[v2];

	if (binary_search(v1_list.begin(), v1_list.end(), v2)) return 1;

	return 0;
}

int tricode(int u, int v, int w) {
//	vector<int> ret(2);
	int i=0;

/*
	i += (link(u, v)) ? 3 : 0; 
	i += (link(w, v)) ? 12 : 0;
	i += (link(u, w)) ? 48 : 0;
*/

	i += (link(v, u)) ? 1 : 0;
	i += (link(u, v)) ? 2 : 0;

	i += (link(v, w)) ? 4 : 0;
	i += (link(w, v)) ? 8 : 0;

	i += (link(u, w)) ? 16 : 0;
	i += (link(w, u)) ? 32 : 0;

	// All 1-edge cases
	if (i == 3 || i == 1 || i == 2 || 
	    i == 4 || i == 8 || i == 12 || i == 48
	   || i == 16 || i == 32) return 2;

	// All 2-edge cases
 	if (i == 5 || i == 9 || i == 17 ||
	    i == 33 || i == 6 || i == 10 || i == 18 ||
	    i == 34 || i == 20 || i == 36 ||
	    i == 24 || i == 40 || i == 51 || 
	    i == 15) return 2;

	if (i == 63) return 3;

	// 0-63
	return i;

}


int main (int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int mapSize, numProcessors, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    fstream inputFile;
    string line;

    long double triadCounts[4] = {0, 0, 0, 0}, total = 0;
    int ret=0;
    int local1=0, local2=0;
    int count=0;

   
    if (rank == 0) {
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
	           //remove the first node from list of neighbors
	            nodes.erase(nodes.begin());
	
		    sort(nodes.begin(), nodes.end());
	
	            //assign neighbors to map with node as key
	            list[key] = nodes;
		    count += nodes.size() + 1;
	        }
	
	        inputFile.close();
	
	    }

	    mapSize = list.size();
	}

    MPI_Bcast(&mapSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
    for (int i=0; i < mapSize; i++)  
    {
	int rowSize, key;
	vector<int> row;

	if (rank == 0) {
		row = list[i];
		rowSize = row.size();
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&rowSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int rowArray[rowSize];

	if (rank == 0) {
		vector<int> vectorRow = list[i];
		for (int x=0; x < rowSize; x++) {
			rowArray[x] = vectorRow[x];
		}
		key = i;
	}


        MPI_Bcast(&rowArray, rowSize, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

       //links[i].assign(row, row.size());
        if(rank != 0)
        {
            vector<int> vectorRow;
             for( int x = 0; x < rowSize; x++)
            {
               vectorRow.push_back(rowArray[x]);
            }          
           //cout << "Key: " << key << " | ";
           list[key] = vectorRow;

           vector <int> testRow = list[key];/*
           for( int j = 0; j < rowSize; j++)
           {
                //vectorRow = links[i];
                cout << testRow[j] << " ";
            }
            cout << endl;*/
        }

    }
		

    map<int, vector <int> >::iterator it;

   // had to modify loop so that each CPU would work on its own rows, and wouldn't overlap the others
    for(int i = rank; i < mapSize; i+=numProcessors)
    {
        int u = i;
        vector<int> nodes = list[i];    
	// for each neighbor v
        for (int x = 0; x < nodes.size(); x++)
            {           
                int v = nodes[x];                 
                if(u < v) {
                    vector <int> vNodes = list[v];
                    
                    //if(std::count(S.begin(), S.end(), key ) == 1)
                      //  triadCounts[1] += 1; // every neighbor is 1 connection because it's undirected
                   
                    vector<int> S (nodes);
                    vector<int> nodes2 = list[nodes[x]];
                    sort(nodes2.begin(), nodes2.end());

                    // S is the union of the node and its neighbors
                    S.insert(S.end(), nodes2.begin(), nodes2.end() );
                    sort(S.begin(), S.end());
                    unique(S.begin(), S.end());
                    
		    if (link(u, nodes[i]) && link(nodes[i], u))
			triadCounts[1] += list.size() - S.size() - 2;
		    else triadCounts[2] += list.size() - S.size() - 2;
   		    // XXX remove u,v

		    local1 = 0;
      		    for (int y=0; y < vNodes.size(); y++) 
                    {
                        int w = vNodes[y];     
                        triadCounts[2] +=1; // if its in nodes, it's already connected once.  every neighbor of this neighbor then forms a 2 connection triad.                   
			if (nodes[i] < w || 
			    (u < w && w < nodes[i] && !link(w, u)) ) {
                        {           
				ret = tricode(u, nodes[i], w);

				triadCounts[ret]++;
				local1++;
			}
	   	    }

                   if (S.size() - local1 > 0) triadCounts[2] += S.size() - local1;
                   else if (S.size() <= 0) triadCounts[1]++;
                            
                }   
                //cout << endl;
        }
    }
   }

   if(rank != 0)
    {
        MPI_Send(&triadCounts,4,MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD);
        //cout << "SENT" << endl;
    }
    if(rank == 0)
    {
        int count = 0;
        long double counts[4] = {0,0,0,0};
        
        while(count < numProcessors -1) // so all processes return
        {
            MPI_Recv(&counts, 4, MPI_LONG_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int x = 0; x < 4; x++)
            {
                triadCounts[x] += counts[x];
            }
            count++;
        }

        //cout << "HERE" << endl;             

        for (int i = 1; i < 4; i++) 
        {
     		total += triadCounts[i];		
        }

    }
   
    if (rank == 0) {
        //long double numZero =  (long double) 12477843 * (long double)(12477843 -1) * (long double)(12477843 -2)/6.0;
        long double numZero =  (long double) mapSize * (long double)(mapSize -1) * (long double) (mapSize -2)/6.0;
        cout.setf(ios::fixed);            
        cout <<  "NumZero: " << setprecision(0) << numZero << endl;

        numZero-= total;
        triadCounts[0] = (long double) numZero;
        total += triadCounts[0];

        cout << endl << "Triad Counts: " << endl;

        for (int i = 0; i < 4; i++) 
        {
            cout.setf(ios::fixed);
            cout << setprecision(0) << i << ": " << triadCounts[i] << endl;
        }


        cout << "Count: " << mapSize << endl;
        cout << "Total: " << total << endl;

        cout << endl;
    }

    MPI_Finalize();

    return 0;
}


    // n! / r!(n-r)!
    // or 1000! / 3! * 997! = 1000*999*998 / 6
    // 166167000


// int n = num nodes;
//intialize an empty array to hold count for each type of triadmpi
// counts[4];
//take first element of row (u)
    //for each "neighbor" v of that row
        // if u is less than v
                //S = all neighbors of first element, all neighbors of the selected neighbor (have to find them)
                //add counts[1] += counts[1] + n - |S| -2
                //for each element w in S
                    // if ( u < w || (v <w && w < u &&  v is not in the neighbor list of w)) // might have to modify this, because we are doing undirected graph, so we don't want to check different configurations, only one. maybe just u <w
                            //Type = Tricode (v,u,w);
                            // counts[Type] ++

//int total = 0;
// for(int x =0; x < 4; x++)
//      total += counts[x];

//Counts[0] = 1/6(n) (n-1) (n-2) - sum;


//Tricode(v, u, w)
// Figure out if they are a 2 or 3 connection triad

