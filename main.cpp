#include <iostream>
using namespace std;

wstring tetromino[7];
int boardWidth = 10;
int boardHeight = 20;
unsigned char *board = nullptr;

int RotatedIndex(int pieceX, int pieceY, int rotation){
	switch(rotation % 4){
		case 0: return pieceY * 4 + pieceX;
		case 1: return 12 + pieceY - pieceX * 4;
		case 2: return 15 - pieceY * 4 - pieceX;
		case 3: return 3 - pieceY + pieceX * 4;
	}

	return 0;
}

void InitialiseBoard(){
	board = new unsigned char[(boardWidth+2) * (boardHeight+1)];
	for (int x = 0; x < boardWidth+2; x++)
		for (int y = 0; y < boardHeight+1; y++)
			board[y*boardWidth + x] = (x == 0 || x == boardWidth + 1 || y == boardHeight) ? 9 : 0;
}

int main() {

	InitialiseBoard();

	return 0;
}