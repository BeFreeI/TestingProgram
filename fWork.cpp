void saveRezult (FILE *f, GAME *head) { // сохрание результатов игры
	GAME *bad;
	if (head) {
		for (GAME *cur = head; cur; ) { // запись в файл пока есть пока есть след элемент в списке
			fwrite (cur, sizeof(GAME), 1, f);
			bad = cur;
			cur = cur->next;
			free(bad); // удаляем элемент из памяти
		}
	}
}

void saveUser (USER* user) { // сохранение структуры пользователя в файл
	char str[strlen(user->login) + 19];
	strcpy (str, "./users/");
	strcat(str, user->login);
	strcat (str, ".bin"); // собираем нужный путь для файла
	FILE *f = fopen (str, "wb");
	fwrite (user, sizeof(USER), 1, f); // записываем в файл структуру пользователя
	saveRezult (f, user->rez); // записываем историю тестов пользователя
	free(user); // удаляем элемент из памяти
	fclose(f); // закрываем файл
}

GAME* uploadRezult (FILE *f) { // загружаем информацию пользователся из файла
	GAME *cur = (GAME*)malloc(sizeof(GAME));
	fread (cur, sizeof(GAME), 1, f);
	if (!feof(f)) { // если попытка считывания удалась
		GAME *head = cur;
		while (1) {
			GAME *next = (GAME*)malloc(sizeof(GAME));
			fread(next, sizeof(GAME), 1, f);
			if (feof(f)) { // если уже вышли за пределы
				free (next);
				break;
			}
			cur->next = next; // вставляем элемент в наш список
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

void printFile (FILE *ft) { // выводит текст из файла
	system("cls");
	if (!ft) // если файл не передан
		printf ("Требуемая информация отсутствует");
	else {
		char str[100];
		while (fgets (str, 100, ft)) // fgets возвращает ноль, если у него не получилось ничего считать
			printf ("%s", str);
	}
	printf ("\n\n");
	system ("pause");
}

bool rightFname (char *name) { // проверка на корректность имени файла
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
	printf ("Введите логин: ");
	gets(user->login);
	printf ("Введите пароль: ");
	gets(user->password);
	if (rightFname(user->login)) { // если неккоректный логин
		printf ("\nНеправильный логин\n\n\n");
		system ("pause");
		free(user);
		return NULL;
	}
	char str[strlen(user->login) + 19];
	strcpy (str, "./users/");
	strcat(str, user->login);
	strcat (str, ".bin"); // собираем путь к файлу
	FILE *f = fopen (str, "rb");
	if (f) { // если файл открылся
		printf ("\nНик %s уже занят\n\n\n", user->login);
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
	printf ("Введите логин: ");
	gets (login);
	printf ("Ведите пароль: ");
	gets (password);
	char str[strlen(login) + 19];
	strcpy (str, "./users/");
	strcat(str, login);
	strcat (str, ".bin");
	FILE *f = fopen (str, "rb");
	if (f) { // если файл открылся
		fread (user, sizeof(USER), 1, f);
		if (!strcmp(user->password, password)) { // проверяем пароль на совпадение
			user->rez = uploadRezult (f);
			return user;
		}
		fclose (f);
	}
	printf ("\nТакой комбинации логина и пароля нет\n\n\n");
	system ("pause");
	return NULL;
}
