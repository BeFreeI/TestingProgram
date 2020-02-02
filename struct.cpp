typedef struct quest {
	char question[1000],
		 true_answer[1000],
		 answer1[1000],
		 answer2[1000],
		 answer3[1000];
}QUESTION;

typedef struct rezult {
	int mark, // оценка за тест
		nQuest, // кол-во вопросов в тесте
		rightQuest; // кол-во правильных ответов
	char dificalt[20]; // название теста
	rezult *next;
}GAME;

typedef struct user {
	char login[50],
		 password[50];
	GAME *rez; // результаты тестов
}USER;
