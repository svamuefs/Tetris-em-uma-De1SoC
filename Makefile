#make all
all:
	gcc main.c -lintelfpgaup -o exec
	sudo ./exec