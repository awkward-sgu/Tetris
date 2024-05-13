#include "tetris.h"

static struct sigaction act, oact;

int main() {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	createRankList();

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit = 1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	rankfree();
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
	DrawBox(0, 0, HEIGHT, MODIFIED_WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, MODIFIED_WIDTH + 10);
	if(WIDE_BLOCK)
		printw("  ");
	printw("NEXT BLOCK");
	DrawBox(3, MODIFIED_WIDTH + 10, 4, 8 + 4 * WIDE_BLOCK);
	if(SHOW_NEXT2)
		DrawBox(9, MODIFIED_WIDTH + 10, 4, 8 + 4 * WIDE_BLOCK);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9 + 6 * SHOW_NEXT2, MODIFIED_WIDTH + 10);
	if(WIDE_BLOCK)
		printw(" ");
	printw(" SCORE");
	DrawBox(10 + 6 * SHOW_NEXT2, MODIFIED_WIDTH + 10, 1, 8 + 4 * WIDE_BLOCK);
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
				if (field[j][i] == 1) {
					attron(A_REVERSE);
					printw(" ");
					if(WIDE_BLOCK)
						printw(" ");
					attroff(A_REVERSE);
				}
				else {
					printw(".");
					if(WIDE_BLOCK)
						printw(" ");
				}
			}
		}

		move(HEIGHT, MODIFIED_WIDTH + 10); // added
}


void PrintScore(int score) {
	move(11 + 6 * SHOW_NEXT2, MODIFIED_WIDTH + 11);
	if(WIDE_BLOCK)
		printw("    ");
	printw("%8d", score);

	move(HEIGHT, MODIFIED_WIDTH + 10); // added
}

void DrawNextBlock(int* nextBlock) {
	int i, j;
	for (i = 0; i < 4; i++) {
		move(4 + i, MODIFIED_WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[1]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				if(WIDE_BLOCK)
					printw(" ");
				attroff(A_REVERSE);
			}
			else {
				printw(" ");
				if(WIDE_BLOCK)
						printw(" ");
			}
		}
	}
	if(SHOW_NEXT2){
		for (i = 0; i < 4; i++) {
			move(10 + i, MODIFIED_WIDTH + 13);
			for (j = 0; j < 4; j++) {
				if (block[nextBlock[2]][0][i][j] == 1) {
					attron(A_REVERSE);
					printw(" ");
					if(WIDE_BLOCK)
						printw(" ");
					attroff(A_REVERSE);
				}
				else {
					printw(" ");
					if(WIDE_BLOCK)
						printw(" ");
				}
			}
		}
	}

}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				if(WIDE_BLOCK)
					move(i + y + 1, (j + x) * 2 + 1);
				else
					move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				if(WIDE_BLOCK)
					printw(" ");
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT, MODIFIED_WIDTH + 10);
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
			if(WIDE_BLOCK){
				DrawBox(HEIGHT / 2 - 1, WIDTH - 10, 1, 20);
				move(HEIGHT / 2, WIDTH - 9);
				printw("     Good-bye!!     ");
			}
			else{
				DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
				move(HEIGHT / 2, WIDTH / 2 - 4);
				printw("Good-bye!!");
			}
			move(HEIGHT, MODIFIED_WIDTH + 10); // added
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	if(WIDE_BLOCK){
		DrawBox(HEIGHT / 2 - 1, WIDTH - 10, 1, 20);
		move(HEIGHT / 2, WIDTH - 9);
		printw("     GameOver!!     ");
	}
	else{
		DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
		move(HEIGHT / 2, WIDTH / 2 - 4);
		printw("GameOver!!");
	}
	move(HEIGHT, MODIFIED_WIDTH + 10); // added
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
		int check = 0;
		if(blockY == -2)
			check = 1;

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		PrintScore(score);

		if(!check)
			for(int j = 0; j < WIDTH; j++)
				if(field[0][j]){
					check = 1;
					break;
				}
		if (check)
			gameOver = 1;

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % NUM_OF_SHAPE;
		blockRotate = 0;
		blockY = -2; // block does not show here, but next time
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
			if (f[i][j] != 1) {
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
			DrawBlock(shadowY, blockX, blockID, blockRotate, '/');
	}

}

void DeleteBlock(int y, int x, int blockID, int blockRotate) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				if(WIDE_BLOCK)
					move(i + y + 1, (j + x) * 2 + 1);
				else
					move(i + y + 1, j + x + 1);
				printw(".");
				if(WIDE_BLOCK)
					printw(" ");
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
						if(WIDE_BLOCK)
							move(i + shadowY + 1, (j + x) * 2 + 1);
						else
							move(i + shadowY + 1, j + x + 1);
						printw(".");
						if(WIDE_BLOCK)
							printw(" ");
					}
				}
	}
	move(HEIGHT, MODIFIED_WIDTH + 10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	// user code

	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}



///////////////////////  2주차  ///////////////////////



