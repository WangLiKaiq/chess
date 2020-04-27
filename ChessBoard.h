#pragma once
#include <vector>
#include <map>
#include <fstream>
#include "Player.h"
#include <stack>
#define RED_PART 1
#define BLACK_PART -1
#define BLANK_PART 0
#define RED_PLAYER 0
#define BLACK_PLAYER 1
#define LOCATION_X 0
#define LOCATION_Y 1
#define PART 2
#define CHESSBOARDSIZE 10
#define NORTH_TO_SOUTH 0
#define EAST_TO_WEST 1
#define NORTHEAST_TO_SOUTHWEST 2
#define SOUTHEAST_TO_NORTHWEST 3
#define FLIP(turnFlag) -turnFlag
#define INPUT_PIECE 1
#define REMOVE_PIECE -1
#define LAYER_NUMBER_OF_SEARCH 8 //
#define ALPHA_PART 1
#define BETA_PART -1
using std::vector;
using  std::stack;
using std::pair;
using std::map;
class ChessBoard {
public:
	friend class Player;
	ChessBoard( int offensiveMove, vector<vector<int>> temp = vector<vector<int >>(CHESSBOARDSIZE, vector<int>(CHESSBOARDSIZE, 0)));
	~ChessBoard();
	void run();
private:
	vector<vector<int>> friendProportion{
		{0, 0, 120},
		{0, 70, 720},
		{0, 300, 4320},
		{51000, 51000, 51000},
		{51000, 51000, 51000},
		{51000, 51000, 51000},
		{51000, 51000, 51000},
		{51000, 51000, 51000}
	}; // score arrary of ours pieces
	vector<vector<int>> enemyProportion{
		{0, 0, 50},
		{0, 30, 600},
		{0, 200, 3000},
		{30000, 30000, 30000},
		{51000, 51000, 51000},
		{51000, 51000, 51000},
		{51000, 51000, 51000},
		{51000, 51000, 51000}
	}; //score array of enemy's Pieces
	stack<vector<int>> gameHistory; //save the game information 
	vector<int> xIncrement{ 0, 1, 1, 1 };  // the x variable for the dirrerent directions
	vector<int> yIncrement{-1, 0, -1, 1}; // the y variable for the dirrerent directions
	stack<vector<int>> history; //save the certain history
	vector<vector<int>> chessBoard; 
	vector<vector<vector<int>>> searchLocation; //record the number of pieces around the center
	Player *player[2];// 0 -- corresponding to  red     1-- corresponding to black
	vector<int> historyValueOnPicesLocation; // record the  pieces' number around center that piece was deleted by the last operate
	vector<bool> firstEntry; // whether if  need to duplicate the last floor's chessBoard
	int simulateX; // the x variable for the next simulate move
	int simulateY;	// the y variable for the next simulate move
	int locationX; //the x variable for the next move 
	int locationY; // the y variable for the next move
	int winnerFlag = 0;//0 not winnner -- 1 red win -- -1 black win
	int turnFlag = 0;// 0 not defined -- 1 red  turn -- -1 black  turn
	int size; // the number of pieces on chessBoard
	void begin();
	void end();
	void print(); // print chessBoard
	void moveInChess();//place the chess in the chessBoard
	void removeFromChess(); //remoe the chess from the chessBoard
	bool judgeVictory(int x, int y, int turnFlagTemp);//determine if there is a winner
	int maxContinuousNum(int x, int y, int turnFlagTemp); // the number of max continous pieces
	pair<int, int> smartMoveInChess();// find the optimal location
	void turn() {
		modifyData(locationX, locationY, -turnFlag);
	}// change the game round
	int evaluate(int x, int y, int part);// calculate the score of the location to help find the optimal location
	int count(int x, int y); // count the number of piece around the center ,if one piece on the location return 0
	pair<int, int> piecesNumInLine(int x, int y, int turnFlag, int orientation); // the number of pieces on the road and both ends
	//first number of pieces on road     second the number of peices at both ends
	void changeSearchLocationComponent(int x, int y, int increment, int pilesNum = 0); // modify the value around the pieces
	void pushInHistory(); // record order that  pieces move  in the chessboard
	void modifyData(int x, int y, int turnFlag); // update the data of locationX, locationY and turnFlag
	void printSearchLocation(int pilesNum = 0); // print information of searchLocation(for DEBUG)
	//int game(int alpha, int beta, int pilesNum);
	void findOrderSearchPath(vector <pair< pair<int, int>, int>>& orderSearchPath, int part, int pilesNum = 0); 	// sort the data in descending order
	void simulatePushPop(int x, int y, int turnFlagTemp, int increment, int pilesNum = 0); //simulate push pieces in chessBoard or pup pieces form chessBoard
	int alphaBeta(int alpha, int beta, int depth, int maxDepth, int part); // find optimal location of next step
	pair<int, int> retract(); // retract a false move
	void test(); // for DEBUG
	// test printSearchLocation function
};
class CompareP {
public:
	bool operator() (pair<pair<int, int>, int>pP1, pair<pair<int, int>, int > pP2) {
		return pP1.second > pP2.second;
	}

};// class object that overloads the function operator for sort the overSearchPath
