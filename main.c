#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/* СТРУКТУРЫ */

typedef struct item {
	char* s;
	int num;
	struct item* next;
} node;

typedef struct buf {
	int length;
	int size;
	char* t;
} buffer;

/* ОПЕРАЦИИ С БУФЕРОМ */

void add_char(buffer* str, int c)
{
	int i;
	char* z;
	str->t[str->length] = c;
	str->length++;
	if (str->length >= str->size) {
		(str->size) *= 2;
		z = malloc(str->size);
		for (i = 0; i < str->length; i++)
			z[i] = str->t[i];
		free(str->t);
		str->t = z;
	}
}

void update_buffer(buffer* str)
{
	str->length = 0;
	str->size = 1;
	str->t = malloc(str->size);
}

char* str_copy(buffer str, int len, int i)
{
	int j, k = 0;
	char* z = malloc(len + 1);
	for (j = (i - len); j < i; j++) {
		if (str.t[j] == '\'') 
			;
		else {
			z[k] = str.t[j];
			k++;
		}
	}
	z[k] = '\0';
	return z;
}

/* ОПЕРАЦИИ СО СПИСКОМ */

node* create_node(char* z, node** first, node** last)
{
	node* tmp;
	tmp = malloc(sizeof(node));
	tmp->s = z;
	tmp->num = 0;
	tmp->next = NULL;
	if (*last) {
		(*last)->next = tmp;
		*last = (*last)->next;
	}
	else
		*first = *last = tmp;
	return (*first);
}

int list_len(node* first)
{
	int len = 0;
	while (first && first->num != 5) {
		len++;
		first = first->next;
	}
	return len;
}

void move_pointer(node** first)
{
	while ((*first)->next && (*first)->num != 5)
		(*first) = (*first)->next;
	if ((*first)->next)
		(*first) = (*first)->next;
}

void output(node* first)
{
	while (first) {
		printf("[%s] [%d] \n", first->s, first->num);
		first = first->next;
	}
}

void free_list(node* first)
{
	node* tmp;
	while (first) {
		tmp = first;
		first = first->next;
		free(tmp->s);
		free(tmp);
	}
}

/* ОБРАБОТКА РАЗДЕЛИТЕЛЕй */

void sep_num(node* last , int c)
{
	switch(c) {
		case '&':
			last->num = 1;
			break;
		case '>':
			last->num = 2;
			break;
		case '<':
			last->num = 3;
			break;
		case '|':
			last->num = 5;
	}
}

int check_sep(int c)
{
	return (c != '&' && 
			c != '<' && 
			c != '>' && 
			c != '|');
}

int check_amp(node* first)
{
	while (first->next)
		first = first->next;
	return (first->num == 1);
}

int pipe_counter(node* first)
{
	int counter = 0;
	while (first) {
		if (first->num == 5)
			counter++;
		first = first->next;
	}
	return (counter + 1);
}


void add_sep(int p, int c, node** first, node** last)
{
	char* z;
	int i;
	z = malloc(p);
	for (i = 0; i < p - 1; i++)
		z[i] = c;
	z[p - 1] = '\0';
	(*first) = create_node(z, first, last);
	sep_num((*last), c);
	if (p > 2)
		(*last)->num = 4;
}

int sep_counter(node* first)
{
	int s1 = 0, s2 = 0, s3 = 0;
	while (first) {
		switch (first->num) {
			case 1:
				s1++;
				break;
			case 2:
				s2++;
				break;
			case 3:
				s3++;
				break;
			case 4:
				s2++;
				break;
		}
		first = first->next;
	}
	return (s1 > 1 || s2 > 1 || s3 > 1);
}

int brack_counter(node* first)
{
	int counter = 0;
	while (first && first->num != 5) {
		if (first->num > 1)
			counter++;
		first = first->next;
	}
	return counter;
}


/* ПЕРВОД БУФФЕРА В СПИСОК */

