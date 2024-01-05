#include <iostream>
using namespace std;

wstring tetromino[7];

int RotatedIndex(int xPos, int yPos, int rotation){
	switch(rotation % 4){
		case 0: return yPos * 4 + xPos;
		case 1: return 12 + yPos - xPos * 4;
		case 2: return 15 - yPos * 4 - xPos;
		case 3: return 3 - yPos + xPos * 4;
	}

	return 0;
}

int main() {
	return 0;
}