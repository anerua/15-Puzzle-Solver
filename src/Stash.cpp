//============================================================================
// Name        : Stash.cpp
// Author      : Martins Anerua
// Version     : 1.1.0
// Copyright   : MIT License
// Description : 15 Puzzle Solver
// Date Created: 23 October 2020 12:14
//============================================================================

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <ctime>
#include <unordered_set>
#include <queue>
#include <algorithm>

using namespace std;

/*
 * Hash function for array unordered set
 */
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

array<char, 16> inputState;

int pathLength(vector<array<char, 16>> path, int meta) {
	/*
	 * meta:
	 * 	0 = first row
	 * 	1 = second row
	 * 	16 = full board
	 */
	int noOfNodes = path.size();
	int hammingDistance = 0;
	int manhattanDistance = 0;
	array<char, 16> leaf = path.back();
	switch (meta) {
	case 0:
		for (char i = 0; i < 16; i++) {
			if (leaf[i] == 1 || leaf[i] == 2 || leaf[i] == 3 || leaf[i] == 4) {
				if (leaf[i] != (i + 1)) {
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			}
		}
		break;
	case 1:
		for (char i = 0; i < 16; i++) {
			if (leaf[i] == 5 || leaf[i] == 6 || leaf[i] == 7 || leaf[i] == 8) {
				if (leaf[i] != (i + 1)) {
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			}
		}
		break;
	case 16:
		for (char i = 0; i < 16; i++) {
			if (leaf[i] != 0) {
				if (leaf[i] != (i + 1)) {
					++hammingDistance;
				}
				manhattanDistance += abs(leaf[i] - (i + 1));
			}
		}
		break;
	}

	return noOfNodes + hammingDistance + manhattanDistance;
}

bool rowSolved(array<char, 16> node, int row) {
	for (char i = 0; i < 4; i++) {
		char j = (i % 4) + (row * 4);
		if ((node[j] == 0) || (node[j] != j + 1)) {
			return false;
		}
	}

	return (row == 0) ? true : rowSolved(node, row - 1);
}

vector<array<char, 16>> getConnectedNodes(array<char, 16> node) {
	vector<array<char, 16>> children;

	// get the empty space index
	char emptyPos;
	for (char i = 0; i < 16; i++) {
		if (node[i] == 0) {
			emptyPos = i;
			break;
		}
	}
	char emptyRow = emptyPos / 4;
	char emptyCol = emptyPos % 4;

	// Move up
	if ((emptyRow != 0) && !(rowSolved(node, emptyRow - 1))) {
		array<char, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos - 4];
		tempState[emptyPos - 4] = 0;
		children.push_back(tempState);
	}

	// Move down
	if (emptyRow != 3) {
		array<char, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos + 4];
		tempState[emptyPos + 4] = 0;
		children.push_back(tempState);
	}

	// Move left
	if (emptyCol != 0) {
		array<char, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos - 1];
		tempState[emptyPos - 1] = 0;
		children.push_back(tempState);
	}

	// Move right
	if (emptyCol != 3) {
		array<char, 16> tempState = node;
		tempState[emptyPos] = tempState[emptyPos + 1];
		tempState[emptyPos + 1] = 0;
		children.push_back(tempState);
	}

	return children;
}

string printNode(array<char, 16> node) {
	stringstream ss;
	ss << ".===================.\n";
	ss << "| ";
	for (char i = 0; i < 16; i++) {
		if (node[i] != 0)
			ss << (int) node[i];
		else
			ss << " ";
		if (node[i] < 10)
			ss << " ";
		ss << " | ";
		if ((i % 4) == 3 && (i != 15))
			ss << "\n|-------------------|\n| ";
	}
	ss << "\n*===================*\n";
	return ss.str();
}

string printPath(vector<array<char, 16>> path) {
	stringstream ss;
	for (unsigned int i = 0; i < path.size() - 1; i++) {
		if (i > 0) {
			ss << "\n         |\n         v\n";
		}
		ss << printNode(path[i]);
	}
	ss << "\nNumber of moves: ";
	ss << path.size() - 2;

	return ss.str();
}

