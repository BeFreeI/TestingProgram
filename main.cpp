#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "struct.cpp"
#include "fWork.cpp"

HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE); // ��� ���������� �������(���������� �������)

const int LEFT = 75, RIGHT = 77, DOWN = 80, UP = 72, ESC = 27, ENTER = 13; //��������� �������

const int Black = 0, White = 15; // ����� �������

void shapka_Y (const char **menu_this, int cur_oper, int size) { //����������� ������� ������ ����
	for (int i = 0; i < size; i++) {
			if (i == cur_oper) { // ���� ������� ������� ������� ��������
				SetConsoleTextAttribute (hstdout, (White << 4 | Black)); // ����� ����� ������ � �������
			}
			else {
				SetConsoleTextAttribute (hstdout, (Black << 4 | White));
			}
			printf ("%s\n", menu_this[i]);
		}
	SetConsoleTextAttribute (hstdout, (Black << 4 | White));
}

char* nameOfnTest (int n) { // ������� ��� ��������� �����
	WIN32_FIND_DATA FindFileData; // ��������� ������ � �����
	HANDLE hf = FindFirstFile ("./tests/*.txt", &FindFileData); // ����� ������� ����� � ����������
	int i = 0;
    if (hf != INVALID_HANDLE_VALUE) { // �������� �� ������������ ����
        do {
        	if (n == i) // ���� ��� ������ ��� ����
				return FindFileData.cFileName; // ���������� ��� ������
        	i++;
    	} while (FindNextFile(hf, &FindFileData)); // ���� ���� ��������� ����
        FindClose (hf); // ��������� ����������
    }
}

GAME* chuseDificalte (int n) { // �������������� ���� � ������
	GAME *game = (GAME*)malloc(sizeof(GAME));
	game->next = NULL;
	game->rightQuest = 0;
	game->nQuest = 0;
	game->mark = 0;
	strcpy(game->dificalt, nameOfnTest(n)); // ��������� ��� �����
	return game;
}

int randRange (int max, int min) { // ��������� ���������� ����� � ��������� [a, b]
	return rand() % (max - min + 1) + min;
}

QUESTION* getNextQuestion (FILE *ft, QUESTION *quest) {
	fgets(quest->question, sizeof(quest->question), ft); // ��������� ��� ���� ��� �������
	fgets(quest->true_answer, sizeof(quest->true_answer), ft);
	fgets(quest->answer1, sizeof(quest->answer1), ft);
	fgets(quest->answer2, sizeof(quest->answer2), ft);
	fgets(quest->answer3, sizeof(quest->answer3), ft);
	if (feof(ft)) // ���� ������� �� ����, �� ������ NULL
		return NULL;
	return quest;
}

USER* start (USER* user, GAME* game) { // ��� ����
	char str[strlen(game->dificalt) + strlen("./tests/")];
	strcpy (str, "./tests/");
	strcat (str, game->dificalt); // �������� ������ ���� � ���������� �����
	FILE *ft = fopen(str, "r"); // �������� ����� � ������
	QUESTION *cur = (QUESTION*)malloc(sizeof(QUESTION));
	char *answer[4];
	int nHelp = 3;
	cur = getNextQuestion (ft, cur);
	srand (time(NULL)); // �������� ��������������� ������������������ � �������� ������� � ��������
	for (cur; cur; cur = getNextQuestion (ft, cur)) { // ��������� ������� ���� �� ������ �� ����� �����
		game->nQuest++; // ������� ���-�� �������� � �����
		answer[0] = cur->true_answer; // ��������� �������� �������
		answer[1] = cur->answer1;
		answer[2] = cur->answer2;
		answer[3] = cur->answer3;
		for (int i = 0; i < 10; i++) { // ������������ ������
			int n1 = randRange(3, 0), n2 = randRange(3, 0);
			char *boof = answer[n1];
			answer[n1] = answer[n2];
			answer[n2] = boof;
		}
		system("cls");
		printf ("%s", cur->question);
		for (int i = 0; i < 4; i++) // ������� ������ �� �����
			printf ("%d. %s", i + 1, answer[i]);
		printf ("��� �����?\n");
		repid: ;
		int key = getch(); // ������ ����� ������
		if (key - '0' >= 1 && key - '0' <= 4) { // �������� �� ������������ �����
			if (!strcmp(answer[(key - '0') - 1], cur->true_answer)) // ��������� ������ �� �����
				game->rightQuest++;
		}
		else if (key == 'h' || key == '�') { // ���� ������������� ���������
			if (!nHelp) { // ��������� �������� �� ���������
				printf ("����������� ���������\n");
				goto repid;
			}
			nHelp--;
			system ("cls");
			printf ("%s", cur->question);
			int n;
			if (!randRange(1, 0)) { // ������ ����� ������ ������� ������
				answer[0] = cur->true_answer;
				do {
					n = randRange (3, 0);
				} while(!strcmp(answer[n], cur->true_answer));
				answer[1] = answer[n];
				printf ("1. %s2. %s", answer[0], answer[1]);
			}
			else { // ������ ����� �������� ������� ������
				do {
					n = randRange (3, 0);
				} while(!strcmp(answer[n], cur->true_answer));
				answer[0] = answer[n];
				answer[1] = cur->true_answer;
				printf ("1. %s2. %s", answer[0], answer[1]);
			}
			repidEase: ;
			key = getch();
			if (key - '0' >= 1 && key - '0' <= 2) { // ���� �������� �� ������������
				if (!strcmp(answer[(key - '0') - 1], cur->true_answer))
					game->rightQuest++;
			}
			else
				goto repidEase;
		}
		else // ���� ������ ������������ �����
			goto repid;
	}
	game->next = user->rez; // ��������� ��������� ����� � ���������� ������������
	user->rez = game;
	if (game->rightQuest == 0) // ���������� ������ ������������
		game->mark = 0;
	else
		game->mark = (float)game->rightQuest / (float)game->nQuest * 10;
	fclose(ft);
	free(cur);
	return user; // ���������� ������������
}

