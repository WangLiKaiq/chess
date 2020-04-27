#include "ChessBoard.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <fstream>
using namespace std;
#define DEBUG
ChessBoard::ChessBoard(int  offensiveMove, vector<vector<int>> temp) {
	turnFlag = offensiveMove;
	chessBoard = temp;// initialize the chessboard
	size = 0;
	searchLocation.resize(LAYER_NUMBER_OF_SEARCH);
	vector<int> tempV(CHESSBOARDSIZE, 0);
	for (int i = 0; i < CHESSBOARDSIZE; i++) {
		for (int j = 0; j < CHESSBOARDSIZE; j++) {
			tempV[j] = count(j, i); // count  pieces' number around center
			if (chessBoard[j][i] != 0)
				size++;//count the number of pieces
		}
		searchLocation[0].push_back(tempV);// initialize the first floor 
	}
	player[0] = new Player(RED_PLAYER);//initialize the player 
	player[1] = new Player(BLACK_PLAYER);
	historyValueOnPicesLocation.resize(LAYER_NUMBER_OF_SEARCH, 0);
	firstEntry.resize(LAYER_NUMBER_OF_SEARCH, true);
}
void ChessBoard::run() {
	begin();
#ifdef DEBUG_OK
	test();
#endif
	while (winnerFlag == 0) {
		if (size == CHESSBOARDSIZE * CHESSBOARDSIZE) {
			cout << "drawn game!" << endl;
		}// if  full board , declared a drawn result
		pair<int, int> location;
		if (turnFlag == RED_PART) {//game begin!
			cout << "retract false move in a chess game? Yes-->Y, No--->N" << endl;
			char retractFlag;
			cin >> retractFlag;
			if (retractFlag == 'Y') 
				location = retract();// whether need to retract
			else {
				try  {
				location = player[0]->moveInChess();	
				}
				catch (int) {// input error and resume load
					cout << "input error" << endl;
					location = player[0]->moveInChess();
				}
			}
		}
		else if (turnFlag == BLACK_PART) 
			location = smartMoveInChess();//find the optimal location 
		modifyData(location.first, location.second, turnFlag);
		try {
		moveInChess();
		}
		catch (domain_error a) {
			cout << a.what() << endl;
			cout << "resume load!" << endl;
			vector<int> temp = history.top();
			modifyData(temp[0], temp[1], temp[2]);
			turn();
			continue;
		}// input error
		turn();//take turns
		print();
	}
	end();
}
void ChessBoard::begin() {
	cout << "The game begin!" << endl;
}
void ChessBoard::end() {
	if (winnerFlag == BLACK_PART) {
		cout << "Red partly win!" << endl;
	}
	else {
		cout << "Black partly win!" << endl;
	}
}
void ChessBoard::moveInChess() {
	system("cls");
	if (chessBoard[locationY][locationX] == 0) {
		chessBoard[locationY][locationX] = turnFlag;
		if (judgeVictory(locationX, locationY, turnFlag))
			winnerFlag = turnFlag;
		if (turnFlag == RED_PART) 
			player[RED_PLAYER]->moveInChess( locationX, locationY);
		else if (turnFlag == BLACK_PART) 
			player[BLACK_PLAYER]->moveInChess(locationX, locationY); //player record the location of pieces
		pushInHistory(); // pieces order
		changeSearchLocationComponent(locationX, locationY, INPUT_PIECE);// change the values  around center
		size++;
#ifdef DEBUG_OK
		printSearchLocation();
#endif
	}
	else {
#ifdef DEBUG_OK
		print();
		printSearchLocation();
#endif
		domain_error illegalLocation("the location is illegal");
		throw illegalLocation;//don't have catch statement
	}// if typeincorrectly , prompt the player ot retype; throw any type of error
}
void ChessBoard::pushInHistory() {
	vector<int> pieceInformation(3);
	pieceInformation[LOCATION_X] = locationX;// record the x variable of last move
	pieceInformation[LOCATION_Y] = locationY;//record the y variable of last move
	pieceInformation[PART] = turnFlag;// the last player
	history.push(pieceInformation);
}
bool ChessBoard::judgeVictory(int x, int y, int turnFlagTemp) {
	if (maxContinuousNum(x, y, turnFlagTemp) >= 4)// max continous number of pieces over 4
		return  true;
	else 
		return  false;
}
void ChessBoard::print() {
	string median;
	for (int i = CHESSBOARDSIZE - 1; i >= 0; i--) {
		for (int k = CHESSBOARDSIZE; k > 0; k--)
			cout << "------";
		cout << endl;
		for (int j = 0; j < CHESSBOARDSIZE; j++) {
			cout << "|  ";
			if (j == locationX && i == locationY) {
				if (chessBoard[i][j] == RED_PART)
					median = "*@ ";
				else if (chessBoard[i][j] == BLACK_PART)
					median = "+@ ";
				else
					median = "   ";
			}
			else {
				if (chessBoard[i][j] == RED_PART)
					median = "*  ";
				else if (chessBoard[i][j] == BLACK_PART)
					median = "+  ";
				else median = "   ";
			}
			cout << median;
		}
		cout << "|" << endl;
	}
	for (int k = CHESSBOARDSIZE; k > 0; k--)
		cout << "------";
#ifdef DEBUG_OK
	printSearchLocation();
#endif
}
pair<int, int> ChessBoard::smartMoveInChess() {
	int alpha = INT_MIN;
	int beta = INT_MAX;
	alphaBeta(alpha, beta, 0, 5, BLACK_PART);
	return make_pair(locationX, locationY);
}
int ChessBoard::evaluate(int x, int y, int part) {
#ifdef DEBUG_OK
	if (x == 4 && y == 4) {
		cout << endl;
	}
#endif
	int result = 0;
	pair<int, int> pieceNumLineInformation = piecesNumInLine(x, y, part, NORTH_TO_SOUTH);// ours socre of NOTH_TO_SOUTH on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += friendProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];// enemy's score of NORTH_TO_SOUTH on this location

	pieceNumLineInformation = piecesNumInLine(x, y, FLIP(part), NORTH_TO_SOUTH);
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
                                        		cout << endl;
	}
