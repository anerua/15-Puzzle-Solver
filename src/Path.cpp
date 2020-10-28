/*
 * Path.cpp
 *
 *  Created on: 23 Oct 2020
 *      Author: martins
 */

#include <array>
#include <vector>
#include <iostream>
#include <sstream>
#include "Path.h"
#include "Node.h"

using namespace std;

Path::Path(vector<Node> nodes) {
	if (nodes.size() > 0) {
		empty = false;
		this->nodes = nodes;
		leaf = nodes.back();
		length = nodes.size() + leaf.getValue();
	} else {
		empty = true;
		length = 0;
	}

}

string Path::print() {
	stringstream ss;

	if (empty) {
		ss << "Empty Path";
	} else {
		for (unsigned int i = 0; i < nodes.size(); i++) {
			ss << "\n   |   \n   v   \n";
			ss << nodes[i].print();
		}
		ss << "\nNumber of moves: ";
		ss << nodes.size() - 1;
	}

	return ss.str();
}

Node Path::getLeaf() {
	return leaf;
}

int Path::getPathLength() {
	return length;
}

vector<Node> Path::getNodes() {
	return nodes;
}
