#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_LEN 1024
#define MAX_COMMANDS 35
/*
Students:

Isaac Fabian Palma Medina 1 1865 0422
Karla Verónica Quiros Delgado 6 0465 0870
*/
void showShell()
{
    printf("\033[0;31m");
    printf("PalmaQuirosShell>> ");
    printf("\033[0m");
}
// Counts the valid characters in a char array
int countChars(char str[]) {
    int num = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        num++;
    }
    return num - 1;
}
// Checks for valid inputs (starting with ! and followed by a number; does not allow spaces, characters, or letters)
bool checkDigits(char *input) {
    int length = countChars(input);
    for (int i = 1; i < length; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true && input[0] == '!';
}

// viewHistory: function that returns the last entered commands within a range of 1–35 (the last 35)
void viewHistory(int* current_command, int * counter, char ** history)
{
    int numCommands = *counter < MAX_COMMANDS ? *counter : MAX_COMMANDS;
    for (int i = numCommands - 1; i >= 0; i--)
    {
        int index = (*current_command - numCommands + i + MAX_COMMANDS) % MAX_COMMANDS;
        printf("%d: %s", i + 1, history[index]);
    }
}

// requestedCommand: checks whether the entered command number exists in the history; if it does, it displays it, otherwise it shows a message
void requestedCommand(char *input, int *counter, char **history) {
    int cont = *counter;
    int commandNum = atoi(input + 1); // Convert the numeric part of the input to an integer
    
    if(!checkDigits(input)){
        printf("Input does not meet the required format.\n");
        return;
    }
    else if (commandNum < 1 || commandNum > 35 ) {
        printf("Requested command number is out of history range (1-35)\n");
        return;
    }
    if (cont == 0){
        printf("History is empty.\n"); 
        return; 
    } else if (commandNum > cont) {
        printf("No command exists at that position\n");
        return;
    }
    printf("%s", history[commandNum - 1]);
}

// lastIsAnd: checks if the last argument in the input is &
bool lastIsAnd(char **args)
{
    int i = 0;
    while (args[i + 1] != NULL)
    {
        i++;
    }
    return strcmp(args[i], "&") == 0;
}

// tokenize: splits a string into tokens according to a delimiter
void tokenize(char *str, const char *delim, char **arr1, char **arr2)
{
    char *token;
    int pos = 0;
    bool piped = false;
    token = strtok(str, delim);
    while (token != NULL)
    {
        if (*token == '|'){
            piped = true;
            pos = 0;
        }
        else{
            if (piped == true){
                arr2[pos] = token;
                pos++;
            }
            else{
                arr1[pos] = token;
                pos++;
            }
        }
        token = strtok(NULL, delim);
    }
}

void initializeHistory(char** history) {
    for (int i = 0; i < MAX_COMMANDS; i++) {
        history[i] = malloc(MAX_LEN * sizeof(char));
    }
}

int main(int argc, char *argv[]) {

    char** history = malloc(MAX_COMMANDS * sizeof(char*));
    initializeHistory(history);
    
    int* current_command = malloc(sizeof(int));
    int* counter = malloc(sizeof(int));
    *current_command = 0;
    *counter = 0;
    char input[MAX_LEN]; // User input

    for (;;) 
    {
        showShell();

    verify:
        if (fgets(input, sizeof(input), stdin) == NULL || input[0] == '\n') // Get user input
        {
            // In case the input is invalid
            showShell();
            goto verify;
        }

        if (strcmp(input, "exit\n") == 0){break;}
        // Option when the user enters !N where N is the command number to retrieve from history
        else if (input[0] == '!')
        {
            if (input[2]=='\0'){
                printf("You need a command number to search, following the ! \n");
            }
            else{requestedCommand(input,counter,history);}
        }
        else if (strcmp(input, "historial\n") == 0 || strcmp(input, "historial &\n") == 0)
        {
            viewHistory(current_command,counter,history);
        }
        else
        {
            if (*counter >= MAX_COMMANDS)
            {
            for (int i = 0; i < MAX_COMMANDS - 1; i++)
                {
                    strcpy(history[i], history[i + 1]);
                }
                strcpy(history[MAX_COMMANDS - 1], input);
               
            }
            else{
                strcpy(history[*current_command], input);
                *current_command = (*current_command + 1) % MAX_COMMANDS;
                *counter = *counter + 1;
            }

            char *arg1[MAX_LEN];
            char *arg2[MAX_LEN];

            for (int i = 0; i < MAX_LEN; i++){ arg1[i] = NULL;}
            for (int i = 0; i < MAX_LEN; i++){arg2[i] = NULL;}

            tokenize(input, " \n", arg1, arg2);

            if (arg2[0] == NULL)
            {
                bool background = lastIsAnd(arg1);
                int pid = fork();
                if (pid < 0)
                {
                    printf("Error creating child process.\n");
                    exit(1);
                }
                else if (pid == 0)
                {
                    // Child process
                    if (background)
                    {
                        // Remove "&" from argument array
                        int j = 0;
                        while (arg1[j + 1] != NULL)
                        {
                            j++;
                        }
                        arg1[j] = NULL;
                        sleep(2);
                        printf("\nChild process says:\n");
                    }

                    execvp(arg1[0], arg1);

                    printf("\tError executing command, invalid command.\n");
                    exit(0);
                }
                else
                {
                    // Parent process
                    if (!background)
                    { 
                        int status;
                        waitpid(pid, &status, 0);
                    }
                    else
                    { // Do not wait for child process
                        printf("Background process.\n");
                    }
                }
            }
            else
            {
                int pipeEnds[2];
                int pipeStatus = pipe(pipeEnds);
                if (pipeStatus == -1)
                {
                    exit(1);
                }
                int pid1 = fork();
                if (pid1 == 0)
                {
                    close(STDOUT_FILENO);
                    dup(pipeEnds[1]);
                    close(pipeEnds[0]);
                    close(pipeEnds[1]);

                    execvp(arg1[0], arg1);
                    printf("\tError executing command, invalid command.\n");
                    exit(0);
                }
                int pid2 = fork();
                if (pid2 == 0)
                {
                    close(STDIN_FILENO);
                    dup(pipeEnds[0]);
                    close(pipeEnds[1]);
                    close(pipeEnds[0]);

                    execvp(arg2[0], arg2);
                       printf("\tError executing command, invalid command.\n");
                    exit(0);
                }

                close(pipeEnds[0]);
                close(pipeEnds[1]);
                int status1;
                int status2;
                waitpid(pid1, &status1, 0);
                waitpid(pid2, &status2, 0);
            }
        }
    }
    free(counter);
    free(current_command);
    free(history);
    return 0;
}
/*
References:

https://www.geeksforgeeks.org/piping-in-unix-or-linux/
https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection
https://cplusplus.com/reference/cstring/strcmp/
https://www.programacion.com.py/escritorio/c/pipes-en-c-linux
https://stackoverflow.com/questions/8058704/how-to-detect-empty-string-from-fgets
https://www.cs.swarthmore.edu/~newhall/unixhelp/c_codestyle.html
https://linux.die.net/man/3/execlp
https://stackoverflow.com/questions/40605075/how-to-compare-2-character-arrays

*/
