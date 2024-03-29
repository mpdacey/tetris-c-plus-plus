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
unsigned char* nextPreview = nullptr;

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

void InitialiseNextPieceBorder() {
	nextPreview = new unsigned char[6 * 6];
	for(int x = 0; x < 6; x++)
		for(int y = 0; y < 6; y++)
			nextPreview[y*6+x] = (x == 0 || x == 5 || y == 0 || y == 5) ? 9 : 0;
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

wchar_t* DrawNextPiece(wchar_t* screen, int nextPiece) {
	for (int pieceX = 0; pieceX < 4; pieceX++)
		for (int pieceY = 0; pieceY < 4; pieceY++)
			nextPreview[(pieceY + 1) * 6 + pieceX + 1] = (tetrominos[nextPiece][RotatedIndex(pieceX, pieceY, 0)] == L'X') ? nextPiece + 1 : 0;

	for (int x = 0; x < 6; x++)
		for (int y = 0; y < 6; y++)
			screen[(y + boardOffsetY +2) * screenWidth + x + boardOffsetX + boardWidth + 2] = L" ABCDEFG=#"[nextPreview[y * 6 + x]];

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

void SetPiece(int* currentX, int* currentY, int* currentRotation) {
	*currentX = boardWidth / 2 - 2;
	*currentY = 0;
	*currentRotation = 0;
}

int ChooseNextPiece(int randOffset = 0) {
	return abs((rand()+randOffset) * (int)time(NULL)) % 7;
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

void PrintCurrentHeld(wchar_t* screen, int heldPiece) {
	char pieceLetters[7] = { L'I', L'T', L'L', L'J', L'O', L'Z', L'S' };
	char nextPiece = (heldPiece == -1) ? L' ' : pieceLetters[heldPiece];

	swprintf_s(&screen[(boardHeight+2) * screenWidth + boardWidth + 5], 16, L"HELD: %9c", nextPiece);
}

int main() {
	CreatePieceAssets();
	InitialiseBoard();
	InitialiseNextPieceBorder();

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

	int nextPiece = 0;

	int heldPiece = -1;
	bool hasHeld = false;

	bool inputKeys[6];
	bool rotateHold = false;

	int speed = 21;
	int speedCounter = 0;
	bool forceDown = false;

	vector<int> lines;
	int linesCleared = 0;
	int levelUpThreshold = 10;

	int level = 0;
	int score = 0;

	currentPiece = ChooseNextPiece();
	nextPiece = ChooseNextPiece(time(NULL) * 2);
	SetPiece(&currentX, &currentY, &currentRotation);

	while(!gameOverFlag){
		this_thread::sleep_for(50ms);
		speedCounter++;
		forceDown = speedCounter == speed;

		for (int key = 0; key < 6; key++)
			inputKeys[key] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28ZXC"[key]))) != 0;

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

		if (!hasHeld && inputKeys[5]) {
			if (heldPiece == -1) {
				heldPiece = currentPiece;
				currentPiece = nextPiece;
				nextPiece = ChooseNextPiece();
			}
			else {
				int temp = currentPiece;
				currentPiece = heldPiece;
				heldPiece = temp;
			}

			SetPiece(&currentX, &currentY, &currentRotation);
			hasHeld = true;
		}

		if (forceDown) {
			speedCounter = 0;
			if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) currentY++;
			else
			{
				board = LockPieceIntoBoard(board, currentPiece, currentRotation, currentX, currentY);
				board = FindLinesInBoard(board, currentY, &lines);
				currentPiece = nextPiece;
				SetPiece(&currentX, &currentY, &currentRotation);
				nextPiece = ChooseNextPiece();
				gameOverFlag = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);
				hasHeld = false;
			}
		}

		screen = DrawField(screen);
		screen = DrawActivePiece(screen, currentPiece, currentRotation, currentX, currentY);
		screen = DrawNextPiece(screen, nextPiece);
		swprintf_s(&screen[1 * screenWidth + boardWidth + 4], 16, L"SCORE: %8d", score);
		swprintf_s(&screen[2 * screenWidth + boardWidth + 5], 16, L"LEVEL: %8d", level);
		PrintCurrentHeld(screen,heldPiece);

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
				level++;
				speed -= speed == 1 ? 0 : 2;
			}
		}
	}

	CloseHandle(console);
	cout << "GAME OVER! Final Score: " << score << endl;
	system("pause");

	return 0;
}