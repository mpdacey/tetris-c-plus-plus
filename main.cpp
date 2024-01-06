#include <iostream>
#include <thread>
using namespace std;

#include <Windows.h>
#include <vector>
#include <cstdlib>

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
	tetrominos[0].append(L"....");
	tetrominos[0].append(L"XXXX");
	tetrominos[0].append(L"....");
	tetrominos[0].append(L"....");

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

wchar_t* DrawActivePiece(wchar_t* screen, int currentPiece, int currentRotation, int currentX, int currentY) {
	for (int pieceX = 0; pieceX < 4; pieceX++)
		for (int pieceY = 0; pieceY < 4; pieceY++)
			if (tetrominos[currentPiece][RotatedIndex(pieceX, pieceY, currentRotation)] == L'X')
				screen[(currentY + pieceY + boardOffsetY) * screenWidth + currentX + pieceX + boardOffsetX] = currentPiece + 65;

	return screen;
}

unsigned char* LockPieceIntoBoard(unsigned char* board, int currentPiece, int currentRotation, int currentX, int currentY) {
	for (int pieceX = 0; pieceX < 4; pieceX++)
		for (int pieceY = 0; pieceY < 4; pieceY++)
			if (tetrominos[currentPiece][RotatedIndex(pieceX, pieceY, currentRotation)] == L'X')
				board[(currentY + pieceY) * boardWidth + currentX + pieceX] = currentPiece + 1;

	return board;
}

unsigned char* FindLinesInBoard(unsigned char* board, int currentY, vector<int>* lines) {
	for (int pieceY = 0; pieceY < 4; pieceY++)
		if (currentY + pieceY < boardHeight - 1) {
			bool line = true;
			for (int x = 1; x < boardWidth - 1; x++)
				line &= board[(currentY + pieceY) * boardWidth + x] != 0;

			if (line)
			{
				for (int x = 1; x < boardWidth - 1; x++)
					board[(currentY + pieceY) * boardWidth + x] = 8;

				(*lines).push_back(currentY + pieceY);
			}
		}

	return board;
}

void ChooseNextPiece(int* currentX, int* currentY, int* currentRotation, int* currentPiece) {
	*currentX = boardWidth / 2-2;
	*currentY = 0;
	*currentRotation = 0;
	*currentPiece = rand() * time(NULL) % 7;
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

	int currentPiece = 0;
	int currentRotation = 0;
	int currentX = 0;
	int currentY = 0;

	bool inputKeys[5];
	bool rotateHold = false;

	int speed = 20;
	int speedCounter = 0;
	bool forceDown = false;

	vector<int> lines;
	int linesCleared = 0;
	int levelUpThreshold = 10;

	int score = 0;

	ChooseNextPiece(&currentX, &currentY, &currentRotation, &currentPiece);

	while(!gameOverFlag){
		this_thread::sleep_for(50ms);
		speedCounter++;
		forceDown = speedCounter == speed;

		for (int key = 0; key < 5; key++)
			inputKeys[key] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28ZX"[key]))) != 0;

		if (inputKeys[0] && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) currentX++;
		if (inputKeys[1] && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) currentX--;
		if (inputKeys[2] && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) {
			currentY++;
			score++;
		}
		if (!rotateHold && inputKeys[3] && DoesPieceFit(currentPiece, currentRotation - 1 + 4, currentX, currentY)) {
			currentRotation--;
			if (currentRotation < 0)
				currentRotation += 4;
		}
		if (!rotateHold && inputKeys[4] && DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) currentRotation++;

		rotateHold = inputKeys[3] || inputKeys[4];

		if (forceDown) {
			speedCounter = 0;
			if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) currentY++;
			else
			{
				board = LockPieceIntoBoard(board, currentPiece, currentRotation, currentX, currentY);
				board = FindLinesInBoard(board, currentY, &lines);
				ChooseNextPiece(&currentX, &currentY, &currentRotation, &currentPiece);
				gameOverFlag = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);
			}
		}

		screen = DrawField(screen);
		screen = DrawActivePiece(screen, currentPiece, currentRotation, currentX, currentY);
		swprintf_s(&screen[2 * screenWidth + boardWidth + 6], 16, L"SCORE: %8d", score);

		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);

		if (!lines.empty()) {
			this_thread::sleep_for(400ms);
			score += (1<< lines.size())*100;

			for (auto& v : lines) {
				for (int x = 1; x < boardWidth - 1; x++) {
					for (int y = v; y > 0; y--)
						board[y * boardWidth + x] = board[(y - 1) * boardWidth + x];
					board[x] = 0;
				}
				linesCleared++;
			}

			lines.clear();

			if (linesCleared >= levelUpThreshold) {
				linesCleared = 0;
				speed -= speed == 1 ? 0 : 1;
			}
		}
	}

	CloseHandle(console);
	cout << "GAME OVER! Final Score:" << score << endl;
	system("pause");

	return 0;
}