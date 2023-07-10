#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

const int max = 1000;
char buffer[max];
char* parsedCommands[max];
char history_storage[max][max][max];
int his_ind=0;

void parseCommand(char* buffer);
void printCommands();
bool isInternalCommand();
void executeInternalCommand();
void executeFromUsrBin();
void cd();
void prependUserBin();
void AddToHistory();
void clear();

int main(){

    while(true) {

        char cwd[1024];

        // Get the current working directory.
        getcwd(cwd, sizeof(cwd));

        printf("My-Shell:%s$ ", cwd);
        fflush(stdout);

        fgets(buffer, 1000, stdin); // overwrites the buffer code everytime

        // Consume the newline character.
        if (strlen(buffer) > 0) {
            buffer[strlen(buffer) - 1] = '\0';
        }

        if (strlen(buffer) <= 0) {
            continue;
        }

        parseCommand(buffer);
        AddToHistory();
        if (isInternalCommand()) {
            executeInternalCommand();
        }else {
            executeFromUsrBin();
        }

    }    

    return 0;
}

void cd() {
    char* new_dir = parsedCommands[1];
    int ret = chdir(new_dir);

    if (ret == -1) {
        perror("cd");
    }
}

void clear() {
    his_ind=0;
}

void history(){
    for(int i = 0; i < his_ind; i++){
        int j = 0;
        printf("%d ", i + 1);
        while(strlen(history_storage[i][j]) > 0){
            printf("%s ", history_storage[i][j++]);
        }
        printf("\n");
    }
}

void AddToHistory(){
    int i = 0;
    while(parsedCommands[i] != NULL){
        strcpy(history_storage[his_ind][i], parsedCommands[i]);
        i++;
    }
    history_storage[his_ind][i][0]='\0';
    his_ind++;
}

bool isInternalCommand() {
    if (strcmp(parsedCommands[0], "cd") == 0) {
        return true;
    }
    if (strcmp(parsedCommands[0], "clear") == 0) {
        return true;
    }
    if (strcmp(parsedCommands[0], "history") == 0) {
        return true;
    }
    return false;
}

void executeInternalCommand() {
    if (strcmp(parsedCommands[0], "cd") == 0) {
        cd();
    }
    else if (strcmp(parsedCommands[0], "clear") == 0) {
        clear();
    }
    else if (strcmp(parsedCommands[0], "history") == 0) {
        history();
    }
    else {
        printf("Command not found");
    }
}

void executeFromUsrBin() {
    
    prependUserBin();

    if (fork() == 0) {
        if (execv(parsedCommands[0], parsedCommands) < 0) {
            perror("my shell");
        };
        // child process will come here only when execv is not executed
        exit(0); // it will end the child process and parent process will continue;
    }else {
        wait(NULL);
    }
}

void prependUserBin() {
    char* binary_name = parsedCommands[0];
    char full_path[1024];
    strcpy(full_path, "/usr/bin/");
    strcat(full_path, binary_name);
    parsedCommands[0] = full_path;
}

void parseCommand(char* buffer) {

    char *token;
    char delimiters[] = " "; // decided the breakpoint
    token = NULL;

    // Iterate through the buffer, one character at a time
    int i = 0;
    while ((token = strtok(buffer, delimiters)) != NULL) {
        // entire buffer has already been read by strtok
        buffer = NULL;
        parsedCommands[i++] = token;
    }

    parsedCommands[i] = NULL;

}

void printCommands() {
    
    int i = 0;
    while(parsedCommands[i] != NULL) {
        printf("%s ", parsedCommands[i++]);
    }
}
