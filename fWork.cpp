void saveRezult (FILE *f, GAME *head) { // �������� ����������� ����
	GAME *bad;
	if (head) {
		for (GAME *cur = head; cur; ) { // ������ � ���� ���� ���� ���� ���� ���� ������� � ������
			fwrite (cur, sizeof(GAME), 1, f);
			bad = cur;
			cur = cur->next;
			free(bad); // ������� ������� �� ������
		}
	}
}

void saveUser (USER* user) { // ���������� ��������� ������������ � ����
	char str[strlen(user->login) + 19];
	strcpy (str, "./users/");
	strcat(str, user->login);
	strcat (str, ".bin"); // �������� ������ ���� ��� �����
	FILE *f = fopen (str, "wb");
	fwrite (user, sizeof(USER), 1, f); // ���������� � ���� ��������� ������������
	saveRezult (f, user->rez); // ���������� ������� ������ ������������
	free(user); // ������� ������� �� ������
	fclose(f); // ��������� ����
}

GAME* uploadRezult (FILE *f) { // ��������� ���������� ������������� �� �����
	GAME *cur = (GAME*)malloc(sizeof(GAME));
	fread (cur, sizeof(GAME), 1, f);
	if (!feof(f)) { // ���� ������� ���������� �������
		GAME *head = cur;
		while (1) {
			GAME *next = (GAME*)malloc(sizeof(GAME));
			fread(next, sizeof(GAME), 1, f);
			if (feof(f)) { // ���� ��� ����� �� �������
				free (next);
				break;
			}
			cur->next = next; // ��������� ������� � ��� ������
			cur = next;
		}
		cur->next = NULL;
		return head;
	}
	else {
		free(cur);
		return NULL;
	}
}

void printFile (FILE *ft) { // ������� ����� �� �����
	system("cls");
	if (!ft) // ���� ���� �� �������
		printf ("��������� ���������� �����������");
	else {
		char str[100];
		while (fgets (str, 100, ft)) // fgets ���������� ����, ���� � ���� �� ���������� ������ �������
			printf ("%s", str);
	}
	printf ("\n\n");
	system ("pause");
}

bool rightFname (char *name) { // �������� �� ������������ ����� �����
	for (int i = 0; name[i]; i++) {
		if (name[i] == ' ' || name[i] == '/' || name[i] == ':' || name[i] == '*' ||
			name[i] == '?' || name[i] == '"' || name[i] == '<' || name[i] == '>' ||
			name[i] == '|' || name[i] == '+' || name[i] == '!' || name[i] == '.' ||
			name[i] == '@' || name[i] == '\\')
				return 1;
	}
	return 0;
}

USER* registration () {
	system ("cls");
	USER *user = (USER*)malloc(sizeof(USER));
	printf ("������� �����: ");
	gets(user->login);
	printf ("������� ������: ");
	gets(user->password);
	if (rightFname(user->login)) { // ���� ������������ �����
		printf ("\n������������ �����\n\n\n");
		system ("pause");
		free(user);
		return NULL;
	}
	char str[strlen(user->login) + 19];
	strcpy (str, "./users/");
	strcat(str, user->login);
	strcat (str, ".bin"); // �������� ���� � �����
	FILE *f = fopen (str, "rb");
	if (f) { // ���� ���� ��������
		printf ("\n��� %s ��� �����\n\n\n", user->login);
		system ("pause");
		free(user);
		fclose(f);
		return NULL;
	}
	user->rez = NULL;
	return user;
}

USER* autorisation () {
	system ("cls");
	char login[50], password[50];
	USER *user = (USER*)malloc(sizeof(USER));
	printf ("������� �����: ");
	gets (login);
	printf ("������ ������: ");
	gets (password);
	char str[strlen(login) + 19];
	strcpy (str, "./users/");
	strcat(str, login);
	strcat (str, ".bin");
	FILE *f = fopen (str, "rb");
	if (f) { // ���� ���� ��������
		fread (user, sizeof(USER), 1, f);
		if (!strcmp(user->password, password)) { // ��������� ������ �� ����������
			user->rez = uploadRezult (f);
			return user;
		}
		fclose (f);
	}
	printf ("\n����� ���������� ������ � ������ ���\n\n\n");
	system ("pause");
	return NULL;
}
