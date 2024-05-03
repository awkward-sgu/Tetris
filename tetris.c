﻿#include "tetris.h"

static struct sigaction act, oact;

int main() {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit = 1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris() {
	int i, j;

	for (j = 0; j < HEIGHT; j++)
		for (i = 0; i < WIDTH; i++)
			field[j][i] = 0;

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	nextBlock[2] = rand() % 7;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline() {
	int i, j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH * 2);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH * 2 + 10);
	printw("  NEXT BLOCK  ");
	DrawBox(3, WIDTH * 2 + 10, 4, 12);
	DrawBox(9, WIDTH * 2 + 10, 4, 12);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15, WIDTH * 2 + 10);
	printw("  SCORE");
	DrawBox(16, WIDTH * 2 + 10, 1, 12);
}

int GetCommand() {
	int command;
	command = wgetch(stdscr);
	switch (command) {
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command) {
	int ret = 1;
	int drawFlag = 0;
	switch (command) {
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
			blockRotate = (blockRotate + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
			blockX++;
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
			blockX--;
		break;
	case ' ':
			for(int shadowY = blockY; shadowY < HEIGHT; shadowY++)
				if(CheckToMove(field, nextBlock[0], blockRotate, shadowY + 1, blockX))
					ProcessCommand(KEY_DOWN);
		break;
	default:
		break;
	}
	if (drawFlag) DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	return ret;
}

void DrawField() {
	int i, j;
	for (j = 0; j < HEIGHT; j++) {
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++) {
			if (field[j][i]) {
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
			}
			else printw(". ");
		}
	}

	move(HEIGHT, WIDTH * 2 + 10); // added
}


void PrintScore(int score) {
	move(17, WIDTH * 2 + 11);
	printw("%12d", score);

	move(HEIGHT, WIDTH * 2 + 10); // added
}

void DrawNextBlock(int* nextBlock) {
	int i, j;
	for (i = 0; i < 4; i++) {
		move(4 + i, WIDTH * 2 + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[1]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
			}
			else printw("  ");
		}
	}
	for (i = 0; i < 4; i++) {
		move(10 + i, WIDTH * 2 + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
			}
			else printw("  ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, (j + x) * 2 + 1);
				attron(A_REVERSE);
				attron(COLOR_PAIR(0));
				printw("%c ", tile);
				attroff(A_REVERSE);
				attroff(COLOR_PAIR(0));
			}
		}

	move(HEIGHT, WIDTH * 2 + 10);
}

void DrawBox(int y, int x, int height, int width) {
	int i, j;
	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++) {
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play() {
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH - 10, 1, 20);
			move(HEIGHT / 2, WIDTH - 9);
			printw("     Good-bye!!     ");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH - 10, 1, 20);
	move(HEIGHT / 2, WIDTH - 9);
	printw("     GameOver!!     ");
	refresh();
	getch();
	newRank(score);
}

char menu() {
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (i + blockY < 0 || i + blockY >= HEIGHT)
					return 0;
				if (j + blockX < 0 || j + blockX >= WIDTH)
					return 0;
				if (f[i + blockY][j + blockX] == 1)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	int originalRotate = blockRotate;
	int originalY = blockY;
	int originalX = blockX;

	switch (command) {
	case KEY_UP:
		originalRotate = (blockRotate + 3) % 4;
		break;
	case KEY_DOWN:
		originalY--;
		break;
	case KEY_RIGHT:
		originalX--;
		break;
	case KEY_LEFT:
		originalX++;
		break;
	default:
		break;
	}
	DeleteBlock(originalY, originalX, currentBlock, originalRotate);
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig) {
	// user code

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		DeleteBlock(blockY, blockX, nextBlock[0], blockRotate);
		blockY++;
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	}
	else {
		if (blockY == -1)
			gameOver = 1;

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		PrintScore(score);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % NUM_OF_SHAPE;
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
		DrawNextBlock(nextBlock);
		DrawField();
	}
	timed_out = 0;

	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code

	int touched = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				f[i + blockY][j + blockX] = 1;
				if (i + blockY + 1 >= HEIGHT)
					touched++;
				else if (f[i + blockY + 1][j + blockX] == 1)
					touched++;
			}
		}
	}

	return touched * 10;

	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	// user code

	int num_line = 0;

	int check;
	for (int i = 0; i < HEIGHT; i++) {
		check = 1;
		for (int j = 0; j < WIDTH; j++) {
			if (!f[i][j]) {
				check = 0;
				break;
			}
		}
		if (check) {
			num_line++;
			for (int i1 = i; i1 > 0; i1--) {
				for (int j1 = 0; j1 < WIDTH; j1++) {
					f[i1][j1] = f[i1 - 1][j1];
				}
			}
			for (int j1 = 0; j1 < WIDTH; j1++) {
				f[0][j1] = 0;
			}
		}
	}

	return num_line * num_line * 100;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID, int blockRotate) {
	// user code

	if (SHADOW_ON) {
		int shadowY;
		for (shadowY = y; shadowY < HEIGHT; shadowY++) {
			if (!CheckToMove(field, blockID, blockRotate, shadowY + 1, x)) {
				break;
			}
		}

		if (shadowY - y >= 4)
			DrawBlock(shadowY,x,blockID,blockRotate,'/');

	move(HEIGHT, WIDTH * 2 + 10);
	}

}

void DeleteBlock(int y, int x, int blockID, int blockRotate) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, (j + x) * 2 + 1);
				printw(". ");
			}
		}

	if (SHADOW_ON) {
		int shadowY;
		for (shadowY = y; shadowY < HEIGHT; shadowY++) {
			if (!CheckToMove(field, blockID, blockRotate, shadowY + 1, x)) {
				break;
			}
		}

		if (shadowY - y >= 4)
			for (i = 0; i < 4; i++)
				for (j = 0; j < 4; j++) {
					if (block[blockID][blockRotate][i][j] == 1 && i + shadowY >= 0) {
						move(i + shadowY + 1, (j + x) * 2 + 1);
						printw(". ");
					}
				}
	}
	move(HEIGHT, WIDTH + 10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	// user code

	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}



void createRankList() {
	// user code
}

void rank() {
	// user code
}

void writeRankFile() {
	// user code
}

void newRank(int score) {
	// user code
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
	// user code
}

int recommend(RecNode* root) {
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay() {
	// user code
}