void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i, j;
	score_number = 0;
	a = malloc(sizeof(Node));
	Node* curr = a;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");

	if(!fp){ // 파일이 없을 때 기본 파일 생성
		writeRankFile();
		fp = fopen("rank.txt", "r");
	}

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if (fscanf(fp, "%d", &j) != EOF) {
		if(j < 0)
			j = 0;
		if(j > MAX_NODE)
			j = MAX_NODE;
		
		for(i = 0; i < j; i++) {
			curr->link = malloc(sizeof(Node));
			curr = curr->link;
			fscanf(fp, "%16s %d", curr->name, &curr->score);
			
			score_number++;
		}
		curr->link = NULL;
		// 수정 : 맨 첫 줄에 목록의 개수가 나오기 때문에 개수만큼 입력받음
	}
	else {
		return;
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=score_number, ch, i, j;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력

	Node* curr = a;

	if (ch == '1') {
		printw("X: ");
		readnumber(&X, 2);
		printw("Y: ");
		readnumber(&Y, 2);

		printw("       name       |   score\n");
		printw("------------------------------\n");

		if(X == -1)
			X = 1;
		if(Y == -1)
			Y = score_number;

		if(X < 1)
			X = 1;
		if(Y < 1)
			Y = 1;

		if(Y > score_number)
			Y = score_number;
		
		if(score_number && X <= Y) {
			for(i = 0; i < X; i++) {
				curr = curr->link;
			}
			for(; i <= Y; i++) {
				printw("%-16s  | %d\n", curr->name, curr->score);
				curr = curr->link;
			}
		}
		else {
			printw("\nsearch failure: no rank in the list\n");
		}

	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		printw("Input the name: ");
		readstring(str, NAMELEN);

		printw("       name       |   score\n");
		printw("------------------------------\n");
		for(int i = 0; i < score_number; i++) {
			curr = curr->link;
			if(!strcmp(str, curr->name)) {
				printw("%-16s  | %d\n", curr->name, curr->score);
				check = 1;
			}
		}

		if(!check){
			printw("\nsearch failure: no name in the list\n");
		}

	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		Node* prev = NULL;
		int num;
		printw("Input the rank: ");
		readnumber(&X, 2);

		if(X < 1 || X > score_number) {
			printw("\nsearch failure: the rank not in the list\n");
		}
		else {
			for(int i = 0; i < X; i++){
				prev = curr;
				curr = curr->link;
			}
			prev->link = curr->link;
			free(curr);
			score_number--;
			writeRankFile();
			printw("\nresult: the rank deleted\n");
		}
	}
	else
		return;
	getch(); // 아무 키를 누를 시 종료

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int i;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);


	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node* curr = a->link;
	for(i = 0; i < score_number; i++){
		fprintf(fp, "%s %d\n", curr->name, curr->score);
		curr = curr->link;
	}
	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	printw("your name: ");
	readstring(str, NAMELEN);
	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가 CHILDREN_MAX를 넘어서면 꼴찌 없앰
	Node* newnode = malloc(sizeof(Node));
	for(i = 0; i < NAMELEN + 1; i++)
		newnode->name[i] = str[i];
	newnode->score = score;


	Node* curr = a;

	for(j = 0; j < score_number; j++){
		if(newnode->score > curr->link->score)
			break;
		curr = curr->link;
	}

	newnode->link = curr->link;
	curr->link = newnode;
	score_number++;

	if(score_number > MAX_NODE){
		for(i = j; i < score_number; i++){
			curr = curr->link;
		}
		Node* temp = curr;
		curr->link = NULL;
		free(temp);
		score_number--;
	}

	writeRankFile();
}

void readnumber(int* num, int size){
	if(size <= 0)
		return;

	int ch;
	char* buffer = malloc(size);
	buffer[0] = 0;
	int i = 0;
	int n = 0;
	while(1){
		ch = wgetch(stdscr);
		if('0' <= ch && ch <= '9'){
			if(i < size) {
				printw("%c", ch);
				buffer[i++] = ch;
			}
		}
		else if(ch == '\n') {
			printw("\n");
			break;
		}
		else if(ch == KEY_BACKSPACE) {
			if(i > 0){
				i--;
				int y,x;
				getyx(stdscr,y,x);
				x--;
				move(y,x);
				printw(" ");
				move(y,x);
			}
		}
	}

	n = buffer[0] - '0';
	for(int j = 1; j < i; j++) {
		n *= 10;
		n += buffer[j] - '0';
	}
	*num = n;

	if(i == 0)
		*num = -1;
	free(buffer);
}

void readstring(char* str, int size){
	if(size <= 0)
		return;
	
	int ch;
	char* buffer = malloc(size + 1);
	int i = 0;
	while(1){
		ch = wgetch(stdscr);
		if(33 <= ch && ch <= 126){
			if(i < size) {
				printw("%c", ch);
				buffer[i++] = ch;
			}
		}
		else if(ch == '\n') {
			printw("\n");
			break;
		}
		else if(ch == KEY_BACKSPACE) {
			if(i > 0){
				i--;
				int y,x;
				getyx(stdscr,y,x);
				x--;
				move(y,x);
				printw(" ");
				move(y,x);
			}
		}
	}
	buffer[i++] = '\0';

	for(int j = 0; j < i; j++)
		str[j] = buffer[j];
	free(buffer);
}

void rankfree(){
	Node* curr = a;
	Node* temp;
	for(int i = 0; i <= score_number; i++){
		temp = curr;
		curr = curr->link;
		free(temp);
	}
}

//////////////////////    3주차    //////////////////////


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