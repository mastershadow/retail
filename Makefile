all: retail.c 
	gcc retail.c -o retail -lncurses -lpthread
