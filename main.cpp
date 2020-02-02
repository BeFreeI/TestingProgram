#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "struct.cpp"
#include "fWork.cpp"

HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE); // для управления выводом(деструктор консоли)

const int LEFT = 75, RIGHT = 77, DOWN = 80, UP = 72, ESC = 27, ENTER = 13; //системные клавиши

const int Black = 0, White = 15; // цвета консоли

void shapka_Y (const char **menu_this, int cur_oper, int size) { //вертикально выводит пункты меню
	for (int i = 0; i < size; i++) {
			if (i == cur_oper) { // если элемент который выводим активный
				SetConsoleTextAttribute (hstdout, (White << 4 | Black)); // смена цвета вывода в консоль
			}
			else {
				SetConsoleTextAttribute (hstdout, (Black << 4 | White));
			}
			printf ("%s\n", menu_this[i]);
		}
	SetConsoleTextAttribute (hstdout, (Black << 4 | White));
}

char* nameOfnTest (int n) { // выводит имя активного теста
	WIN32_FIND_DATA FindFileData; // структура файлов в винде
	HANDLE hf = FindFirstFile ("./tests/*.txt", &FindFileData); // поиск первого файла в директории
	int i = 0;
    if (hf != INVALID_HANDLE_VALUE) { // проверка на правильность пути
        do {
        	if (n == i) // если это нужный нам файл
				return FindFileData.cFileName; // возвращаем имя файйла
        	i++;
    	} while (FindNextFile(hf, &FindFileData)); // пока есть следующий файл
        FindClose (hf); // закрываем директорию
    }
}

GAME* chuseDificalte (int n) { // подготавливаем игру к началу
	GAME *game = (GAME*)malloc(sizeof(GAME));
	game->next = NULL;
	game->rightQuest = 0;
	game->nQuest = 0;
	game->mark = 0;
	strcpy(game->dificalt, nameOfnTest(n)); // сохраняем имя теста
	return game;
}

int randRange (int max, int min) { // генерация рандомного числа в диапозоне [a, b]
	return rand() % (max - min + 1) + min;
}

QUESTION* getNextQuestion (FILE *ft, QUESTION *quest) {
	fgets(quest->question, sizeof(quest->question), ft); // считываем все поля для вопроса
	fgets(quest->true_answer, sizeof(quest->true_answer), ft);
	fgets(quest->answer1, sizeof(quest->answer1), ft);
	fgets(quest->answer2, sizeof(quest->answer2), ft);
	fgets(quest->answer3, sizeof(quest->answer3), ft);
	if (feof(ft)) // если вопроса не было, то вернем NULL
		return NULL;
	return quest;
}

USER* start (USER* user, GAME* game) { // сам тест
	char str[strlen(game->dificalt) + strlen("./tests/")];
	strcpy (str, "./tests/");
	strcat (str, game->dificalt); // собираем полный путь к выбранному тесту
	FILE *ft = fopen(str, "r"); // открытие файла с тестом
	QUESTION *cur = (QUESTION*)malloc(sizeof(QUESTION));
	char *answer[4];
	int nHelp = 3;
	cur = getNextQuestion (ft, cur);
	srand (time(NULL)); // начинаем псевдорандомную последовательность с текущего времени в секундах
	for (cur; cur; cur = getNextQuestion (ft, cur)) { // считываем вопросы пока не дойдем до конца файла
		game->nQuest++; // считаем кол-во вопросов в тесте
		answer[0] = cur->true_answer; // считываем варианты ответов
		answer[1] = cur->answer1;
		answer[2] = cur->answer2;
		answer[3] = cur->answer3;
		for (int i = 0; i < 10; i++) { // перемешиваем ответы
			int n1 = randRange(3, 0), n2 = randRange(3, 0);
			char *boof = answer[n1];
			answer[n1] = answer[n2];
			answer[n2] = boof;
		}
		system("cls");
		printf ("%s", cur->question);
		for (int i = 0; i < 4; i++) // выводим ответы на экран
			printf ("%d. %s", i + 1, answer[i]);
		printf ("Ваш выбор?\n");
		repid: ;
		int key = getch(); // вводим номер ответа
		if (key - '0' >= 1 && key - '0' <= 4) { // проверям на корректность ответ
			if (!strcmp(answer[(key - '0') - 1], cur->true_answer)) // проверяем верный ли ответ
				game->rightQuest++;
		}
		else if (key == 'h' || key == 'р') { // если использованна подскахка
			if (!nHelp) { // проверяем остались ли подсказки
				printf ("Закончились подсказки\n");
				goto repid;
			}
			nHelp--;
			system ("cls");
			printf ("%s", cur->question);
			int n;
			if (!randRange(1, 0)) { // первым будет верный вариант ответа
				answer[0] = cur->true_answer;
				do {
					n = randRange (3, 0);
				} while(!strcmp(answer[n], cur->true_answer));
				answer[1] = answer[n];
				printf ("1. %s2. %s", answer[0], answer[1]);
			}
			else { // первым будет неверный вариант ответа
				do {
					n = randRange (3, 0);
				} while(!strcmp(answer[n], cur->true_answer));
				answer[0] = answer[n];
				answer[1] = cur->true_answer;
				printf ("1. %s2. %s", answer[0], answer[1]);
			}
			repidEase: ;
			key = getch();
			if (key - '0' >= 1 && key - '0' <= 2) { // тоже проверка на корректность
				if (!strcmp(answer[(key - '0') - 1], cur->true_answer))
					game->rightQuest++;
			}
			else
				goto repidEase;
		}
		else // если выбран неккоректный ответ
			goto repid;
	}
	game->next = user->rez; // сохраняет результат теста в статистику пользователя
	user->rez = game;
	if (game->rightQuest == 0) // выставляем оценку пользователю
		game->mark = 0;
	else
		game->mark = (float)game->rightQuest / (float)game->nQuest * 10;
	fclose(ft);
	free(cur);
	return user; // возвращаем пользователя
}

