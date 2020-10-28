//============================================================================
// Name        : Stash.cpp
// Author      : Martins Anerua
// Version     :
// Copyright   : MIT License
// Description : Sliding Number Puzzle Solver
// Date Created: 23 October 2020 12:14
//============================================================================

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <algorithm>
#include <ctime>
#include <unordered_set>
#include "Node.h"
#include "Path.h"

using namespace std;

namespace std {
template<typename T, size_t N>
struct hash<array<T, N>> {
	typedef array<T, N> argument_type;
	typedef size_t result_type;

	result_type operator()(const argument_type &a) const {
		hash<T> hasher;
		result_type h = 0;
		for (result_type i = 0; i < N; ++i) {
			h = h * 31 + hasher(a[i]);
		}
		return h;
	}
};
}

bool pathLength(Path p1, Path p2) {
	return (p1.getPathLength() > p2.getPathLength());
}

bool first(array<int, 2> p1, array<int, 2> p2) {
	return (p1[0] > p2[0]);
}

Path aStar(Path start, Node goal) {
	vector<Path> queue = { start };
	vector<array<int, 16>> extendedNodes = { };
	Path extendedPath = start;
	while ((queue.size() > 0) && !(extendedPath.getLeaf().equals(goal))) {
		queue.pop_back();
		Node leafNode = extendedPath.getLeaf();
		vector<Node> connectedNodes = leafNode.getConnectedNodes();
		extendedNodes.push_back(leafNode.getFlattened());

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(find(extendedNodes.begin(), extendedNodes.end(),
					connectedNodes[i].getFlattened()) != extendedNodes.end())) {
				vector<Node> tempExtended = extendedPath.getNodes();
				tempExtended.push_back(connectedNodes[i]);
				queue.push_back(Path(tempExtended));
			}
		}
		sort(queue.begin(), queue.end(), pathLength);
		extendedPath = queue.back(); // move to bottom
	}
	return extendedPath;
}

int pathLength(vector<array<int, 16>> path, int meta) {
	/*
	 * meta:
	 * 	0 = first row
	 * 	1 = first column
	 * 	2 = second row
	 * 	16 = full board
	 */
	int noOfNodes = path.size();
	int hammingDistance = 0;
	int manhattanDistance = 0;
	array<int, 16> leaf = path.back();
	switch(meta) {
	case 0:
		for (int i = 0; i < 16; i++) {
			if (leaf[i] == 1 || leaf[i] == 2 || leaf[i] == 3 || leaf[i] == 4) {
				if (leaf[i] !=  (i+1)){
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			} else {
				continue;
			}
		}
		break;
	case 1:
		for (int i = 0; i < 16; i++) {
			if (leaf[i] == 1 || leaf[i] == 5 || leaf[i] == 9 || leaf[i] == 13) {
				if (leaf[i] !=  (i+1)){
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			} else {
				continue;
			}
		}
		break;
	case 2:
		for (int i = 0; i < 16; i++) {
			if (leaf[i] == 5 || leaf[i] == 6 || leaf[i] == 7 || leaf[i] == 8) {
				if (leaf[i] !=  (i+1)){
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			} else {
				continue;
			}
		}
		break;
	case 16:
		for (int i = 0; i < 16; i++) {
			if (leaf[i] == 0) {
				continue;
			} else {
				if (leaf[i] !=  (i+1)){
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			}
		}
		break;
	}

	return noOfNodes + hammingDistance + manhattanDistance;
}

vector<array<int, 16>> bestPath(vector<vector<array<int, 16>>> queue,
		int *bestPos, int meta) {
	/*
	 * row:
	 * 	0 = first row
	 * 	16 = full board
	 */
	unsigned int i = 0;
	*bestPos = 0;
	vector<array<int, 16>> best = queue[i];
	++i;
	for (; i < queue.size(); i++) {
		if (pathLength(best, meta) < pathLength(queue[i], meta)) {
			continue;
		} else {
			best = queue[i];
			*bestPos = i;
		}
	}
	return best;
}

bool colSolved(array<int,16> node, int col) {
	if (col != 0) {
		return false;
	} else {
		for (int i = 0; i < 4; i++) {
			if (node[i*4] == 0) {
				return false;
			} else if (node[i*4] != (i*4) + 1){
				return false;
			} else {
				continue;
			}
		}
		return true;
	}
}

bool rowSolved(array<int,16> node, int row) {
	for (int i = 0; i < 4; i++) {
		if (node[(i%4) + (row*4)] == 0) {
			return false;
		} else if (node[(i%4) + (row*4)] != (i%4) + (row*4) + 1){
			return false;
		} else {
			continue;
		}
	}

	if (row == 0) {
		return true;
	} else {
		return rowSolved(node, row - 1);
	}
}

vector<array<int, 16>> getConnectedNodes(array<int, 16> node) {
	vector<array<int, 16>> children;

	// get the empty space index
	int emptyPos;
	for (int i = 0; i < 16; i++) {
		if (node[i] == 0) {
			emptyPos = i;
		}
	}
	int emptyRow = emptyPos / 4;
	int emptyCol = emptyPos % 4;

	// Move up
	if ((emptyRow != 0) && !(rowSolved(node, emptyRow - 1))) {
		array<int, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos - 4];
		tempState[emptyPos - 4] = 0;
		children.push_back(tempState);
	}

	// Move down
	if (emptyRow != 3) {
		array<int, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos + 4];
		tempState[emptyPos + 4] = 0;
		children.push_back(tempState);
	}

	// Move left
	if ((emptyCol != 0) && !(colSolved(node, emptyCol - 1))) {
		array<int, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos - 1];
		tempState[emptyPos - 1] = 0;
		children.push_back(tempState);
	}

	// Move right
	if (emptyCol != 3) {
		array<int, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos + 1];
		tempState[emptyPos + 1] = 0;
		children.push_back(tempState);
	}

	return children;
}