#endif
	result += enemyProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, part, EAST_TO_WEST);//ours score of EAST_TO_WEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += friendProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, FLIP(part), EAST_TO_WEST);//enemy's score of EAST_TO_WEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += enemyProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, part, NORTHEAST_TO_SOUTHWEST);// ours score of NORTHEAST_TO_SOUTHWEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += friendProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, FLIP(part), NORTHEAST_TO_SOUTHWEST);//enemy's score of NORTHEAST_TO_SOUTHWEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += enemyProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, part, SOUTHEAST_TO_NORTHWEST);// ours score of SOUTHEAST_TO_NORTHWEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += friendProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

	pieceNumLineInformation = piecesNumInLine(x, y, FLIP(part), SOUTHEAST_TO_NORTHWEST);// enmey's score of SOUTHEAST_TO_NORTHWEST on this location
#ifdef DEBUG_OK
	if (pieceNumLineInformation.first == 5 || pieceNumLineInformation.second == 5) {
		cout << endl;
		print();
		cout << endl;
	}
#endif
	result += enemyProportion[pieceNumLineInformation.first][pieceNumLineInformation.second];

#ifdef DEBUG_OK
	cout << "(x,  y) = (" << x << ", " << y << ")" <<  endl;
	cout << "result = " << result << endl;
