/*
 * Node.cpp
 *
 *  Created on: 23 Oct 2020
 *      Author: Martins Anerua
 */

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <cstdlib>
#include "Node.h"

using namespace std;

Node::Node(array<int, 16> state) :
		value(0), empty(false) {
	flattened = state;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			rows[i][j] = state[(j % 4) + (i * 4)];
		}
	}
}

string Node::print() {
	stringstream ss;

	if (empty) {
		ss << "Empty Node";
	} else {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ss << rows[i][j];
				ss << "  ";
			}
			ss << "\n";
		}
	}

	return ss.str();
}

bool Node::equals(Node node) {
	if (this->empty && node.empty) {
		return true;
	} else if (this->empty || node.empty) {
		return false;
	} else {
		for (int i = 0; i < 16; i++) {
			if (this->flattened[i] != node.flattened[i]) {
				return false;
			}
		}
	}

	return true;
}

int Node::getValue() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (rows[i][j] == 0){
				continue;
			} else {
				value += abs(rows[i][j] - ((j+1) + (4*i)));
			}
		}
	}
	return value;
}

array<int, 16> Node::getFlattened() {
	return flattened;
}

vector<Node> Node::getConnectedNodes() {
	vector<Node> children;

	if (empty) {
		cout << "How?" << endl;
//		children[0] = Node();
//		children[1] = Node();
//		children[2] = Node();
//		children[3] = Node();
	} else {
		// get the empty space index
		int emptyPos;
		for (int i = 0; i < 16; i++) {
			if (flattened[i] == 0) {
				emptyPos = i;
			}
		}
		int emptyRow = emptyPos / 4;
		int emptyCol = emptyPos % 4;

		// Move up
		if (emptyRow != 0) {
			array<int, 16> tempState = flattened;
			tempState[emptyPos] = tempState[emptyPos - 4];
			tempState[emptyPos - 4] = 0;
			Node tempNode = Node(tempState);
			children.push_back(tempNode);
		}

		// Move down
		if (emptyRow != 3) {
			array<int, 16> tempState = flattened;
			tempState[emptyPos] = tempState[emptyPos + 4];
			tempState[emptyPos + 4] = 0;
			Node tempNode = Node(tempState);
			children.push_back(tempNode);
		}

		// Move left
		if (emptyCol != 0) {
			array<int, 16> tempState = flattened;
			tempState[emptyPos] = tempState[emptyPos - 1];
			tempState[emptyPos - 1] = 0;
			Node tempNode = Node(tempState);
			children.push_back(tempNode);
		}

		// Move right
		if (emptyCol != 3) {
			array<int, 16> tempState = flattened;
			tempState[emptyPos] = tempState[emptyPos + 1];
			tempState[emptyPos + 1] = 0;
			Node tempNode = Node(tempState);
			children.push_back(tempNode);
		}
	}

	return children;
}
