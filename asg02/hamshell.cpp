#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;


//---------------------------------------------------------------------//
//-------------------Tokeniztion functions Proto-----------------------//
//---------------------------------------------------------------------//
int Tokenize_Pipe(char *input, char **output);
int Tokenize_GreaterThan(char *input, char **output);
void Tokenize_Space(char *input, char **output);

//---------------------------------------------------------------------//
//--------------------Execution functions Proto------------------------//
//---------------------------------------------------------------------//
void Execute_Pipe(char *input, char *output);
void Execute_ioRedirect(char **input, char *output);
void Execute_Command(char **input);

//---------------------------------------------------------------------//
//-----------------------------Constants-------------------------------//
//---------------------------------------------------------------------//
const char *shell = "HamShell$ ";
const int MAX_INPUT = 100;
const int PIPE_REDIRECT_TOTAL_ARGS = 2;
const int IO_REDIRECT_TOTAL_ARGS = 2;




//---------------------------------------------------------------------//
//-----------------------Main Shell (HamShell)-------------------------//
//---------------------------------------------------------------------//
int main() {
    // Shell input and exit condition variables
    char shell_Input[MAX_INPUT];
    int exit_while = 0;


    // Variable initialization
    char *command_args[10];
    char *IO_args[4];
    char io_IN[25];
    char io_OUT[25];
    char *pipe_args[10];
    char pipe_IN[25];
    char pipe_OUT[25];

    // Stay in shell unless told to exit
    while (!exit_while) {

        // Shell interface I/O
        printf(shell);                              // Print shell user
        fgets(shell_Input, MAX_INPUT, stdin);       // get standard input
        shell_Input[strcspn(shell_Input, "\n")] = NULL;


        // Exit shell if user prints exit
        if (strcmp(command_args[0], "exit") == 0) {
            exit_while = 1;
        }






        //---------------------------------------------------------------------//
        //-----------------------------Processes-------------------------------//

        // Process tokenized ">"
        // (Little jank now trying to implement last example)
        else if (Tokenize_GreaterThan(shell_Input, IO_args) == IO_REDIRECT_TOTAL_ARGS) {
            
            // Setup input and output from tokenized IO redirect
            strcpy(io_IN, IO_args[0]);
            strcpy(io_OUT, IO_args[1]);

            char temp_io_IN[25];
            strcpy(temp_io_IN, io_IN);

            Tokenize_Space(io_IN, command_args);



            if (Tokenize_Pipe(temp_io_IN, pipe_args) == PIPE_REDIRECT_TOTAL_ARGS) {
                // Setup input and output from tokenized pipe
                strcpy(pipe_IN, pipe_args[0]);
                strcpy(pipe_OUT, pipe_args[1]);


                // Setup for pipe and io using part of IO redirect function
                int temp_Descriptor = open(io_OUT, O_CREAT|O_RDWR|O_TRUNC, 0644); 
                int id; 




                // Pipe and IO redirect modifcation
                char *temp_args[10];
                int fds[2];
                pipe(fds);
                // Child 1 will duplicate downstream into stdin
                if(fork() == 0) {
                    dup2(fds[0], 0); // normally 0 is for stdin
                            // will now read to fds[0]
                            // end of pipe 
                    close(fds[1]);   // close other end of pipe
                    Tokenize_Space(pipe_OUT, temp_args);
                    dup2(temp_Descriptor, 1);
                    Execute_Command(temp_args);
                    close(temp_Descriptor);

                    exit(0); 

                }
                // Child2 duplicates upstream into stdout
                else if (fork() == 0) {
                    dup2(fds[1], 1); // normally 1 is for stdout
                            // will now write to fds[1] 
                            // end of pipe
                    close(fds[0]);   // close other end of pipe
                    Tokenize_Space(pipe_IN, temp_args);
                    Execute_Command(temp_args);

                    exit(0); 
                }
                // Parent
                else {
                    close(fds[0]);
                    close(fds[1]);
                    wait(0);
                    wait(0);
                }




            }

            // If the last example doesn't exist, process normally
            else {
                // Call function to process IO redirection
                Execute_ioRedirect(command_args, io_OUT);
            }


        }


        // Process tokenized "|"
        else if(Tokenize_Pipe(shell_Input, pipe_args) == PIPE_REDIRECT_TOTAL_ARGS) {
            // Setup input and output from tokenized pipe
            strcpy(pipe_IN, pipe_args[0]);
            strcpy(pipe_OUT, pipe_args[1]);



            Execute_Pipe(pipe_IN, pipe_OUT);


        }

        
        // Process tokenized " "
        else if (!(shell_Input[0] == 0)){
            
            Tokenize_Space(shell_Input, command_args);

            // Call function to execute command
            Execute_Command(command_args);

        }
        


        
    }
    
    return 0;
}