#endif
	return result;
}
ChessBoard::~ChessBoard() {
	//delete[]player;
	delete player[1];
	delete player[0];
}
void ChessBoard::modifyData(int x, int y, int turnFlagTemp) {
	locationX = x;
	locationY = y;
	turnFlag = turnFlagTemp;
}
int ChessBoard::maxContinuousNum(int x, int y, int turnFlagTemp) {
	int maxContinuousPiecesNum = 0;
	int tempNum;
	tempNum = piecesNumInLine(x, y, turnFlagTemp, NORTH_TO_SOUTH).first;
	if (tempNum > maxContinuousPiecesNum)
		maxContinuousPiecesNum = tempNum;
	tempNum = piecesNumInLine(x, y, turnFlagTemp, EAST_TO_WEST).first;
	if (tempNum > maxContinuousPiecesNum)
		maxContinuousPiecesNum = tempNum;
	tempNum = piecesNumInLine(x, y, turnFlagTemp, NORTHEAST_TO_SOUTHWEST).first;
	if (tempNum > maxContinuousPiecesNum)
		maxContinuousPiecesNum = tempNum;
	tempNum = piecesNumInLine(x, y, turnFlagTemp, SOUTHEAST_TO_NORTHWEST).first;
	if (tempNum > maxContinuousPiecesNum)
		maxContinuousPiecesNum = tempNum;
#ifdef DEBUG_OK
	cout << "(locationX,  locationY) = (" <<locationX  << ", " << locationY << ")" << endl;
	cout << maxContinuousPiecesNum;
#endif 
	return maxContinuousPiecesNum;
}
pair<int, int> ChessBoard::retract() {
	int last, nextToLast;
	if (turnFlag == RED_PART) {
		last = BLACK_PLAYER;
		nextToLast = RED_PLAYER;
	}
	else {
		last =RED_PLAYER;
		nextToLast = BLACK_PLAYER;
	}
	vector<int> pieceInformation = history.top();
	history.pop();// remove the last move
	chessBoard[pieceInformation[LOCATION_Y]][pieceInformation[LOCATION_X]] = BLANK_PART; // remove the last piece 
	player[last]->pop();//remove the last step
	pieceInformation = history.top();
	history.pop();// remove the next-to-last move
	chessBoard[pieceInformation[LOCATION_Y]][pieceInformation[LOCATION_X]] = BLANK_PART;// remove the next-to-last peice
	player[nextToLast]->pop();// remove the last step
	pair<int, int> location;
	location = player[nextToLast]->moveInChess();// resume load
	return location;
}
pair<int, int> ChessBoard::piecesNumInLine(int x, int y, int turnFlagTemp, int orientation) {
	int tempNum = 0;
	int emptyNum = 0;
	int tempX = x + xIncrement[orientation];
	int tempY = y + yIncrement[orientation];
	while (tempX >= 0 && tempX < CHESSBOARDSIZE && tempY >= 0 && tempY < CHESSBOARDSIZE && turnFlagTemp == chessBoard[tempY][tempX]) {
		tempNum++;
		tempX += xIncrement[orientation];
		tempY += yIncrement[orientation];
	}//count continous pieces' number on side
	if (tempX >= 0 && tempX < CHESSBOARDSIZE && tempY >= 0 && tempY < CHESSBOARDSIZE && chessBoard[tempY][tempX] == BLANK_PART)
		emptyNum++;//whether is empty on side
	tempX = x + FLIP(xIncrement[orientation]);
	tempY = y + FLIP(yIncrement[orientation]);
	while (tempX >= 0 && tempX < CHESSBOARDSIZE && tempY >= 0 && tempY < CHESSBOARDSIZE && turnFlagTemp == chessBoard[tempY][tempX]) {
		tempNum++;
		tempX += FLIP(xIncrement[orientation]);
		tempY += FLIP(yIncrement[orientation]);
	}//count continous pieces' number on other side
	if (tempX >= 0 && tempX < CHESSBOARDSIZE && tempY >= 0 && tempY < CHESSBOARDSIZE && chessBoard[tempY][tempX] == BLANK_PART)
		emptyNum++;//whether is empty on other side
#ifdef DEBUG_OK
	cout << "(x,  y) = (" << x << ", " << y << ")" << endl;
	cout << "the number of pieces in the ";
	switch (orientation) {
	case 0:
		cout << "NORTHTOSOUTH" << endl;
		break;
	case 1:
		cout << "EASTTOWEST" << endl;
		break;
	case 2:
		cout << "NORTHEASTTOSOUTHWEST" << endl;
		break;
	case 3:
		cout << "SOUTHEASTTONORTHWEST" << endl;
		break;
	default:
		break;
	}
	cout << ":  " << tempNum << endl;
	cout << "empty location at both ends:\t" << emptyNum << endl;
#endif
	return make_pair(tempNum, emptyNum);
}
void ChessBoard::changeSearchLocationComponent(int x, int y, int increment, int pilesNum /* = 0 */) {
	if (pilesNum > 0) {
		if (firstEntry[pilesNum]) {
			searchLocation[pilesNum] = searchLocation[pilesNum - 1];
			firstEntry[pilesNum] = false;
		}
	}// if firstEntry is true , duplicate the last floor's chessBoard,  
	if (increment == REMOVE_PIECE)// if remove from chessBoard
		searchLocation[pilesNum][y][x] = historyValueOnPicesLocation[pilesNum];//restore the last number of pieces around the center piece
	else {
		historyValueOnPicesLocation[pilesNum] = searchLocation[pilesNum][y][x];// record the number of pieces around the center piece
		searchLocation[pilesNum][y][x] = 0; // move the piece in the chess, set the number to zero
	}
	x += 1;
	if (x < CHESSBOARDSIZE) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;//modify the value in the east of center
	}
	x -= 2;
	if (x >= 0) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;//modify the value in the west of center
	}
	y -= 1;
	if (x >= 0 && y >= 0) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;//modify the value in the southwest of center
	}
	x += 2;
	if (x < CHESSBOARDSIZE && y >= 0) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;//modify the value in the southeast of center
	}
	y += 2;
	if (x < CHESSBOARDSIZE && y < CHESSBOARDSIZE) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;//modify the value in the northeeast of center
	}
	if (--x, y < CHESSBOARDSIZE) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;// modify the value in the north of center
	}
	if (--x >= 0 && y < CHESSBOARDSIZE) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;// modify the value in the northwest of center
	}
	y -= 2;
	if (++x, y >= 0) {
		if (chessBoard[y][x] == 0)
			searchLocation[pilesNum][y][x] += increment;// modify the value in the south of center
	}
}
int ChessBoard::count(int x, int y) {
	if (chessBoard[y][x] != 0)
		return 0;
	int num = 0;
	x += 1;
	if (x < CHESSBOARDSIZE)
		if (chessBoard[y][x] != 0)
			num++;// determine if there is a piece to the east of the center
	x -= 2;
	if (x >= 0)
		if (chessBoard[y][x] != 0)
			num++;//determine if there is a piece to the west of the center
	y -= 1;
	if (x >= 0 && y >= 0)
		if (chessBoard[y][x] != 0)
			num++;//determine if there is a piece to the southwest  of the center 
	x += 2;
	if (x < CHESSBOARDSIZE && y >= 0)
		if (chessBoard[y][x] != 0)
			num++;//determine if there is a piece to the southeast of the center  
	y += 2;
	if (x < CHESSBOARDSIZE && y < CHESSBOARDSIZE)
		if (chessBoard[y][x] != 0)
			num++;//determine if there is a piece to the northeeast  of the center 
	if (--x, y < CHESSBOARDSIZE)
		if (chessBoard[y][x] != 0)
			num++;// determine if there is a piece to the  north  of the center
	if (--x >= 0 && y < CHESSBOARDSIZE)
		if (chessBoard[y][x] != 0)
			num++;// determine if there is a piece to the northwest of the center 
	y -= 2;
	if (++x, y >= 0)
		if (chessBoard[y][x] != 0)
			num++;// determine if there is a piece to the south  of the center
	return num;
}
void ChessBoard::printSearchLocation(int piliesNum /* = 0*/) {
	cout << endl;
	for (int i = CHESSBOARDSIZE - 1; i >= 0; i--) {
		for (int j = 0; j < CHESSBOARDSIZE; j++)
			cout << searchLocation[piliesNum][i][j] << "\t";
		cout << endl;
	}
}
void ChessBoard::simulatePushPop(int x, int y, int turnFlagTemp, int increment, int pilesNum /* = 0 */) {
	if (increment == INPUT_PIECE) 
		chessBoard[y][x] = turnFlagTemp;//simulate that put piece in chessBoard
	else
		chessBoard[y][x] = 0;//simulate that remove the piece from chessBoard
	//turn();//  don't move in chess but reversed it once
	changeSearchLocationComponent(x, y, increment, pilesNum);
#ifdef DEBUG_OK
	printSearchLocation();
	//print();
#endif
}
//int ChessBoard::game(int alpha, int beta, int pilesNum) {
//	pilesNum++;
//	int value;
//	if (pilesNum == 4) {
//		for (int i = 0; i < CHESSBOARDSIZE; i++) {
//			for (int j = 0; j < CHESSBOARDSIZE; j++) {
//				value = evaluate(i, j, );
//				if (value > beta) {
//					beta = value;
//				}
//			}
//		}
//		return beta;
//	}//recursive base
//	map <pair<int, int>, int> orderSearchPath;
//	findOrderSearchPath(orderSearchPath);
//	if (orderSearchPath.size() == 0) {
//		locationX = 5;
//		locationY = 5;
//		return INT_MAX;
//	}
//	if (pilesNum % 2 == 0) {
//		beta = min(alpha, beta, pilesNum, orderSearchPath);
//		return beta;
//	}
//	else {
//		alpha = max(alpha, beta, pilesNum, orderSearchPath);
//		return alpha;
//	}
//}
//int ChessBoard::max(int &alpha, int &beta, int pilesNum, map <pair<int, int>, int>& orderSearchPath) {
//	int i = 0;
//	int tempI = 0;
//	int tempAlpha = alpha;
//#ifdef DEBUG_NO
//	printSearchLocation();
//#endif
//	for (auto ptr = orderSearchPath.begin(); ptr != orderSearchPath.end(); ++ptr, i++) {
//		simulatePushPop(ptr->first.first, ptr->first.second, turnFlag, INPUT_PIECE);
//		if (judgeVictory(ptr->first.first, ptr->first.second,turnFlag)) {
//			simulatePushPop(ptr->first.first, ptr->first.second, turnFlag, REMOVE_PIECE);
//			return 30000;
//		}
//		
//#ifdef DEBUG_NO
//		print();
//#endif
//		tempAlpha = game(alpha, beta, pilesNum);
//		if (tempAlpha > alpha) {
//			alpha = tempAlpha;
//			tempI = i;
//		}
//		simulatePushPop(ptr->first.first, ptr->first.second, turnFlag, REMOVE_PIECE);
//#ifdef DEBUG_NO
//		print();
//#endif
//	}
//	if (pilesNum == 1) {
//		auto ptr = orderSearchPath.begin();
//		advance(ptr, tempI);
//		locationX = ptr->first.first;
//		locationY = ptr->first.second;
//	}
//	return alpha;
//}
//int ChessBoard::min(int &alpha, int &beta, int pilesNum, map <pair<int, int>, int> & orderSearchPath) {
//	int tempBeta = beta;
//	for (auto ptr = orderSearchPath.begin(); ptr != orderSearchPath.end(); ++ptr) {
//		simulatePushPop(ptr->first.first, ptr->first.second, FLIP(	turnFlag), INPUT_PIECE);
//		if (judgeVictory(ptr->first.first, ptr->first.second, FLIP(turnFlag))) {
//			simulatePushPop(ptr->first.first, ptr->first.second, FLIP(turnFlag), REMOVE_PIECE);
//			return 10000;
//		}
//
//#ifdef DEBUG_NO
//		print();
//#endif
//		tempBeta = game(alpha, beta, pilesNum);
//		if (tempBeta < beta) 
//			beta = tempBeta;
//		simulatePushPop(ptr->first.first, ptr->first.second, FLIP(turnFlag), REMOVE_PIECE);
//#ifdef DEBUG_NO
//		print();
//#endif
//	}
//	return beta;
//}
void ChessBoard::findOrderSearchPath(vector <pair< pair<int, int>, int>>& orderSearchPath, int part, int pilesNum /* = 0*/) {
	int value;
	for (int i = 0; i < CHESSBOARDSIZE; i++) {
		for (int j = 0; j < CHESSBOARDSIZE; j++) {
			if (searchLocation[pilesNum][j][i] != 0) {
				value = evaluate(i, j, part);
				orderSearchPath.push_back(make_pair(make_pair(i, j),value));
			}
		}
	}
	sort(orderSearchPath.begin(), orderSearchPath.end(), CompareP());// order the vector by score for  pruning
}
int ChessBoard::alphaBeta(int alpha, int beta, int depth, int maxDepth, int part) {
	if (depth > 0) {
		if (judgeVictory(simulateX, simulateY, FLIP(part))) {
			return   evaluate(simulateX, simulateY, FLIP(part)) * FLIP(part);
		}
	}// if victory in advanc, return the score
	int count;
	vector <pair< pair<int, int>, int>> orderSearchPath;
	findOrderSearchPath(orderSearchPath, part);
	if (orderSearchPath.size() == 0) {
#ifdef DEBUG_OK
		print();
		printSearchLocation();
#endif
		locationX = 5;
		locationY = 5;
		return INT_MAX;
	}// for the first step
	if (depth == maxDepth) {
		if (part == RED_PART)
			return orderSearchPath[orderSearchPath.size() - 1].second * FLIP(RED_PART);// return alpha
		else 
			return orderSearchPath.begin()->second;// return beta
	}//recursive base
	firstEntry[depth + 1] = true;
	if (part == BLACK_PART) {
		for (auto ptr = orderSearchPath.begin(); ptr != orderSearchPath.end(); ++ptr) {
			simulateX = ptr->first.first;
			simulateY = ptr->first.second;// update simulation value
			simulatePushPop(ptr->first.first, ptr->first.second, part, INPUT_PIECE, depth + 1);// simulate putting piece in chessBoard
			count = -alphaBeta(-beta, -alpha, depth + 1, maxDepth, FLIP(part));// receive the score of the simulation location
			if (count > alpha) {
				alpha = count;// update ahpha, search the max value
				if (depth == 0) {
#ifdef DEBUG_OK
					printSearchLocation();
#endif
					locationX = ptr->first.first;
					locationY = ptr->first.second;
				}// update location of next step
			}
			simulatePushPop(ptr->first.first, ptr->first.second, part, REMOVE_PIECE, depth + 1);// simulate removing piece from chessBoard
		}
		return alpha;	
	}
	else if (part == RED_PART) {
		int max = INT_MIN;
		for (auto ptr = orderSearchPath.begin(); ptr != orderSearchPath.end(); ++ptr) {
			simulateX = ptr->first.first;
			simulateY = ptr->first.second;// update simulation value
			simulatePushPop(ptr->first.first, ptr->first.second, part, INPUT_PIECE, depth + 1);// simulate putting piece in chessBoard
			count = -alphaBeta(-beta, -alpha, depth + 1, maxDepth, FLIP(part));// receive the score of simulation location
			if (count > max)
				max = count;// search min value
			if (count >= beta) {//prunin
				simulatePushPop(ptr->first.first, ptr->first.second, part, REMOVE_PIECE, depth + 1);// simulate removing piece from chessBoard
				return count;
			}
			simulatePushPop(ptr->first.first, ptr->first.second, part, REMOVE_PIECE, depth + 1);
		}
		return max;
	}
}
//int AlphaBeta(int depth, int alpha, int beta) {
//	if (depth == 0) {
//		return Evaluate();
//	}
//	GenerateLegalMoves();
//	while (MovesLeft()) {
//		MakeNextMove();
//		val = -AlphaBeta(depth - 1, -beta, -alpha);
//		UnmakeMove();
//		if (val >= beta) {
//			return beta;
//		}
//		if (val > alpha) {
//			alpha = val;
//		}
//	}
//	return alpha;
//}
void ChessBoard::test() {
	simulatePushPop(5, 5, RED_PART, INPUT_PIECE);
	print();
	printSearchLocation();
	simulatePushPop(4, 4, BLACK_PART, INPUT_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(3, 3, RED_PART, INPUT_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(2, 2, BLACK_PART, INPUT_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(1, 1, RED_PART, INPUT_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(1, 1, RED_PART, REMOVE_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(2, 2, BLACK_PART, REMOVE_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(3, 3, RED_PART, REMOVE_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(4, 4, BLACK_PART, REMOVE_PIECE);
	system("sys");
	print();
	printSearchLocation();
	simulatePushPop(5, 5, RED_PART, REMOVE_PIECE);
	system("sys");
	print();
	printSearchLocation();
}//test function
