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
#include <ctime>
#include <unordered_set>
#include <queue>

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
		int *bestPos) {

	unsigned int i = 0;
	*bestPos = 0;
	vector<array<int, 16>> best = queue[i];
	++i;
	for (; i < queue.size(); i++) {
		if (best.back()[0] < queue[i].back()[0]) {
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
	for (unsigned int i = 0; i < path.size() - 1; i++) {
		ss << "\n   |   \n   v   \n";
		ss << printNode(path[i]);
	}
	ss << "\nNumber of moves: ";
	ss << path.size() - 2;

	return ss.str();
}

vector<array<int, 16>> aStarFirstCol(vector<array<int, 16>> start) {
	array<int, 4> goal = { 1, 5, 9, 13 };
	auto cmp = [](vector<array<int, 16>> left, vector<array<int, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<int, 16>>, vector<vector<array<int, 16>>>, decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath[extendedPath.size() - 2][0], extendedPath[extendedPath.size() - 2][4],
			extendedPath[extendedPath.size() - 2][8], extendedPath[extendedPath.size() - 2][12] };

	while ((queue.size() > 0) && !(check == goal)) {
		queue.pop();
		array<int, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);
		extendedPath.pop_back();

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<int, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 1));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
			}
		}
		extendedPath = queue.top();
		check = { extendedPath[extendedPath.size() - 2][0], extendedPath[extendedPath.size() - 2][4],
				extendedPath[extendedPath.size() - 2][8], extendedPath[extendedPath.size() - 2][12] };
	}
	cout << "done with first colunm" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStarSecondRow(vector<array<int, 16>> start) {
	array<int, 4> goal = { 5, 6, 7, 8 };
	auto cmp = [](vector<array<int, 16>> left, vector<array<int, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<int, 16>>, vector<vector<array<int, 16>>>, decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath[extendedPath.size() - 2][4], extendedPath[extendedPath.size() - 2][5],
			extendedPath[extendedPath.size() - 2][6], extendedPath[extendedPath.size() - 2][7] };

	while ((queue.size() > 0) && !(check == goal)) {
		queue.pop();
		array<int, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);
		extendedPath.pop_back();

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<int, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 2));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
			}
		}
		extendedPath = queue.top();
		check = { extendedPath[extendedPath.size() - 2][4], extendedPath[extendedPath.size() - 2][5],
				extendedPath[extendedPath.size() - 2][6], extendedPath[extendedPath.size() - 2][7] };
	}
	cout << "done with second row" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStarFirstRow(vector<array<int, 16>> start) {
	clock_t startFirst = clock();
	array<int, 4> goal = { 1, 2, 3, 4 };

	auto cmp = [](vector<array<int, 16>> left, vector<array<int, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<int, 16>>, vector<vector<array<int, 16>>>, decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	array<int, 4> check = { extendedPath[extendedPath.size() - 2][0], extendedPath[extendedPath.size() - 2][1],
			extendedPath[extendedPath.size() - 2][2], extendedPath[extendedPath.size() - 2][3] };
	double part1, part2, part3 = 0;
	while ((queue.size() > 0) && !(check == goal)) {
		clock_t p1 = clock();
		queue.pop();
		array<int, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);
		extendedPath.pop_back();

		clock_t p2 = clock();
		part1 += (double) (p2 - p1) / CLOCKS_PER_SEC;
		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<int, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 0));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
			}
		}

		clock_t p3 = clock();
		part2 += (double) (p3 - p2) / CLOCKS_PER_SEC;
		extendedPath = queue.top();
		check = { extendedPath[extendedPath.size() - 2][0], extendedPath[extendedPath.size() - 2][1],
					extendedPath[extendedPath.size() - 2][2], extendedPath[extendedPath.size() - 2][3] };
		clock_t endPart = clock();
		part3 += (double) (endPart - p3) / CLOCKS_PER_SEC;
	}
	clock_t endFirst = clock();
	double totalFirst = (double) (endFirst - startFirst) / CLOCKS_PER_SEC;
	cout << "Total: " << totalFirst << " seconds." << endl;
	cout << "Part 1: " << part1 << " seconds;" << " = " << (int)((part1/totalFirst)*100) << "%" << endl;
	cout << "Part 2: " << part2 << " seconds;" << " = " << (int)((part2/totalFirst)*100) << "%" << endl;
	cout << "Part 3: " << part3 << " seconds;" << " = " << (int)((part3/totalFirst)*100) << "%" << endl;
//	cout << "done with first row" << endl;
	return extendedPath;
}

vector<array<int, 16>> aStar(vector<array<int, 16>> start,
		array<int, 16> goal) {

	auto cmp = [](vector<array<int, 16>> left, vector<array<int, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<int, 16>>, vector<vector<array<int, 16>>>, decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<int, 16>> extendedNodes;
	vector<array<int, 16>> extendedPath = start;
	while ((queue.size() > 0) && !(extendedPath[extendedPath.size() - 2] == goal)) {
		queue.pop();
		array<int, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<int, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedNodes.insert(leafNode);
		extendedPath.pop_back();

		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<int, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<int, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 16));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
			}
		}
		extendedPath = queue.top();
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
	array<int, 16> solv = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };

	array<int, 16> rootLength;
	rootLength.fill(0);

	vector<array<int, 16>> testPath1 = { test1, rootLength };
	vector<array<int, 16>> testPath2 = { test2, rootLength };
	vector<array<int, 16>> testPath3 = { test3, rootLength };
	vector<array<int, 16>> testPath4 = { test4, rootLength };
	vector<array<int, 16>> gamePath1 = { game1, rootLength };
	vector<array<int, 16>> gamePath2 = { game2, rootLength };
	vector<array<int, 16>> gamePath3 = { game3, rootLength };
	vector<array<int, 16>> gamePath4 = { game4, rootLength };
	vector<array<int, 16>> gamePath5 = { game5, rootLength };
	vector<array<int, 16>> gamePath6 = { game6, rootLength };
	vector<array<int, 16>> gamePath7 = { game7, rootLength };
	vector<array<int, 16>> gamePath8 = { game8, rootLength };
	vector<array<int, 16>> gamePath9 = { game9, rootLength };
	vector<array<int, 16>> gamePath10 = { game10, rootLength };
	vector<array<int, 16>> trivialPath = { solv, rootLength };

//	auto paths = {gamePath1, gamePath2, gamePath3, gamePath4, gamePath5, gamePath6, gamePath7, gamePath8, gamePath9, gamePath10};
	clock_t startTime = clock();
//	for (auto p: paths){
//		vector<array<int, 16>> solution = aStarFirstRow(p);
//	}
//	vector<array<int, 16>> solution = aStarFirstRow(gamePath10);
//	vector<array<int, 16>> solution = aStar(gamePath10, solv);
//	vector<array<int, 16>> solution = aStar(aStarFirstRow(gamePath10), solv);
	vector<array<int, 16>> solution = aStar(aStarSecondRow(aStarFirstRow(gamePath4)), solv);
//	vector<array<int, 16>> solution = aStar(aStarFirstCol(aStarSecondRow(aStarFirstRow(gamePath4))), solv);
//	vector<array<int, 16>> solution = aStar(aStarSecondRow(aStarFirstCol(aStarFirstRow(gamePath10))), solv);



	clock_t programTime = clock() - startTime;
	cout << "Solution:" << endl;
	cout << printPath(solution) << endl;
	cout << "Program took: " << (float) programTime / CLOCKS_PER_SEC
			<< " seconds." << endl;

	return 0;
}
