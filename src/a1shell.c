#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/wait.h>
#include "error_handling.h"

#define BUFSIZE 82


void flush_stdin() {
	char c;
	while ((c != EOF)&&((c = getchar()) != '\n')) {}
}

// Store input from stdin as string
void get_line(char *buf) {
	printf("a1shell%%");
	fgets(buf, BUFSIZE, stdin);
	if (strchr(buf, '\n') == NULL) {
		WARNING("command line is too long\n");
		flush_stdin();
	}
	buf[strlen(buf) - 1] = '\0';
}

// Store arguments that are separated by spaces into a
// string array
int get_arguments(char **args, char *line) {
	char *token;
	int i = 0;

	char *copy = (char*) malloc(sizeof(char) * BUFSIZE);
	strcpy(copy, line);

	token = strtok(copy, " ");
	while (token != NULL) {
		args[i] = token;
		i++;
		token = strtok(NULL, " ");
	}

	return i;
}

// Current Directory Command
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

// Print Working Directory Command
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

// Print Current User file creation MASK Command
void umask_c() {
	mode_t mask = umask(0);
	umask(mask);
	printf("current umask = %o, %d\n", mask, mask);
	printf("S_IRWXU = %o, %d\n", S_IRWXU, S_IRWXU);
	printf("S_IRWXG = %o, %d\n", S_IRWXG, S_IRWXG);
	printf("S_IRWXO = %o, %d\n", S_IRWXO, S_IRWXO);
}

// Exitng a1shell Command
void done_c() {
	exit(0);
}

// Display Process Time
void disp_proc_time(clock_t start, clock_t end,
					struct tms *tmsstart, struct tms *tmsend) {
	static long clktck;
	if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
		FATAL("sysconf failed");
	}

	printf("time elapsed: %7.2f\n", (end-start) / (double)clktck);
	printf("a1shell user: %7.2f\n", (tmsend->tms_utime  - tmsstart->tms_utime)  / (double)clktck);
	printf("a1shell  sys: %7.2f\n", (tmsend->tms_stime  - tmsstart->tms_stime)  / (double)clktck);
	printf("child 	user: %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double)clktck);
	printf("child 	 sys: %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double)clktck);
}

// Run other commands in Bash
void do_cmd(char *line) {
	struct tms tmsstart, tmsend;
	clock_t start, end;
	pid_t pid;

	if ((start = times(&tmsstart)) == -1) {
		FATAL("times failed");
	}

	if ((pid = fork()) < 0) {
		FATAL("fork failed");
	} else if (pid == 0) { // Child Process
		execl("/bin/bash", "bash", "-c", line, (char *) 0);
		exit(0);
	} else {			   // Parent Process
		if (waitpid(pid, NULL, 0) != pid) {
			FATAL("waitpid error");
		}
		if ((end = times(&tmsend)) == -1) {
			FATAL("times failed");
		}
		disp_proc_time(start, end, &tmsstart, &tmsend);
	}
}

// Logic for executing different commands
void execute_commands(char **args, int no_args, char *line) {
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
		do_cmd(line);
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

	char *line, **args;
	int no_args, interval;

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

		execute_commands(args, no_args, line);

		free(line);
		free(args);
	}

}
