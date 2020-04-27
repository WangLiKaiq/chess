#include "Player.h"
#include <iostream>
#include <fstream>
#define DEBUG
Player::Player(bool sideFlag) {
	if (sideFlag == RED_PLAYER)
		sideValue = 1;
	else
		sideValue = -1;
}
Player::Player(bool sideFlag, vector<pair<int, int>> initializationLayout) {
	new (this)Player(sideFlag);
	for (auto ptr = initializationLayout.begin(); ptr != initializationLayout.end(); ptr++) {
		pieces.push(*ptr);
	}
}

pair<int, int> Player::moveInChess(int x /* = -1*/ ,int y/*" = -1*/) {
	if (x == -1 && y == -1) {
		cout << "input the location of chess" << endl;
		cin >> x >> y;
#ifdef DEBUG_OK
		if (x == 1 && y == 9)
			cout << endl;
#endif
		if (!cin.good() || x < 0 || y < 0 || x > CHESSBOARDSIZE || y > CHESSBOARDSIZE)  {
			int a = 0;
			throw a;
		}
	}
	push(x, y);
	return top();
}