vector<array<char, 16>> aStarSecondRow(vector<array<char, 16>> start) {
	array<char, 4> goal = { 5, 6, 7, 8 };
	auto cmp = [](vector<array<char, 16>> left, vector<array<char, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<char, 16>>, vector<vector<array<char, 16>>>,
			decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<char, 16>> extendedNodes;
	vector<array<char, 16>> extendedPath = start;
	array<char, 4> check = { extendedPath[extendedPath.size() - 2][4],
			extendedPath[extendedPath.size() - 2][5],
			extendedPath[extendedPath.size() - 2][6],
			extendedPath[extendedPath.size() - 2][7] };
	extendedNodes.insert(extendedPath[extendedPath.size() - 2]);
	while ((queue.size() > 0) && !(check == goal)) {
		queue.pop();
		array<char, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<char, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedPath.pop_back();
		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<char, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<char, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 1));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
				extendedNodes.insert(connectedNodes[i]);
			}
		}
		extendedPath = queue.top();
		check = { extendedPath[extendedPath.size() - 2][4],
				extendedPath[extendedPath.size() - 2][5],
				extendedPath[extendedPath.size() - 2][6],
				extendedPath[extendedPath.size() - 2][7] };
	}
	return extendedPath;
}

vector<array<char, 16>> aStarFirstRow(vector<array<char, 16>> start) {
	array<char, 4> goal = { 1, 2, 3, 4 };
	auto cmp = [](vector<array<char, 16>> left, vector<array<char, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<char, 16>>, vector<vector<array<char, 16>>>,
			decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<char, 16>> extendedNodes;
	vector<array<char, 16>> extendedPath = start;
	array<char, 4> check = { extendedPath[extendedPath.size() - 2][0],
			extendedPath[extendedPath.size() - 2][1],
			extendedPath[extendedPath.size() - 2][2],
			extendedPath[extendedPath.size() - 2][3] };
	extendedNodes.insert(extendedPath[extendedPath.size() - 2]);
	while ((queue.size() > 0) && !(check == goal)) {
		queue.pop();
		array<char, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<char, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedPath.pop_back();
		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<char, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<char, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 0));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
				extendedNodes.insert(connectedNodes[i]);
			}
		}
		extendedPath = queue.top();
		check = { extendedPath[extendedPath.size() - 2][0],
				extendedPath[extendedPath.size() - 2][1],
				extendedPath[extendedPath.size() - 2][2],
				extendedPath[extendedPath.size() - 2][3] };
	}
	return extendedPath;
}

vector<array<char, 16>> aStar(vector<array<char, 16>> start,
		array<char, 16> goal) {

	auto cmp = [](vector<array<char, 16>> left, vector<array<char, 16>> right) {
		return left.back()[0] > right.back()[0];
	};
	priority_queue<vector<array<char, 16>>, vector<vector<array<char, 16>>>,
			decltype(cmp)> queue(cmp);
	queue.push(start);
	unordered_set<array<char, 16>> extendedNodes;
	vector<array<char, 16>> extendedPath = start;
	extendedNodes.insert(extendedPath[extendedPath.size() - 2]);
	while ((queue.size() > 0)
			&& !(extendedPath[extendedPath.size() - 2] == goal)) {
		queue.pop();
		array<char, 16> leafNode = extendedPath[extendedPath.size() - 2];
		vector<array<char, 16>> connectedNodes = getConnectedNodes(leafNode);
		extendedPath.pop_back();
		for (unsigned int i = 0; i < connectedNodes.size(); i++) {
			if (!(extendedNodes.find(connectedNodes[i]) != extendedNodes.end())) {
				vector<array<char, 16>> tempExtended = extendedPath;
				tempExtended.push_back(connectedNodes[i]);
				array<char, 16> pathLengthArray;
				pathLengthArray.fill(pathLength(tempExtended, 16));
				tempExtended.push_back(pathLengthArray);
				queue.push(tempExtended);
				extendedNodes.insert(connectedNodes[i]);
			}
		}
		extendedPath = queue.top();
	}
	return extendedPath;
}

void progressMessage(char status) {
	if (status) {
		cout << "\nInput error:" << endl;
		switch (status) {
		case 1:
			cout << "\tToo much/little board cells provided" << endl;
			break;
		case 2:
			cout << "\tOne or more invalid cell values provided" << endl;
			break;
		case 3:
			cout << "\tInsufficient/duplicate valid cell values provided" << endl;
			break;
		case 4:
			cout << "\tPuzzle is unsolvable" << endl;
		}
	} else {
		cout << "\tPuzzle is solvable!" << endl;
	}
}

