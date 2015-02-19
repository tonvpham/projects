//Tony Pham
//CPSC 351
//Assignment #1
//Parallel Downloader 

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	FILE *fp;					//File pointer
	char url[10][200];			//Array used to store each url from file
	int n = 0;					//Counter variable
	int c;						//Holds the index to access urls in the array
	
	fp = fopen("urls.txt", "r");
	
	//Error check to see if file was opened correctly
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open file urls.txt! \n");
		exit(1);
	}
	//Get the line(url) count from the file
	do {
		c = fgetc(fp);
		if (c == '\n'){
			n++;}
	} while (c != EOF);
	
	//Set file pointer back to the start of the file
	rewind(fp);
	printf("Number of lines in file: %i\n", n);
	
	//Iterate through all urls in the file
	for (c = 0; c < n; ++c)
	{
		fscanf(fp, "%s", url[c]);
		
		//Check to see if it is a valid url
		if (url[c][0] == 'h' && url[c][1] == 't') {
			pid_t pid[c];
			
			//Fork a process for each url
			pid[c] = fork();
			if (pid[c] < 0) {
				printf("Fork Failed\n");
				exit(-1);
			}
			
			//Execute wget command per url
			else if (pid[c] == 0)  {
				execlp("/usr/bin/wget", "wget", url[c], NULL);
			}
		}
	}

	/*Instead of calling wait after each iteration the parent waits until it reaches the last url*/
	while (n > 0) {
		wait(NULL);
		--n;
	}
	
	
	fclose(fp);
}
