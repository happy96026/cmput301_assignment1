#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "error_handling.h"

void flush() {
	char c;
	while ((c != EOF)&&((c = getchar()) != '\n')) {}
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

	int interval;
	int buf_size = 82;
	char buf[buf_size];

	if (argc < 2) {
		FATAL("not enough argument(s) (argc=%d)\n", argc);
	} else if (argc > 2) {
		FATAL("too many argument(s) (argc=%d)\n", argc);
	} else if ((interval = atoi(argv[1])) == 0) {
		FATAL("not a valid argument\n");
	} else {
		while (1) {
			printf("a1shell%%");
			fgets(buf, buf_size, stdin);
			if (strchr(buf, '\n') == NULL) {
				WARNING("command line is too long\n");
				flush();
			} else if (strlen(buf) == 1) {
				continue;
			} else {
				break;
			}
		}
		//printf("hello");
	}

}
