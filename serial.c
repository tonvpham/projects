//Tony Pham
//CPSC 351
//Assignment #1
//Serial downloader
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	FILE *fp;
	int nbytes = 200;
	char url[10][200];
	fp = fopen("urls2.txt", "r");
	int n = 0;	
	int c;

	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open file urls.txt! \n");
		exit(1);
	}
	
	do {
		c = fgetc(fp);
		if (c == '\n'){
			n++;}
	} while (c != EOF);
	rewind(fp);
	printf("Number of lines in file: %i\n", n);
	for (c = 0; c < n; ++c)
	{
		fscanf(fp, "%s", url[c]);
		if (url[c][0] == 'h' && url[c][1] == 't') {
			pid_t pid[c];
			pid[c] = fork();
			if (pid[c] < 0) {
				printf("Fork Failed\n");
				exit(-1);
			}
			else if (pid[c] == 0)  {
				execlp("/usr/bin/wget", "wget", url[c], NULL);
			}
			else {
				wait (NULL);
				printf("Child Complete\n");
			}
		}
	}

	fclose(fp);
}