//---------------------------------------------------------------------//
//-----------------------Tokeniztion functions-------------------------//
//---------------------------------------------------------------------//
int Tokenize_Pipe(char *input, char **output) {
    int pipe_ctr = 0;

    // Tokenize ">"
    char *token_gt = strtok(input, "|");

    while (token_gt != NULL) {
        output[pipe_ctr] = token_gt;
        token_gt = strtok(NULL," "); //include <space> inside "" 
        pipe_ctr++;
    }
    output[PIPE_REDIRECT_TOTAL_ARGS] = NULL;

    return pipe_ctr;
}


int Tokenize_GreaterThan(char *input, char **output) {
    int io_ctr = 0;

    // Tokenize ">"
    char *token_gt = strtok(input, ">");

    while (token_gt != NULL) {
        output[io_ctr] = token_gt;
        token_gt = strtok(NULL," "); //include <space> inside "" 
        io_ctr++;
    }
    output[IO_REDIRECT_TOTAL_ARGS] = NULL;

    return io_ctr;
}



void Tokenize_Space(char *input, char **output) {
    int command_ctr = 0;
    
    // Tokenize " "
    char *token_Spaces = strtok(input, " ");
    
    while (token_Spaces != NULL) {
        //printf("tokenized: %s\n", token_Spacesd);
        output[command_ctr] = token_Spaces;
        token_Spaces = strtok(NULL," "); //include <space> inside "" 
        command_ctr++;
    }
    output[command_ctr] = NULL;

    return;
}





//---------------------------------------------------------------------//
//------------------------Execution functions--------------------------//
//---------------------------------------------------------------------//
void Execute_Pipe(char *input, char *output) {
    char *temp_args[10];
    int fds[2];
    pipe(fds);
    // Child 1 will duplicate downstream into stdin
    if(fork() == 0) {
        dup2(fds[0], 0); // normally 0 is for stdin
                // will now read to fds[0]
                // end of pipe 
        close(fds[1]);   // close other end of pipe
        Tokenize_Space(output, temp_args);
        Execute_Command(temp_args);

        exit(0); 

    }
    // Child2 duplicates upstream into stdout
    else if (fork() == 0) {
        dup2(fds[1], 1); // normally 1 is for stdout
                // will now write to fds[1] 
                // end of pipe
        close(fds[0]);   // close other end of pipe
        Tokenize_Space(input, temp_args);
        Execute_Command(temp_args);

        exit(0); 
    }
    // Parent
    else {
        close(fds[0]);
        close(fds[1]);
        wait(0);
        wait(0);
    }

    return;
}


void Execute_ioRedirect(char **input, char *output) {
    int myDescriptor = open(output, O_CREAT|O_RDWR|O_TRUNC, 0644); 
    int id; 
    // creating a child that redirects the stdout to test.txt 
    // you can use similar functionality for '>' operator 
    if((id = fork()) == 0) { 
        //lets call dup2 so that out stdout (second argument) is now copied to (points to) test.txt (first argument) 
        // what this essentially means is that anything that you send to stdout will be sent to myDescriptor 
        dup2(myDescriptor, 1); //1 is stdout, 0 is stdin and 2 is stderr 
        // printf("You would expect this to go to your stdout, but since we called dup2, this will go to test.txt"); 
        Execute_Command(input);
        close(myDescriptor); 
        exit(0); 
    }
    else { 
        wait(0); 
    }

    return;
}


void Execute_Command(char **input) {
    // Execute of command
    int id;
    
    //I will spawn a child that will run my execvp command 
    if((id = fork()) == 0) {
        execvp(input[0], input);
        printf("Invalid Command: %s\n", input[0]);
        exit(1);
    }
    else if(id < 0) 
        printf("Failed to make child...\n"); 
    else 
    { 
        //parent shall wait until the child is killede
        wait(0); 
    }
}


