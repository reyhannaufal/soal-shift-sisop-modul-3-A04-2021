// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 


	int fd1[2], fd2[2];
 
	pid_t p, m; 

	if (pipe(fd1)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 

    if (pipe(fd2)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 

	p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 

	// Parent process 
	else if (p > 0) 
	{ 

        dup2(fd1[1], 1);

		close(fd1[0]); 
		close(fd1[1]); 
        close(fd2[0]); 
		close(fd2[1]); 

        char *argv1[] = {"ps", "aux", NULL};
        execv("/bin/ps", argv1);

	} 

	// child process 
	else
	{
        wait(NULL);
        m = fork(); 

        if (m < 0) 
        { 
            fprintf(stderr, "fork Failed" ); 
            return 1; 
        } 

        else if(m > 0) 
        {
            dup2(fd1[0], 0);
            close(fd1[1]); 
            close(fd1[0]); 

            dup2(fd2[1], 1);
            close(fd2[0]); 
            close(fd2[1]);
            char *argv1[] = {"sort", "-nrk", "3,3", NULL};
            execv("/bin/sort", argv1);


        }
        else
        {
            wait(NULL);
            dup2(fd2[0], 0);

            close(fd1[1]); 
            close(fd1[0]);
            close(fd2[0]); 
		    close(fd2[1]); 

            char *argv1[] = {"head", "-5", NULL};
            execv("/bin/head", argv1);
            exit(0);
        }        
    }
} 