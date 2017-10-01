#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "error_handling.h"

#define BUFSIZE 82


void flush_stdin() {
	char c;
	while ((c != EOF)&&((c = getchar()) != '\n')) {}
}

void get_line(char *buf) {
	printf("a1shell%%");
	fgets(buf, BUFSIZE, stdin);
	if (strchr(buf, '\n') == NULL) {
		WARNING("command line is too long\n");
		flush_stdin();
	}
	buf[strlen(buf) - 1] = '\0';
}

int get_arguments(char **args, char *line) {
	char *token;
	int i = 0;

	token = strtok(line, " ");
	while (token != NULL) {
		args[i] = token;
		i++;
		token = strtok(NULL, " ");
	}

	return i;
}

void cd_c(char **args) {
	char *path = (char*) calloc(PATH_MAX, sizeof(char));
	char *token, *env;
	if (path == NULL) {
		printf("malloc failed\n");
	}

	token = strtok(args[1], "%/");
	while (token != NULL) {
		if ((env = getenv(token)) == NULL) {
			strcat(path, token);
		} else {
			strcat(path, env);
		}
		strcat(path, "/");
		token = strtok(NULL, "/");
	}

	if (chdir(path) < 0) {
		WARNING("chdir failed\n");
	}
	free(path);
	path = NULL;
}

void pwd_c() {
	char *ptr  = (char*) malloc(PATH_MAX);
	if (ptr == NULL) {
		FATAL("malloc failed");
	}
	if (getcwd(ptr, PATH_MAX) == NULL) {
		WARNING("getcwd failed\n");
	} else {
		printf("%s\n", ptr);
	}
	free(ptr);
}

void umask_c() {
	mode_t mask = umask(0);
	umask(mask);
	printf("current umask = %o, %d\n", mask, mask);
	printf("S_IRWXU = %o, %d\n", S_IRWXU, S_IRWXU);
	printf("S_IRWXG = %o, %d\n", S_IRWXG, S_IRWXG);
	printf("S_IRWXO = %o, %d\n", S_IRWXO, S_IRWXO);
}

void done_c() {
	exit(0);
}

void do_cmd(char **args, int no_args) {
	struct tms tmsstart, tmsend;
	clock_t start, end;
	pid_t pid;

	if ((start = times(&tmsstart)) == -1) {
		FATAL("times failed");
	}

	
}

void execute_commands(char **args, int no_args) {
	if (strcmp(args[0], "cd") == 0) {
		if (no_args < 2) {
			WARNING("no path declared\n");
		} else if (no_args > 2) {
			WARNING("too many paths declared\n");
		} else {
			cd_c(args);
		}
	} else if (strcmp(args[0], "pwd") == 0) {
		if (no_args > 1) {
			WARNING("too many arguments\n");
		} else {
			pwd_c();
		}
	} else if (strcmp(args[0], "umask") == 0) {
		if (no_args > 1) {
			WARNING("too many arguments\n");
		} else {
			umask_c();
		}
	} else if (strcmp(args[0], "done") == 0) {
		if (no_args > 1) {
			WARNING("too many arguments\n");
		} else {
			done_c();
		}
	} else {
		do_cmd(args, no_args);
	}
}

void a1monitor() {

	time_t t;
	struct tm *tmp;
	char buf[64], load_avg0[10], load_avg1[10], load_avg2[10], processes[10];
	FILE *fp;

	t = time(NULL);
	tmp	= localtime(&t);

	if (strftime(buf, 64, "%a, %b %d, %Y %r", tmp) == 0) {
		FATAL("buffer length is too small\n");
	} else if ((fp = fopen("/proc/loadavg", "r")) == NULL) {
		FATAL("fopen error (fp=%p)\n", fp);
	} else {
		fscanf(fp, "%s %s %s %s", load_avg0, load_avg1, load_avg2, processes);
		printf("a1monitor: %s\n", buf);
		printf("           Load average: %s, %s, %s\n", load_avg0, load_avg1, load_avg2);
		printf("           Processes: %s\n", processes);
		fclose(fp);
	}

}


int main(int argc, char **argv) {

	char *line;
	char **args;
	int no_args;
	int interval;

	if (argc < 2) {
		FATAL("not enough argument(s) (argc=%d)\n", argc);
	} else if (argc > 2) {
		FATAL("too many argument(s) (argc=%d)\n", argc);
	} else if ((interval = atoi(argv[1])) == 0) {
		FATAL("not a valid argument\n");
	} 

	while (1) {
		line = (char*) malloc(sizeof(char) * BUFSIZE);
		args = (char**) malloc(sizeof(char*) * BUFSIZE);
		if (line == NULL || args == NULL) {
			FATAL("malloc failed");
		}

		get_line(line);
		no_args = get_arguments(args, line);

		execute_commands(args, no_args);

		free(line);
		free(args);
	}

}
