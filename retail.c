#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/ioctl.h>

#define MAX_LINE_W 2048
#define INPUT_BUFFER_SIZE 512

void *screenDrawingThread(void *);
void gracefulExit();
void initScreen();
void updateScreen();
void sigIntHandler(int);
void calculateTermSize();

extern int errno;
bool volatile hasToExit = false;
sem_t semaphore;
pthread_t drawingThread;
char **linesBuffer;
int currentLine;
int maxCols;
int maxRows;

int main(int argc, char **argv) {
	signal(SIGINT, sigIntHandler);
	// thread shared semaphore
	sem_init(&semaphore, 0, 1);
	// TODO handle SIGWINCH for window size changes. now it is fixed.
	calculateTermSize();

	// init buffers
	linesBuffer = (char **)(malloc(sizeof(char *) * INPUT_BUFFER_SIZE));
	memset(linesBuffer, 0, sizeof(char *) * INPUT_BUFFER_SIZE);
	currentLine = 0;

	// open STDIN
	int stdinFd = dup(STDIN_FILENO);
	if (stdinFd == -1) {
		gracefulExit();
		fprintf(stderr, "Error opening STDIN\n");
		fprintf(stderr, "ERRNO is %s (errno=%d)\n", strerror(errno), errno);
		return 1;			
	}
/*
	// init screen 
	initScreen();

	// screen drawing thread
	if (pthread_create(&drawingThread, NULL, screenDrawingThread, &stdinFd)) {
		gracefulExit();
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
*/
	
	while (!hasToExit) {
		char *buffer = (char *)malloc(sizeof(char) * MAX_LINE_W);
		// fgets is already null terminated but for sake of security i'll memset the same
		memset(buffer, 0, sizeof(char) * MAX_LINE_W);
		if (fgets(buffer, MAX_LINE_W, stdin) != NULL) {

			fprintf(stderr, "%s", buffer);
		}
		// freeing will be managed from linesBuffer clean itself
		free(buffer);
	}

	gracefulExit();
	return 0;
}

void sigIntHandler(int dummy) {
    	hasToExit = true;
	fprintf(stdout, "\n");
}

void calculateTermSize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	maxCols = w.ws_col;
	maxRows = w.ws_row;
}

void gracefulExit() {
	int i = 0;

	if (drawingThread) {
		pthread_cancel(drawingThread);
	}
	endwin();

	// lines buffer clearing
	for (i = 0; i < INPUT_BUFFER_SIZE; i++) {
		char *line = linesBuffer[i * sizeof(char *)];
		if (line) {
			free(line);
		}
	}
	free(linesBuffer);
}

void initScreen() {
	initscr();
	cbreak();
	noecho();
}

void* screenDrawingThread (void *ptr) {
	updateScreen();
	return NULL;
}

void updateScreen() {
	addch(42);
	refresh();	
}
