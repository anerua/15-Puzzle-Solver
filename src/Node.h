/*
 * Node.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Martins Anerua
 */

#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <array>
#include <vector>

using namespace std;

class Node {
private:
	int rows[4][4];
	int value;
	bool empty;
	array<int, 16> flattened;

public:
	Node(): value(0), empty(true) {};
	Node(array<int, 16> state);
	string print();
	bool equals(Node node);
	vector<Node> getConnectedNodes();
	int getValue();
	array<int, 16> getFlattened();
};

#endif /* NODE_H_ */
