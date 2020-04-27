#pragma once
#include <stack>
#include <vector>
#include "ChessBoard.h"
#include <fstream>
using namespace std;
class Player {
public:
	Player() {}
	Player(bool sideFlag);
	~Player() {}
	Player(bool sideFlag , vector<pair<int, int>> initializationLayout);
	pair<int, int > moveInChess(int x = -1, int y = -1);// input the location of chess
	void push(int x, int y) {
		pieces.push(make_pair(x, y));
	}
	void pop() {
		pieces.pop();
	}
	pair<int, int> top() {
		return pieces.top();
	}
private:
	int sideValue;
	stack<pair<int, int>> pieces;

};

