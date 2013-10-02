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

	// O(n) :-( can we do better? XXX
	for (int i=0; i < v1_list.size(); i++) {
		if (v1_list[i] == v2) return 1;
	}

	return 0;
}

int tricode(int u, int v, int w) {
	int i = 0;

	// Since we arent directed, we count edges as both ways?
	
	i += (link(u, v)) ? 3 : 0; 
	i += (link(w, v)) ? 12 : 0;
	i += (link(u, w)) ? 48 : 0;

	if (i == 3 || i == 12 || i == 48) return 0; 
	if (i == 15 || i == 51 || i == 60) return 2;
	if (i == 63) return 3;

	// 0-63
	return i;
}


int main ()
{
    fstream inputFile;
    string line;

    map<int, vector <int> > links;
    int triadCounts[4] = {0, 0, 0, 0}, total = 0;
    int ret=0;
    int count = 0; // counts number of nodes

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
            //assign neighbors to map with node as key
            list[key] = nodes;
            count++;
        }

        inputFile.close();

    }

    map<int, vector <int> >::iterator it;

    for (it=list.begin(); it!=list.end(); ++it)
    {
	   // row u
        int key = it -> first; 
        cout << "Node: " << key << endl;
        cout << "Neighbors: ";
        vector<int> nodes = it -> second;
        sort(nodes.begin(), nodes.end());
	    // for each neighbor v
        for (int i = 0; i < nodes.size(); i++)
            {
                cout << nodes[i] << " ";
                if(key < nodes[i])
                {
                    vector<int> S (nodes);
                    vector<int> nodes2 = list[nodes[i]];
                    sort(nodes2.begin(), nodes2.end());
                    vector<int> :: iterator it2;

                    S.insert(S.end(), nodes2.begin(), nodes2.end() );
                    sort(S.begin(), S.end());
                    unique(S.begin(), S.end());
                    //vector<int> :: iterator it3 = S.begin();
                    //it2 = set_union(nodes.begin(), nodes.end(), nodes2.begin(), nodes2.end(), S.begin() );
                    //S.resize( it2-S.begin() );

                   

                    for(it2 = S.begin(); it2!= S.end(); it2++)
                    {
                            //cout << ' ' << *it2;
                    }
                    cout << endl;
                    
                    // add list of nodes from v, get rid of duplicates

                    triadCounts[1] += count- S.size() -2;
            		// XXX how to determine tricode type for count?
            			
        	        //add the neighbors of key and neighbors of node[i] into new vector
        			
        			for (int j=0; j < S.size(); j++) 
                    {
        				if (key < S[j]) 
                        {
        					ret = tricode(key, nodes[i], S[j]);
        					triadCounts[ret]++;
        				}
        			}
            	
                    //do the other stuff in the pseudocode below

                }
        }

    }

    cout << endl << "Triad Counts: " << endl;
    for (int i = 0; i < 4; i++) 
    {
 		total += triadCounts[i];		
    }

    cout << endl << "Triad Counts: " << endl;
    for (int i = 0; i < 4; i++) 
    {
        cout << i << ": " << triadCounts[i] << endl;
    }

    triadCounts[0] = 1/6 * count * (count -1) * (count -2) - total;
    cout << "Total: " << total << endl;

    cout << endl;


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
                    // if ( u < w || (v <w && w < u &&  v is not in the neighbor list of w)) // might have to modify this, because we are doing undirected graph, so we don't want to check different configurations, only one. maybe just u <w
                            //Type = Tricode (v,u,w);
                            // counts[Type] ++

//int total = 0;
// for(int x =0; x < 4; x++)
//      total += counts[x];

//Counts[0] = 1/6(n) (n-1) (n-2) - sum;


//Tricode(v, u, w)
// Figure out if they are a 2 or 3 connection triad

