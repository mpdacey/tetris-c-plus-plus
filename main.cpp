#include <iostream>
using namespace std;

#include <Windows.h>

wstring tetromino[7];
int boardWidth = 10;
int boardHeight = 20;
unsigned char *board = nullptr;

int screenWidth = 120;
int screenHeight = 60;
int boardOffsetX = 2;
int boardOffsetY = 2;

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
	boardWidth += 2;
	boardHeight += 1;

	board = new unsigned char[(boardWidth) * (boardHeight)];
	for (int x = 0; x < boardWidth; x++)
		for (int y = 0; y < boardHeight; y++)
			board[y*boardWidth + x] = (x == 0 || x == boardWidth -1 || y == boardHeight - 1) ? 9 : 0;
}

wchar_t* DrawField(wchar_t* screen) {
	for (int x = 0; x < boardWidth; x++)
		for (int y = 0; y < boardHeight; y++)
			screen[(y + boardOffsetY) * screenWidth + x + boardOffsetX] = L" ABCDEFG=#"[board[y * boardWidth + x]];

	return screen;
}

int main() {

	InitialiseBoard();

	wchar_t* screen = new wchar_t[screenWidth*screenHeight];
	for (int i = 0; i < screenWidth*screenHeight; i++) screen[i] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD dwBytesWritten = 0;

	bool gameOverFlag = false;

	while(!gameOverFlag){
		screen = DrawField(screen);
		
		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}