string printNode(array<int, 16> node) {
	stringstream ss;
	for (int i = 0; i < 16; i++) {
		ss << node[i];
		ss << "  ";
		if ((i % 4) == 3) {
			ss << "\n";
		}
	}
	return ss.str();
}

string printPath(vector<array<int, 16>> path) {
	stringstream ss;
	for (unsigned int i = 0; i < path.size(); i++) {
		ss << "\n   |   \n   v   \n";
		ss << printNode(path[i]);
	}
	ss << "\nNumber of moves: ";
	ss << path.size() - 1;

	return ss.str();
}

vector<array<int, 16>> aStarFirstCol(vector<array<int, 16>> start) {
	array<int, 4> goal = { 1, 5, 9, 13 };
	vector<vector<array<int, 16>>> queue = { start };
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath.back()[0], extendedPath.back()[4],
			extendedPath.back()[8], extendedPath.back()[12] };
	int pop_index = 0;
	while ((queue.size() > 0) && !(check == goal)) {
		queue.erase(queue.begin() + pop_index);
		array<int, 16> leafNode = extendedPath.back();
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				queue.push_back(tempExtended);
			}
		}
		extendedPath = bestPath(queue, &pop_index, 1);
		check = { extendedPath.back()[0], extendedPath.back()[4],
				extendedPath.back()[8], extendedPath.back()[12] };
	}
	cout << "done with first colunm" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStarSecondRow(vector<array<int, 16>> start) {
	array<int, 4> goal = { 5, 6, 7, 8 };
	vector<vector<array<int, 16>>> queue = { start };
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath.back()[4], extendedPath.back()[5],
			extendedPath.back()[6], extendedPath.back()[7] };
	int pop_index = 0;
	while ((queue.size() > 0) && !(check == goal)) {
		queue.erase(queue.begin() + pop_index);
		array<int, 16> leafNode = extendedPath.back();
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				queue.push_back(tempExtended);
			}
		}
		extendedPath = bestPath(queue, &pop_index, 2);
		check = { extendedPath.back()[4], extendedPath.back()[5],
				extendedPath.back()[6], extendedPath.back()[7] };
	}
	cout << "done with second row" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStarFirstRow(vector<array<int, 16>> start) {
	array<int, 4> goal = { 1, 2, 3, 4 };
	vector<vector<array<int, 16>>> queue = { start };
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath.back()[0], extendedPath.back()[1],
			extendedPath.back()[2], extendedPath.back()[3] };
	int pop_index = 0;
	while ((queue.size() > 0) && !(check == goal)) {
		queue.erase(queue.begin() + pop_index);
		array<int, 16> leafNode = extendedPath.back();
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				queue.push_back(tempExtended);
			}
		}
		extendedPath = bestPath(queue, &pop_index, 0);
		check = { extendedPath.back()[0], extendedPath.back()[1],
				extendedPath.back()[2], extendedPath.back()[3] };
	}
	cout << "done with first row" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStar(vector<array<int, 16>> start,
		array<int, 16> goal) {
	vector<vector<array<int, 16>>> queue = { start };
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	int pop_index = 0;
	while ((queue.size() > 0) && !(extendedPath.back() == goal)) {
		queue.erase(queue.begin() + pop_index);
		array<int, 16> leafNode = extendedPath.back();
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				queue.push_back(tempExtended);
			}
		}
		extendedPath = bestPath(queue, &pop_index, 16);
	}
	return extendedPath;
}

