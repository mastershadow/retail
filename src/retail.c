/**
 * Retail is a nightly project by Eduard Roccatello.
 * 
 * Made with love for Guly and his tremendous peppers.
 * Greets to #openbsd on AzzurraNet ;)
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>


#define MAX_LINE_W 2048
#define BUFFER_SIZE 512

typedef struct {
    char *value;
} Line;

typedef struct {
    int size;
    int start;
    int count;
    Line *items;
} Buffer;

void bufInit(Buffer *b, int maxSize);
bool bufIsFull(Buffer *b);
bool bufIsEmpty(Buffer *b);
void bufWrite(Buffer *b, Line *l);
Line *bufRead(Buffer *b, int pos);
void bufFree(Buffer *b);

void *screenDrawingThread(void *);
void gracefulExit();
void initScreen();
void updateScreen();
void sigIntHandler(int);
void calculateTermSize();
void putLineInBuffer(Line *line);
int rowsForLine(Line *l, int maxCols);

extern int errno;
bool volatile hasToExit = false;
bool volatile hasToRedraw = false;
pthread_t drawingThread;
pthread_mutex_t lock;

Buffer buffer;

int main(int argc, char **argv) {
        // ctrl - c handler
	signal(SIGINT, sigIntHandler);
        
        // mutex 
        if (pthread_mutex_init(&lock, NULL) != 0) {
            gracefulExit();
            fprintf(stderr, "Error creating mutex\n");
            return 1;
        }
        
        // init buffer
        bufInit(&buffer, BUFFER_SIZE);

	// init screen 
	initScreen();

	// screen drawing thread
	if (pthread_create(&drawingThread, NULL, screenDrawingThread, NULL)) {
		gracefulExit();
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
        
        // Configure STDIN to be non blocking
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
        char *line = (char *)malloc(sizeof(char) * MAX_LINE_W);
	while (!hasToExit) {
		if (fgets(line, MAX_LINE_W, stdin) != NULL) {
                    pthread_mutex_lock(&lock);
                    Line *l = (Line *)malloc(sizeof(Line));
                    // dup line
                    l->value = strdup(line);
                    putLineInBuffer(l);
                    pthread_mutex_unlock(&lock);
		}
	}
        free(line);
	gracefulExit();
	return 0;
}

void sigIntHandler(int dummy) {
    	hasToExit = true;
	fprintf(stdout, "\n");
}

void putLineInBuffer(Line *line) {
    bufWrite(&buffer, line);
    hasToRedraw = true;
}

void gracefulExit() {

	if (drawingThread) {
            pthread_cancel(drawingThread);
	}
        
        pthread_mutex_destroy(&lock);
	endwin();

	// lines buffer clearing
        bufFree(&buffer);
}

void initScreen() {
	initscr();
	cbreak();
	noecho();
}

void* screenDrawingThread (void *ptr) {
        while (!hasToExit) {
            if (hasToRedraw) {
                pthread_mutex_lock(&lock);
                updateScreen();
                hasToRedraw = false;
                pthread_mutex_unlock(&lock);
            }
        }
	return NULL;
}

int rowsForLine(Line *l, int maxCols) {
    int len = strlen(l->value);
    return len / maxCols;
}

void updateScreen() {
        Buffer *b = &buffer;
        int mCols = 0;
        int mRows = 0;
        int rowsLeft = 0;
        int iterations = 0;
        long iterator = (b->start + b->count - 1) % b->size;
        clear();
        getmaxyx(stdscr, mCols, mRows);
        rowsLeft = mRows;
        if (b->count > 0) {
            while (iterations < b->count && rowsLeft > 0) {
                Line *l = bufRead(b, iterator);
                if (l != NULL) {
                    int rowsLine = rowsForLine(l, mCols);
                    printw("%s", l->value);
                    iterations++;
                    iterator--;
                    if (iterator < 0) {
                        iterator = b->start + b->count - 1;
                    }
                    rowsLeft -= rowsLine;
                }
            }
        }
	refresh();	
}

void bufInit(Buffer *b, int maxSize) {
    b->size = maxSize;
    b->start = 0;
    b->count = 0;
    b->items = (Line *)(malloc(sizeof(Line) * maxSize));
}

bool bufIsFull(Buffer *b) {
    return b->count == b->size;
}

bool bufIsEmpty(Buffer *b) {
    return b->count == 0;
}

void bufWrite(Buffer *b, Line *l) {
    int end = (b->start + b->count) % b->size;
    if (b->count == b->size) { // buffer is full
        b->start = (b->start + 1) % b->size;
        // free the item
        Line *l = &b->items[end];
        if (l != NULL) {
            free(l->value);
        }
    } else {
        b->count++;
    }
    // set new item
    b->items[end] = *l;
}

Line *bufRead(Buffer *b, int pos) {
    int itemPos = (b->start + pos) % b->size;
    return &b->items[itemPos];
}

void bufFree(Buffer *b) {
    int i = 0;
    for (i = 0; i < b->count; i++) {
        Line *l = &b->items[i];
        if (l != NULL && l->value != NULL) {
            free(l->value);
        }
    }
    free(b->items);
}