node* buf_to_list(buffer str)
{
	int i, c, p, quotes = 0, len = 0;
	char* z;
	node* first = NULL;
	node* last = NULL;
	for (i = 0; i < str.length; i++){
		c = str.t[i];
		p = check_sep(c);
		if ((c != ' ' || quotes) && (p == 1 || quotes)) {
			if (c == '\'')
				quotes = !quotes;
			len++;
		}
		else {
			if (len > 0) {
				z = str_copy(str, len, i);
				first = create_node(z, &first, &last);
			}
			len = 0;
			if (!check_sep(c)) {
				if (c == '>' && str.t[i + 1] == '>') {
					add_sep(3, c, &first, &last);
					i++;
				}
				else
					add_sep(2, c, &first, &last);
			}
		}
	}
	if (len > 0) {
		z = str_copy(str, len, i);
		first = create_node(z, &first, &last);
	}
	return first;
}

/* ПРОВЕРКА БАЛАНСА КАВЫЧЕК */

int check_syntax(buffer str)
{
	int quotes = 0;
	int i, c;
	for (i = 0; i <= str.length; i++) {
		c = str.t[i];
		if (c == '\'')
			quotes = !quotes;
		else 
			if (quotes && c != ' ' && check_sep(c))
				return 1;
	}
	return 0;

}

int sep_syntax(node* first)
{
	if ((first->num) > 0 ||
		sep_counter(first))
	{
		return 1;
	}

	while (first->next) {
		if (first->num > 1 && first->next->num > 0)
			return 1;
		else 
			if (first->num == 1 && first->next)
				return 1;
		first = first->next;
	}

	if ((first->num) > 1 && 
		first->next == NULL)
	{
		return 1;
	}
	else
		return 0;	
}

/* МАССИВ ФАЙЛОВ*/

char** file_names(node* first)
{
	int i;
	char** a = malloc(sizeof(char*) * 3);
	for(i = 0; i < 3; i++)
		a[i] = NULL;
	while (first) {
		if (first->num > 1 && first->num < 5)
			a[first->num - 2] = first->next->s;
		first = first->next;
	}
	return a;
}

/* ПЕРЕВОД СПИСКА В МАССИВ */

char** list_to_argv(node* first)
{
	int len = list_len(first);
	int brack = brack_counter(first);
	int amp = check_amp(first);
	int p = len - (brack * 2) - amp;
	int i;
	char** a = malloc(sizeof(char*) * (p + 1));
	for (i = 0; i < p; i++) {
		if (first->num >= 1 && first->num < 5) {
			if (first->next->next) {
				first = first->next->next;
				i--;
			}
		}
		else {
			a[i] = first->s;
			first = first->next;
		}
	}
	a[p] = NULL;
	return a;
}

/* ТРЁХМЕРНЫЙ МАССИВ ДЛЯ КОНВЕЕРА*/

char*** argv_array(node* first)
{
	int i;
	int p = pipe_counter(first);
	char*** a = malloc(sizeof(char**) * (p + 1));
	for (i = 0; i < p; i++) {
		a[i] = list_to_argv(first);
		move_pointer(&first);
	}
	a[p] = NULL;
	return a;
}

/* ДЛИНА МАССИВА */

int argv_len(char** argv)
{
	int len = 0;
	while (*(argv++))
		len++;
	return len;
}

/* ВЫВОД МАССИВА */

void argv_output(char** argv)
{
	int i;
	int n = argv_len(argv);
	for (i = 0; i < n; i++) 
		printf("[%s] ", argv[i]);
	printf("\n");
}

/* ВЫВОД ТРЁХМЕРНОГО МАССИВА */

void argv_array_output(char*** a, node* first)
{
	int i;
	int p = pipe_counter(first);
	for (i = 0; i < p; i++)
		argv_output(a[i]);
}

/* ОЧИСТКА ТРЁХМЕРНОГО МАССИВА */

void free_argv_array(char*** a, node* first)
{
	int i;
	int p = pipe_counter(first);
	for(i = 0; i < p; i++)
		free(a[i]);
	free(a);
}

/* НАСТРОЙКА ДЕСКРИПТОРОВ */