int sizeOfDir () { // узнаем кол-во файлов в директории
	WIN32_FIND_DATA FindFileData; // все аналогично с функцией nameOfnTest
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

void writeTests (int activ) { // выводим все файла в директории
	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile ("./tests/*.txt", &FindFileData);
	int n = 0;
    do {
		if (n == activ) // если файл активный, выделяем его белым
 			SetConsoleTextAttribute (hstdout, (White << 4 | Black));
		else
			SetConsoleTextAttribute (hstdout, (Black << 4 | White));
		printf ("%s\n", FindFileData.cFileName);
		n++;
	} while (FindNextFile(hf, &FindFileData));
	SetConsoleTextAttribute (hstdout, (Black << 4 | White));
}

USER* startGame (USER *user) {
	int cur_oper = 0, // отвечает за текущий пункт
		size = sizeOfDir() - 1; // будет проверять на выход за пределы
	while (1) {
		system("cls");
		writeTests(cur_oper); // выводим названия тестов
		switch(getch()){ // скроллинг
			case DOWN:
				cur_oper++;
				if (cur_oper > size) // проверяем на выход за пределы
					cur_oper = 0;
			break;
			case UP:
				cur_oper--;
				if (cur_oper < 0)
					cur_oper = size;
			break;
			case ENTER:
				return start (user, chuseDificalte(cur_oper)); // начинаем тест под этим пользователем
			break;
		}
	}
}

void showRezult(USER* user) { // выводим результаты пользователся
	system("cls");
	if (user->rez) { // если пользователь проходил хоть один тест
		GAME *cur = user->rez;
		for (cur; cur; cur = cur->next) // переходим на следующий результат пока он есть
			printf ("Название теста: %s\n Результат: %d / %d \n оценка %d \n", cur->dificalt, cur->rightQuest, cur->nQuest, cur->mark);
	}
	else
		printf ("Результатов нет\n\n");
	system ("pause");
}

void gameMenu (USER *user) { // меню после авторизации
	const char *menu[] = {"Начать тест", "Показать результаты пользователя", "Выход из аккаунта"};
	int cur_oper = 0;
	int size = sizeof(menu) / sizeof(char*); // кол-во пунктов в  меню
	while (1) {
		system("cls");
		shapka_Y(menu, cur_oper, sizeof(menu) / sizeof(char*)); // отрисовка меню
		switch(getch()) { // по нажатой кнопке
			case DOWN: // скроллинг по меню
				cur_oper++;
				if (cur_oper > size - 1) // проверка на выход за пределы
					cur_oper = 0;
				break;
			case UP:
				cur_oper--;
				if (cur_oper < 0)
					cur_oper = (sizeof(menu) / sizeof(char*)) - 1;
				break;
			case ENTER:
				switch(cur_oper) { // на кадую кнопку привязанна своя функция. Выполняем ее в зависимости от cur_oper
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

void startScreen () { // интерактив работает как и во всех остальных функциях меню
	const char *menu[] = {"Регистрация", "Авторизация", "Помощь", "Выход"};
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