int main() {

	array<int, 16> test1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0,
			15 };
	array<int, 16> test2 = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 15, 14, 13, 12, 11, 10,
			9 };
	array<int, 16> test3 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 13, 14,
			15 };
	array<int, 16> test4 = { 1, 2, 3, 4, 5, 6, 7, 8, 14, 0, 15, 9, 10, 13, 12,
			11 };
	array<int, 16> game1 = { 4, 8, 9, 13, 12, 6, 5, 1, 10, 14, 7, 3, 11, 0, 2,
			15 };
	array<int, 16> game2 = { 11, 3, 8, 4, 10, 9, 14, 0, 2, 5, 7, 15, 1, 13, 6,
			12 };
	array<int, 16> game3 = { 7, 8, 9, 2, 11, 12, 14, 10, 0, 13, 1, 4, 5, 15, 6,
			3 };
	array<int, 16> game4 = { 7, 10, 3, 0, 15, 6, 4, 8, 12, 1, 11, 5, 14, 13, 9,
			2 };
	array<int, 16> game5 = { 2, 15, 8, 5, 10, 12, 3, 6, 0, 11, 4, 1, 9, 14, 7,
			13 };
	array<int, 16> game6 = { 1, 6, 2, 7, 5, 11, 0, 3, 9, 10, 15, 4, 13, 14, 12,
			8 };
	array<int, 16> game7 = { 6, 14, 3, 7, 8, 4, 15, 12, 13, 0, 5, 1, 10, 2, 9,
				11 };
	array<int, 16> game8 = { 3, 8, 11, 4, 10, 15, 14, 9, 6, 1, 2, 7, 0, 12, 13,
				5 };
	array<int, 16> game9 = { 1, 4, 11, 10, 14, 8, 7, 6, 0, 5, 9, 3, 13, 12, 15,
				2 };
	array<int, 16> game10 = { 0, 10, 7, 5, 3, 9, 8, 12, 6, 1, 15, 13, 11, 2, 14,
				4 };

	array<int, 16> solv =
			{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };

