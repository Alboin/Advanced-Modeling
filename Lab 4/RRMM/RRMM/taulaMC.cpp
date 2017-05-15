#include "taulaMC.hpp"
#include <assert.h>
#include <iostream>
using namespace std;

int main() {
  int cas;
  MCcases casos;
  
  cout << "Which case do you want to check? (0-255, ^D to exit): ";
  while (cin >> cas) {
    const vector<vector<int> >& triangles = casos(cas);
    cout << "case " << cas << " generates " << triangles.size() << " triangles:" << endl;
    for (unsigned int i = 0; i < triangles.size(); ++i) {
      assert(triangles.at(i).size() == 3);
      cout <<"<"<<triangles.at(i)[0]<<", "<<triangles.at(i)[1]<<", "<<triangles.at(i)[2]<<">    ";
    }
    cout << endl << "Which case do you want to check? (0-255): ";
  }
  cout << endl;
  return 0;
}
  