void dup_settings(char** files, int num)
{
	int fd_i, fd_o_1, fd_o_2;
	if (files[0] && num == 2) {
		fd_o_1 = open(files[0], 
			O_CREAT|O_WRONLY|O_TRUNC, 
			0666);
		if (fd_o_1 == -1) {
			perror(files[0]);
			exit(1);
		}
		dup2(fd_o_1, 1);
		close(fd_o_1);
	}
	if (files[2] && num == 2) {
		fd_o_2 = open(files[2], 
			O_CREAT|O_WRONLY|O_APPEND, 
			0666);
		if (fd_o_2 == -1) {
			perror(files[2]);
			exit(1);
		}
		dup2(fd_o_2, 1);
		close(fd_o_2);
	}
	if (files[1] && num == 1) {
		fd_i = open(files[1], 
			O_CREAT|O_RDONLY, 
			0666);
		if (fd_i == -1) {
			perror(files[1]);
			exit(1);
		}
		dup2(fd_i, 0);
		close(fd_i);
	}
}

/* ВЫПОЛНЕНИЕ НЕСКОЛЬКИХ КОМАНД (КОНВЕЙЕР) */

void several_commands(char*** a, char** files, node* first)
{
	int i, j;
	int v = check_amp(first);
	int p = pipe_counter(first) - 1;
	int pipes[p][2];
	for (i = 0; i < p; i++)
		pipe(pipes[i]);
	for (i = 0; i <= p; i++) {
		if (fork() == 0) {
			if (i == 0) {
				dup2(pipes[i][1], 1);
				dup_settings(files, 1);
			}
			else {
				if (i == p) {
					dup2(pipes[i - 1][0], 0);
					dup_settings(files, 2);
				}
				else {
					dup2(pipes[i - 1][0], 0);
					dup2(pipes[i][1], 1);
				}
			}
			for (j = 0; j < p; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			execvp(a[i][0], a[i]);
			perror(a[i][0]);
			exit(1);
		}
	}
	for (j = 0; j < p; j++) {
		close(pipes[j][0]);
		close(pipes[j][1]);
	}
	if (v == 0)
		for (i = 0; i < p; i++)
			wait(NULL);
}

/* ВЫПОЛНЕНИЕ ОДНОЙ КОМАНДЫ */

void one_command(char*** a, char** files, node* first)
{	
	int x;
	char** argv = a[0];
	int v = check_amp(first);
	if (argv[0][0] == 'c' && 
		argv[0][1] == 'd' && 
		argv[0][2] == '\0') 
	{
		if (argv_len(argv) != 2)
			perror("Wrong number of parameters");
		else {
			int result = chdir(argv[1]);
			if (result != 0)
				perror(argv[1]);
		}
	}
	else {
		int pid = fork();
		if (pid == -1) {
			perror("fork");
			exit(1);
		}
		if (pid == 0) {
			dup_settings(files, 1);
			dup_settings(files, 2);
			execvp(argv[0], argv);
			perror(argv[0]);
			exit(1);
		}
		if (v == 0)
			while((x = wait(NULL)) != pid)
			{}
	}
}

/* ОЧИСТКА ОТ ПРОЦЕССОВ-ЗОМБИ */

void zombie_kill()
{
	int p;
	while ((p = wait4(-1, NULL, WNOHANG, NULL)) > 0)
	{}
}

/* ОСНОВНАЯ ФУНКЦИЯ */

int main()
{
	int i, c;
	buffer str;
	update_buffer(&str);
	while ((c = getchar()) != EOF) {
		if (c != '\n')
			add_char(&str, c);
		else {
			str.t[str.length] = '\0';
			if (check_syntax(str))
				printf("Syntax Error \n");
			else {
				node* first = buf_to_list(str);
				zombie_kill();
				if ((first) && sep_syntax(first))
					printf("Syntax Error \n");
				else if (first){
					char** files = file_names(first);
					char*** a = argv_array(first);
					if ((pipe_counter(first)) < 2)
						one_command(a, files, first);
					else
						several_commands(a, files, first);
					free_argv_array(a, first);
					free(files);
				}
				free_list(first);
			}
			free(str.t);
			update_buffer(&str);
		}
	}
	free(str.t);
	return 0;
}