//	Node node1 = Node(test1);
//	Node node2 = Node(test2);
//	Node node3 = Node(test3);
//	Node node4 = Node(test4);
//	Node solved = Node(solv);
//	Node empty1 = Node();
//	Node empty2 = Node();

	/*cout << "====================" << endl;
	 cout << "Print test:" << endl;
	 cout << "====================" << endl;
	 cout << "Node 1:" << endl;
	 cout << node1.print() << endl;
	 cout << "Node 2:" << endl;
	 cout << node2.print() << endl;
	 cout << "Node 3:" << endl;
	 cout << node3.print() << endl;
	 cout << "Node 4:" << endl;
	 cout << node4.print() << endl;
	 cout << "Empty Node 1:" << endl;
	 cout << empty1.print() << endl;
	 cout << "====================" << endl << endl;

	 cout << "====================" << endl;
	 cout << "Equality test:" << endl;
	 cout << "====================" << endl;
	 cout << "0: " << node1.equals(node2) << endl;
	 cout << "1: " << node1.equals(node1) << endl;
	 cout << "0: " << node3.equals(node4) << endl;
	 cout << "1: " << node3.equals(node3) << endl;
	 cout << "0: " << empty1.equals(node1) << endl;
	 cout << "1: " << empty1.equals(empty2) << endl;
	 cout << "1: " << empty1.equals(empty1) << endl;
	 cout << "====================" << endl << endl;

	 cout << "====================" << endl;
	 cout << "Next moves test:" << endl;
	 cout << "====================" << endl;
	 cout << "Node 1:" << endl;
	 array<Node,4> next = node1.getConnectedNodes();
	 cout << "-------" << endl;
	 cout << "Up:" << endl;
	 cout << next[0].print() << endl;
	 cout << "Down:" << endl;
	 cout << next[1].print() << endl;
	 cout << "Left:" << endl;
	 cout << next[2].print() << endl;
	 cout << "Right:" << endl;
	 cout << next[3].print() << endl;
	 cout << "-------" << endl;
	 cout << "Empty Node 1:" << endl;
	 next = empty1.getConnectedNodes();
	 cout << "-------" << endl;
	 cout << "Up:" << endl;
	 cout << next[0].print() << endl;
	 cout << "Down:" << endl;
	 cout << next[1].print() << endl;
	 cout << "Left:" << endl;
	 cout << next[2].print() << endl;
	 cout << "Right:" << endl;
	 cout << next[3].print() << endl;
	 cout << "====================" << endl << endl;*/

//	vector<Node> testPath1 = { node1 };
//	vector<Node> testPath2 = { node2 };
//	vector<Node> testPath3 = { node3 };
//	vector<Node> testPath4 = { node4 };
//	vector<Node> solvPath = { solved };
	vector<array<int, 16>> testPath1 = { test1 };
	vector<array<int, 16>> testPath2 = { test2 };
	vector<array<int, 16>> testPath3 = { test3 };
	vector<array<int, 16>> testPath4 = { test4 };
	vector<array<int, 16>> gamePath1 = { game1 };
	vector<array<int, 16>> gamePath2 = { game2 };
	vector<array<int, 16>> gamePath3 = { game3 };
	vector<array<int, 16>> gamePath4 = { game4 };
	vector<array<int, 16>> gamePath5 = { game5 };
	vector<array<int, 16>> gamePath6 = { game6 };
	vector<array<int, 16>> gamePath7 = { game7 };
	vector<array<int, 16>> gamePath8 = { game8 };
	vector<array<int, 16>> gamePath9 = { game9 };
	vector<array<int, 16>> gamePath10 = { game10 };
	vector<array<int, 16>> trivialPath = { solv };