int sizeOfDir () { // ������ ���-�� ������ � ����������
	WIN32_FIND_DATA FindFileData; // ��� ���������� � �������� nameOfnTest
	HANDLE hf = FindFirstFile ("./tests/*.txt", &FindFileData);
	int n = 0;
    if (hf != INVALID_HANDLE_VALUE){
            do {
            	n++;
        	} while (FindNextFile(hf, &FindFileData));
            FindClose (hf);
    }
    return n;
}

void writeTests (int activ) { // ������� ��� ����� � ����������
	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile ("./tests/*.txt", &FindFileData);
	int n = 0;
    do {
		if (n == activ) // ���� ���� ��������, �������� ��� �����
 			SetConsoleTextAttribute (hstdout, (White << 4 | Black));
		else
			SetConsoleTextAttribute (hstdout, (Black << 4 | White));
		printf ("%s\n", FindFileData.cFileName);
		n++;
	} while (FindNextFile(hf, &FindFileData));
	SetConsoleTextAttribute (hstdout, (Black << 4 | White));
}

USER* startGame (USER *user) {
	int cur_oper = 0, // �������� �� ������� �����
		size = sizeOfDir() - 1; // ����� ��������� �� ����� �� �������
	while (1) {
		system("cls");
		writeTests(cur_oper); // ������� �������� ������
		switch(getch()){ // ���������
			case DOWN:
				cur_oper++;
				if (cur_oper > size) // ��������� �� ����� �� �������
					cur_oper = 0;
			break;
			case UP:
				cur_oper--;
				if (cur_oper < 0)
					cur_oper = size;
			break;
			case ENTER:
				return start (user, chuseDificalte(cur_oper)); // �������� ���� ��� ���� �������������
			break;
		}
	}
}

void showRezult(USER* user) { // ������� ���������� �������������
	system("cls");
	if (user->rez) { // ���� ������������ �������� ���� ���� ����
		GAME *cur = user->rez;
		for (cur; cur; cur = cur->next) // ��������� �� ��������� ��������� ���� �� ����
			printf ("�������� �����: %s\n ���������: %d / %d \n ������ %d \n", cur->dificalt, cur->rightQuest, cur->nQuest, cur->mark);
	}
	else
		printf ("����������� ���\n\n");
	system ("pause");
}

void gameMenu (USER *user) { // ���� ����� �����������
	const char *menu[] = {"������ ����", "�������� ���������� ������������", "����� �� ��������"};
	int cur_oper = 0;
	int size = sizeof(menu) / sizeof(char*); // ���-�� ������� �  ����
	while (1) {
		system("cls");
		shapka_Y(menu, cur_oper, sizeof(menu) / sizeof(char*)); // ��������� ����
		switch(getch()) { // �� ������� ������
			case DOWN: // ��������� �� ����
				cur_oper++;
				if (cur_oper > size - 1) // �������� �� ����� �� �������
					cur_oper = 0;
				break;
			case UP:
				cur_oper--;
				if (cur_oper < 0)
					cur_oper = (sizeof(menu) / sizeof(char*)) - 1;
				break;
			case ENTER:
				switch(cur_oper) { // �� ����� ������ ���������� ���� �������. ��������� �� � ����������� �� cur_oper
					case 0:
						user = startGame (user);
					break;
					case 1:
						showRezult(user);
					break;
					case 2:
						saveUser (user);
						return;
				}
			break;
			default:
				break;
		}
	}
}

void startScreen () { // ���������� �������� ��� � �� ���� ��������� �������� ����
	const char *menu[] = {"�����������", "�����������", "������", "�����"};
	int cur_oper = 0;
	int size = sizeof(menu) / sizeof(char*);
	USER *user;
	while (1) {
		system ("cls");
		shapka_Y (menu, cur_oper, size);
		switch (getch()) {
			case DOWN:
				cur_oper++;
				if (cur_oper > size - 1)
					cur_oper = 0;
				break;
			case UP:
				cur_oper--;
				if (cur_oper < 0)
					cur_oper = (sizeof(menu) / sizeof(char*)) - 1;
				break;
			case ENTER:
				switch (cur_oper) {
					case 0:
						user = registration();
						if (user)
							gameMenu(user);
						break;
					case 1:
						user = autorisation();
						if (user)
							gameMenu(user);
						break;
					case 2: {
						FILE *f = fopen ("HELP.txt", "r");
						printFile (f);
						fclose (f);
						break;
					}
					case 3:
						return;
				}
		}
	}
}

int main() {
	system ("chcp 1251");
	system ("cls");
//	FILE *f = fopen ("splashScreen.txt", "r");
	printFile (fopen ("splashScreen.txt", "r"));
//	fclose (f);
	startScreen();
}
