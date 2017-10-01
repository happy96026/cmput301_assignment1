#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "error_handling.h"

void flush_stdin() {
	char c;
	while ((c != EOF)&&((c = getchar()) != '\n')) {}
}

void prompt(char *buf, int buf_size) {
	printf("a1shell%%");
	fgets(buf, buf_size, stdin);
	if (strchr(buf, '\n') == NULL) {
		WARNING("command line is too long\n");
		flush_stdin();
	}
}

int get_no_of_args(char *buf) {
	int args = 0;
	char *token;
	token = strtok(buf, " ");
	while (token != NULL) {
		token = strtok(NULL, " ");
		args++;
	}
	return args;
}

void cd_c() {
}

void pwd_c() {
	printf("2\n");
}

void umask_c() {
	printf("3\n");
}

void done_c() {
	printf("3\n");
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

	int interval, args;
	int buf_size = 82;
	char buf[buf_size];
	char *cmd;

	if (argc < 2) {
		FATAL("not enough argument(s) (argc=%d)\n", argc);
	} else if (argc > 2) {
		FATAL("too many argument(s) (argc=%d)\n", argc);
	} else if ((interval = atoi(argv[1])) == 0) {
		FATAL("not a valid argument\n");
	} 

	while (1) {
		prompt(buf, buf_size);
		buf[strlen(buf) - 1] = '\0';
		args = get_no_of_args(buf);
		cmd = strtok(buf, " ");
		if (strcmp(cmd, "cd") == 0) {
			if (args < 2) {
				WARNING("no path declared\n");
			} else if (args > 2) {
				WARNING("too many paths declared\n");
			} else {
				cd_c();
			}
		} else if (strcmp(cmd, "pwd") == 0) {
			if (args > 1) {
				WARNING("too many arguments\n");
			} else {
				pwd_c();
			}
		} else if (strcmp(cmd, "umask") == 0) {
			if (args > 1) {
				WARNING("too many arguments\n");
			} else {
				umask_c();
			}
		} else if (strcmp(cmd, "done") == 0) {
			if (args > 1) {
				WARNING("too many arguments\n");
			} else {
				done_c();
			}
		} else {
			//printf("5\n");
		}
	}
}
