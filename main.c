#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

int main()
{
    FILE *input_file = fopen("input.txt", "r");			//Open input and output files
    FILE *output_file = fopen("output.txt", "w");
    if (input_file == NULL || output_file == NULL)		//Check if they opened correctly
    {
        printf("Error opening file");
        return 1;
    }
    int pipe1[2], pipe2[2];					//Declare pipe file descriptors
    if (pipe(pipe1) == -1) {                                                             //Create both pipes and check for errors
           perror("Pipe 1 Error");
           return 2;
    } else{
          printf("Created Pipe 1 Successfully");
    }
    if (pipe(pipe2) == -1) {
           perror("Pipe 2 Error");
           return 2;
    } else{
      printf("Created Pipe 2 Successfully");
    }				
    
    int child_a, child_b;				//Declare both pids
    child_a = fork();					//Fork first child
    if (child_a == 0)
    {
        /* Child a code */				//Child 1 Code
        close(pipe1[1]);
        close(pipe2[0]);				              //Close unused pipes
        
        char c;
        while (read(pipe1[0], &c, 1) > 0)			//Read from pipe1
        {
          if (isupper(c))				      //Perform uppercase to lower, lower to upper conversion
           {
               c = tolower(c);
           }
           else if (islower(c))
           {
               c = toupper(c);
           }
           write(pipe2[1], &c, 1);				         //Write back to pipe2
        }
        close(pipe1[0]);
        close(pipe2[1]);                                                             //Close remaining pipes
        return 0;                                                                         //CHECK???  break loop if read() returns 0
    }
    else
    {							        //Parent Process
        child_b = fork();					       //Fork second child
        if (child_b == 0)
        {
            /* Child B code */				    //Child 2 Code
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[1]);                		               //Close unused pipes

            char c;
            while (read(pipe2[0], &c, 1) > 0)		//read from second pipe
            {
                fwrite(&c, 1, 1, output_file);	   	//output to file
            }
            close(pipe2[0]);                                                    //Close unused pipes
            fclose(output_file);	                                       //Close output file
            return 0;                                                               //CHECK?   break loop if read() returns 0    
         }
        else
        {
            /* Parent code */				//Parent Process
            close(pipe1[0]);
            close(pipe2[0]);		   		
            close(pipe2[1]);                                                    //Close unused pipes
            
            char c;
            while (fscanf(input_file, "%c", &c) == 1)	//Read from input file
            {
                write(pipe1[1], &c, 1);			//Write to pipe1.
            }
            close(pipe1[1]);                                                    //Close unused pipes
            fclose(input_file);			            //Close input file

            wait(NULL);                                                           // Wait for child A to finish
            wait(NULL);                                                          // Wait for child B to finish
        }
    }
    return 0;
}

