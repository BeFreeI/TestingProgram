typedef struct quest {
	char question[1000],
		 true_answer[1000],
		 answer1[1000],
		 answer2[1000],
		 answer3[1000];
}QUESTION;

typedef struct rezult {
	int mark, // ������ �� ����
		nQuest, // ���-�� �������� � �����
		rightQuest; // ���-�� ���������� �������
	char dificalt[20]; // �������� �����
	rezult *next;
}GAME;

typedef struct user {
	char login[50],
		 password[50];
	GAME *rez; // ���������� ������
}USER;