//	Path path1 = Path(testPath1);
//	Path path2 = Path(testPath2);
//	Path path3 = Path(testPath3);
//	Path path4 = Path(testPath4);
//	Path trivialPath = Path(solvPath);
//	Path path6 = Path();

	/*	cout << "====================" << endl;
	 cout << "Print test:" << endl;
	 cout << "====================" << endl;
	 cout << "Path 1:" << endl;
	 cout << path1.print() << endl;
	 cout << "--------------------" << endl;
	 cout << "Path 2:" << endl;
	 cout << path2.print() << endl;
	 cout << "--------------------" << endl;
	 cout << "Path 3:" << endl;
	 cout << path3.print() << endl;
	 cout << "--------------------" << endl;
	 cout << "Path 4:" << endl;
	 cout << path4.print() << endl;
	 cout << "--------------------" << endl;
	 cout << "Path 5:" << endl;
	 cout << path5.print() << endl;
	 cout << "--------------------" << endl;
	 cout << "Path 6:" << endl;
	 cout << path6.print() << endl;
	 cout << "====================" << endl << endl;*/

	/*vector<array<int,2>> vec1 = {{1,2},{3,4}};
	 array<int,2> vec2 = {3,5};
	 if (vec1[0] == vec2) {
	 cout << "We are equal" << endl;
	 } else {
	 cout << "I hate her" << endl;
	 }
	 if (find(vec1.begin(), vec1.end(), vec2) != vec1.end()){
	 cout << "Present sir" << endl;
	 } else {
	 cout << "Absent sir" << endl;
	 }
	 sort(vec1.begin(), vec1.end(), first);
	 cout << vec1[0][0] << " " << vec1[1][0] << endl;*/

//	Path solution = aStar(path2, solved);
//
//	cout << "Solution:" << endl;
//	cout << solution.print() << endl;
	clock_t startTime = clock();
//	vector<array<int, 16>> solution = aStarFirstRow(gamePath5);
//	vector<array<int, 16>> solution = aStarFirstCol(gamePath4);
//	vector<array<int, 16>> solution = aStar(gamePath2, solv);
//	vector<array<int, 16>> solution = aStarFirstCol(aStarFirstRow(gamePath1));
//	vector<array<int, 16>> solution = aStarSecondRow(aStarFirstRow(gamePath1));
//	vector<array<int, 16>> solution = aStar(aStarFirstRow(gamePath2), solv);
	vector<array<int, 16>> solution = aStar(aStarSecondRow(aStarFirstRow(gamePath10)), solv);
//	vector<array<int, 16>> solution = aStar(aStarFirstCol(aStarSecondRow(aStarFirstRow(gamePath10))), solv);
//	vector<array<int, 16>> solution = aStar(aStarSecondRow(aStarFirstCol(aStarFirstRow(gamePath1))), solv);
//	vector<array<int, 16>> solution = aStar(aStarFirstCol(aStarSecondRow(aStarFirstRow(gamePath1))), solv);

	clock_t programTime = clock() - startTime;
	cout << "Solution:" << endl;
	cout << printPath(solution) << endl;
	cout << "Program took: " << (float) programTime / CLOCKS_PER_SEC
			<< " seconds." << endl;

//	clock_t startTime = clock();
//	vector<array<int, 16>> solution = aStar(gamePath2, solv);
//	clock_t programTime = clock() - startTime;
//	cout << "Solution:" << endl;
//	cout << printPath(solution) << endl;
//	cout << "Program took: " << (float) programTime / CLOCKS_PER_SEC
//			<< " seconds." << endl;

//	unordered_set<array<int, 16>> example;
//	example.insert(test1);
//	example.insert(test2);
//	example.insert(test3);
//	example.insert(test1);
//
//	unordered_set<array<int, 16>>::iterator itr;
//
//	for (itr = example.begin(); itr != example.end(); itr++) {
//		array<int, 16> temp = *itr;
//		cout << "[" << flush;
//		for (int i = 0; i < 16; i++) {
//			cout << temp[i] << ", " << flush;
//		}
//		cout << "]" << endl;
//	}
//
//	if (example.find(test4) != example.end()) {
//		cout << "found!" << endl;
//	} else {
//		cout << "Not found" << endl;
//	}

//	array<int,16> check = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0,
//			15 };
//	vector<array<int,16>> t1 = {test1};
//	if (t1.front() == check) {
//		cout << "Yes!" << endl;
//	}

//	array<int,4> sliced = {check[0], check[1], check[2], check[3]};
//	for (int i = 0; i<4; i++) {
//		cout << sliced[i] << " " << flush;
//	}

//	for (int i = 0; i < 3; ++i){
//		cout << "[" << flush;
//		for (int j = 0; j < 16; ++j){
//			cout << "" << endl;
//		}
//	}

	return 0;
}
