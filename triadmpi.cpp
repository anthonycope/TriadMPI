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
#include "mpi.h"

using namespace std;

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

int main ()
{
    fstream inputFile;
    string line;

    inputFile.open("testinput.egonets");
    if(inputFile.is_open())
    {
        while(!inputFile.eof())
        {
            getline(inputFile, line);
            stringstream ss(line); 
            string num;
            string temp;
            int i;
            bool first = true;

            getline(ss, num, ' ');
        
            vector<string> nodes = tokenize(num, "\t");

            cout << "FIRST " << nodes[0] << ": ";

            for (int i = 1; i < nodes.size(); i++)
                cout << nodes[i] << " ";
            
            cout << "endline" << endl;
        }
    }  

    inputFile.close();
}



// int n = num nodes;
//intialize an empty array to hold count for each type of triadmpi
// counts[4];
//take first element of row (u)
    //for each "neighbor" v of that row
        // if uv is less than v
                //S = all neighbors of of first element, all neighbors of the selected neighbor (have to find them)
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