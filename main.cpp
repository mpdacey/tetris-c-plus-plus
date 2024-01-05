#include <iostream>
using namespace std;

#include <Windows.h>

wstring tetrominos[7];
int boardWidth = 10;
int boardHeight = 20;
unsigned char *board = nullptr;

int screenWidth = 120;
int screenHeight = 60;
int boardOffsetX = screenWidth/2 - boardWidth/2 + 1;
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

void CreatePieceAssets() {
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");

	tetrominos[1].append(L"....");
	tetrominos[1].append(L".XXX");
	tetrominos[1].append(L"..X.");
	tetrominos[1].append(L"....");

	tetrominos[2].append(L"....");
	tetrominos[2].append(L"...X");
	tetrominos[2].append(L".XXX");
	tetrominos[2].append(L"....");

	tetrominos[3].append(L"....");
	tetrominos[3].append(L"X...");
	tetrominos[3].append(L"XXX.");
	tetrominos[3].append(L"....");

	tetrominos[4].append(L"....");
	tetrominos[4].append(L".XX.");
	tetrominos[4].append(L".XX.");
	tetrominos[4].append(L"....");

	tetrominos[5].append(L"....");
	tetrominos[5].append(L"XX..");
	tetrominos[5].append(L".XX.");
	tetrominos[5].append(L"....");

	tetrominos[6].append(L"....");
	tetrominos[6].append(L"..XX");
	tetrominos[6].append(L".XX.");
	tetrominos[6].append(L"....");
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

bool DoesPieceFit(int tetrominoIndex, int rotation, int posX, int posY) {
	for (int pieceX = 0; pieceX < 4; pieceX++) {
		for (int pieceY = 0; pieceY < 4; pieceY++) {
			int pieceIndex = RotatedIndex(pieceX, pieceY, rotation);
			int boardIndex = (posY + pieceY) * boardWidth + posX + pieceX;

			if (pieceX + posX > -1 && pieceX + posX < boardWidth &&
				pieceY + posY > -1 && pieceY + posY < boardHeight &&
				tetrominos[tetrominoIndex][pieceIndex] == L'X' && board[boardIndex] != 0)
				return false;
		}
	}

	return true;
}

int main() {
	CreatePieceAssets();
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