/*
 * Path.h
 *
 *  Created on: 23 Oct 2020
 *      Author: martins
 */

#ifndef PATH_H_
#define PATH_H_

#include <iostream>
#include <array>
#include <vector>
#include "Node.h"

using namespace std;

class Path {
private:
	vector<Node> nodes;
	Node leaf;
	int length;
	bool empty;
public:
	Path(): length(0), empty(true) {};
	Path(vector<Node> nodes);
	string print();
	Node getLeaf();
	vector<Node> getNodes();
	int getPathLength();
};

#endif /* PATH_H_ */
