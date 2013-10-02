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

	if (binary_search(v1_list.begin(), v1_list.end(), v2)) return 1;

	return 0;
}

int tricode(int u, int v, int w) {
//	vector<int> ret(2);
	int i=0;

	i += (link(u, v)) ? 3 : 0; 
	i += (link(w, v)) ? 12 : 0;
	i += (link(u, w)) ? 48 : 0;

	if (i == 3 || i == 12 || i == 48) return 1; 
	if (i == 15 || i == 51 || i == 60) return 2;
	if (i == 63) return 3;

	// 0-63
	return i;

/*	
	if (link(u,v)) {
		ret[1]++;
		ret[0] += 1;
	}

	if (link(v,u)) {
		ret[1]++;
		ret[0] += 2;
	}

	if (link(w,v)) {
		ret[1]++;
		ret[0] += 6;
	}

	if (link(v,w)) {
		ret[1]++;
		ret[0] += 12;
	}

	if (link(u,w)) {
		ret[1]++;
		ret[0] += 32;
	}

	if (link(w,u)) {
		ret[1]++;
		ret[0] += 48;
	}
*/

/*
	ret[0] += (link(u, v)) ? 1 : 0;
	ret[0] += (link(v, u)) ? 2 : 0;
 
	ret[0] += (link(w, v)) ? 6 : 0;
	ret[0] += (link(v, w)) ? 12 : 0;

	ret[0] += (link(u, w)) ? 32 : 0;
	ret[0] += (link(w, u)) ? 48 : 0;
*/

//	return ret;
}


int main ()
{
    fstream inputFile;
    string line;

    map<int, vector <int> > links;
    int triadCounts[4] = {0, 0, 0, 0}, total = 0;
    int ret=0;
    int local1=0, local2=0;
    int count=0;

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

    map<int, vector <int> >::iterator it;

    for (it=list.begin(); it!=list.end(); ++it)
    {
	// row u
        int key = it -> first; 
/*
        cout << "Node: " << key << endl;
        cout << "Neighbors: ";
*/
        vector<int> nodes = it -> second;

	// for each neighbor v
        for (int i = 0; i < nodes.size(); i++)
       	    {
//                cout << nodes[i] << " ";

		if (key < nodes[i]) {
			
                    vector<int> S (nodes);
                    vector<int> nodes2 = list[nodes[i]];
                    vector<int> :: iterator it2;

                    S.insert(S.end(), nodes2.begin(), nodes2.end() );
                    sort(S.begin(), S.end());
                    unique(S.begin(), S.end());
		    //triadCounts[1] += (nodes.size()+nodes2.size()) - S.size();
		    local1 = 0;
	
		    for (int j=0; j < S.size(); j++) {
				if (key < S[j]) continue;

				ret = tricode(key, nodes[i], S[j]);

				triadCounts[ret]++;
				local1++;
	   	    }

	            if (S.size() - local1 > 0) triadCounts[1] += S.size() - local1;
		    else if (S.size() <= 0) triadCounts[1]++;

		}
        }


    }

    total = triadCounts[1] + triadCounts[2] + triadCounts[3];

    // n! / r!(n-r)!
    // or 1000! / 3! * 997! = 1000*999*998 / 6
    // 166167000

    triadCounts[0] = (list.size() * (list.size()-1) * (list.size()-2))/6 - total;

    total += triadCounts[0];

    cout << endl << "Triad Counts: " << endl;
    for (int i = 0; i < 4; i++) {
		cout << i << ": " << triadCounts[i] << endl;
    }
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