char inversions(array<char, 16> inputGame) {
	char invCount = 0;
	for (char i = 0; i < 15; i++) {
		if (inputGame[i] == 0) {
			continue;
		}
		for (char j = i + 1; j < 16; j++) {
			if (inputGame[j] == 0) {
				continue;
			}
			if (inputGame[i] > inputGame[j]) {
				++invCount;
			}
		}
	}
	return invCount;
}

bool isSolvable(array<char, 16> inputGame) {
	/* RULES OF SOLVABILITY:
	 * Let the board be NxN
	 *
	 * 1. If N is odd, then puzzle instance is solvable if number of
	 * 	  inversions is even in the input state.
	 *
	 * 2. If N is even, puzzle instance is solvable if:
	 * 	  a. the blank is on an even row counting from the bottom and
	 * 	  	 number of inversions is odd.
	 * 	  b. the blank is on an odd row counting from the bottom and
	 * 	     number of inversions is even.
	 *
	 * 3. For all other cases, the puzzle instance is not solvable.
	 *
	 * Reference: https://wwww.geeksforgeeks.org/check-instance-15-puzzle-solvable/amp/
	 */

	// get the empty space index
	char emptyPos;
	for (char i = 0; i < 16; i++) {
		if (inputGame[i] == 0) {
			emptyPos = i;
			break;
		}
	}
	char emptyRow = 4 - (emptyPos / 4); // counting from bottom
	char invCount = inversions(inputGame);
	return ((!(emptyRow & 1) && (invCount & 1)) || ((emptyRow & 1) && !(invCount & 1))) ? true : false;
}

char verifyInput(string rawInput) {
	char status = 0;
	cout << "Checking input format..." << flush;
	char noComma = count(rawInput.begin(), rawInput.end(), ',');
	// check for more than required cells
	if (noComma != 15) {
		status = 1;
	} else {
		vector<string> tokens;
		stringstream check(rawInput);
		string intermediate;
		vector<string> valids = { "1", "2", "3", "4", "5", "6", "7", "8", "9",
				"10", "11", "12", "13", "14", "15", "0" };
		vector<string> validStream;
		// check cell values
		while (getline(check, intermediate, ',')) {
			if (!(find(valids.begin(), valids.end(), intermediate)
					!= valids.end())) {
				status = 2;
			} else if (!(find(validStream.begin(), validStream.end(), intermediate)
						!= validStream.end())) {
					validStream.push_back(intermediate);
			}
		}
		// check number of valid cell values
		if (validStream.size() != 16) {
			status = 3;
		} else {
			cout << "OK" << endl << "Checking solvability..." << endl;
			array<char, 16> inputGame;
			char i = 0;
			for (string cell : validStream) {
				inputGame[i] = (char) stoi(cell);
				++i;
			}
			if (!isSolvable(inputGame)) {
				status = 4;
			} else {
				inputState = inputGame;
			}
		}
	}
	return status;
}

string getInput() {
	string rawInput;
	cout << "Input the game as a comma-separated sequence of cell values" << endl;
	cout << "with the empty cell represented as 0" << endl;
	cout << "Do not put spaces between commas and values:" << endl;
	cout << "For example, a solved game would look like this:" << endl;
	cout << "\t1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0" << endl;
	cout << ">" << flush;
	cin >> rawInput;
	cout << endl;
	return rawInput;
}

int main() {
	array<char, 16> solved = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };
	array<char, 16> rootLength; // initial length
	rootLength.fill(0);
	cout << "===========================================================" << endl;
	cout << "                     15 Puzzle Solver                      " << endl;
	cout << "===========================================================" << endl << endl;
	string rawInput = getInput();
	char status = verifyInput(rawInput);
	progressMessage(status);
	if (!status) {
		cout << "Solving...\n" << endl;
		vector<array<char, 16>> inputPath = { inputState, rootLength };
		clock_t startTime = clock();
		vector<array<char, 16>> solution = aStar(aStarSecondRow(aStarFirstRow(inputPath)), solved);
		clock_t programTime = clock() - startTime;
		cout << "Solution:" << endl;
		cout << printPath(solution) << endl;
		cout << "Program took: " << (float) programTime / CLOCKS_PER_SEC << " seconds." << endl;
	}

	return 0;
}
