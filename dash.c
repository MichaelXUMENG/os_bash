/*************************
 Add the description of your project here
 
 *************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CMD_BUFSIZE 1024
#define MAX_CMD 8
#define MAX_PARA 5
#define MAX_CMDL 16
#define MAX_STRING 32
char PATH[256];
char ENVPATH[16][MAX_STRING]={
    "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"
};
// initiate the command array with empty string (limite 8 command within a single line)
char cmd[MAX_CMD][MAX_CMDL];
// initiate the parameter array with empty string (limite 5 parameters for each command)
char cmd_para[MAX_CMD][MAX_PARA][MAX_STRING];


struct execute_count
{
    int cmd_count;
    int para_count[MAX_CMD];
};


void initialize_array(void)
{
    //assign empty string value to both the command and parameter array
    for (int i=0; i<MAX_CMD; i++){
        //Empty the string, and then assign an empty value to the string
        memset(cmd[i],0,strlen(cmd[i]));
        strcpy(cmd[i], "\0");
        for (int j=0; j<MAX_PARA; j++){
            //strcpy(cmd_para[i][j], "\0");
            memset(cmd_para[i][j],0,strlen(cmd_para[i][j]));
            strcpy(cmd_para[i][j], "\0");
        }
    }
}


struct execute_count prepare_cmd(char *line)
{
    // declare and initiate the pointers
    int line_cursor=0;
    int curCmd=0;
    int curPar=0;
    int base=0;
    int isWord=0;
    struct execute_count result;
    
    while (line[line_cursor]!='\n') {
        // when a space is reached
        if (line[line_cursor]==' '){
            // make sure there are characters appearing before this empty space; otherwise skip
            if (isWord){
                // if the command array is empty, then save the word in command
                if (strcmp(cmd[curCmd], "\0")==0){
                    memcpy(cmd[curCmd], &line[base], line_cursor-base);
                }
                //otherwise save it into parameter array
                else{
                    // increase the parameter pointer by 1
                    memcpy(cmd_para[curCmd][curPar++], &line[base], line_cursor-base);
                }
                // move the word base to the next character of the empty space
                base = line_cursor+1;
            }
        }
        // when a '&' is reached, move to the next command, and re-count the parameter
        else if (line[line_cursor]=='&'){
            result.para_count[curCmd]=curPar;
            curCmd++;
            curPar=0;
            isWord=0;
            base = line_cursor+1;
        }
        // characters are reached
        else{
            // set this variable to true
            isWord=1;
        }
        line_cursor++;
    }
    //after the '/n' is reached, if there is a word, save it to either command or parameter
    if (isWord){
        if (strcmp(cmd[curCmd], "\0")==0){
            memcpy(cmd[curCmd++], &line[base], line_cursor-base);
        }
        else{
            result.para_count[curCmd]=curPar+1;
            memcpy(cmd_para[curCmd++][curPar++], &line[base], line_cursor-base);
        }
    }
    // print out the command and parameters captured. Can be deleted before submitting
    for (int i=0; i<8; i++){
        if (strcmp(cmd[i], "\0")!=0){
            printf("Comment%d is: %s\n", i, cmd[i]);
            for (int j=0; j<5; j++){
                if (strcmp(cmd_para[i][j], "\0")!=0){
                    printf("Parameter%d of %d is: %s  ", j, i, cmd_para[i][j]);
                }
                else{
                    break;
                }
            }
            printf("\n");
        }
        else{
            break;
        }
    }
    result.cmd_count=curCmd;
    return result;
}


void execute_cmd(char *line)
{
    printf("The line passed in as: %s\n", line);
    initialize_array();
    char *executable[7];
    char exe_cmd[32];
    char* str=NULL;
    
    // Prepare the command array and parameter array, and get the number of command and parameter
    struct execute_count count_set=prepare_cmd(line);
    int input_cmd=count_set.cmd_count;
    int *input_para=count_set.para_count;
    printf("There is %d commands\n", input_cmd);
    
    for (int i=0; i<input_cmd; i++){
        int PATH_INDEX=0;
        int avaliable=0;
        while (strcmp(ENVPATH[PATH_INDEX], "\0")!=0){
            memset(exe_cmd,0,strlen(exe_cmd));
            strcpy(exe_cmd, ENVPATH[PATH_INDEX]);
            strcat(exe_cmd, "/");
            strcat(exe_cmd, cmd[i]);
            //strcat(exe_cmd, "\0");
            printf("exe_cmd is %s\n", exe_cmd);
            if (access(exe_cmd, X_OK)==0){
                printf("Have access\n");
                avaliable=1;
                break;
            }
            PATH_INDEX++;
        }
        if (avaliable){
            int exe_idx=1;
            printf("Step 2.1\n");
            executable[0] = malloc(sizeof(char) * 32);
            //memset(executable[0],0,strlen(executable[0]));
            strcpy(executable[0], exe_cmd);
            printf("executable[0] is %s\n", executable[0]);
            while (strcmp(cmd_para[i][exe_idx-1], "\0") != 0){
                printf("Step 2.%d\n", exe_idx+1);
                executable[exe_idx] = malloc(sizeof(char) * 32);
                //memset(executable[exe_idx],0,strlen(executable[exe_idx]));
                strcpy(executable[exe_idx], cmd_para[i][exe_idx-1]);
                //executable[exe_idx]=cmd_para[i][exe_idx-1];
                printf("executable[%d] is %s\n", exe_idx, executable[exe_idx]);
                exe_idx++;
            }
            printf("final Step\n");
            executable[exe_idx] = malloc(sizeof(char) * 32);
            //memset(executable[exe_idx],0,strlen(executable[exe_idx]));
            //strcpy(executable[exe_idx], "\0");
            executable[exe_idx]=str;
            //executable[exe_idx]=NULL;
            printf("The final executable is: %s, %s, %s, %s\n", executable[0], executable[1],executable[2],executable[3]);
            
            int child_process=fork();
            if (child_process<0){ // FORK FAILLED
                printf("Fork FAILED!!!!\n");
            }
            else if (child_process==0){ // Child Process
                printf("CHILD PROCESS HERE!\n");
                printf("Command to be execute is: %s\n", executable[0]);
                execv(executable[0], executable);
                printf("CHILD PROCESS FINISHED!\n");
            }
            else{
                printf("\n");
            }
        }
    }
}


int update_status(char *line)
{
    if (strcmp("exit\n", line) == 0){
        return 0;
    }
    else{
        return 1;
    }
}


char *read_line(void)
{
    char *buffer;
    size_t bufsize = CMD_BUFSIZE;
    buffer = malloc(sizeof(char) * bufsize);
    if( buffer == NULL){
        perror("Unable to allocate buffer");
        exit(1);
    }
    getline(&buffer, &bufsize,stdin);
    return buffer;
}


void dash_cmdLine(void)
{
    char *line;
    int status=1;
    int pid;
    char *args[]={"/bin/ls", "-a", "-l", NULL};
    
    do{
        printf("dash> ");
        line = read_line();
        execute_cmd(line);
        /*
        int pid=fork();
        if (pid<0){
            printf("DEMO FORK FAILLED!");
        }
        else if (pid==0){
            printf("CHILD PROCESS");
            execv(args[0], args);
        }
        else{
            printf("PARENT PROCESS");
        }
        */
        status = update_status(line);
        printf("Status is : %d\n", status);
    } while (status);
}


int main(int argc, char **argv) {
    // read the command from user input
    if (argc<=1){
        FILE *fp;
        fp = fopen(".path", "r");
        if (fp == NULL){
            printf("Cannot open file \n");
            exit(0);
        }
        
        //save the line read from '.path' file into PATH
        fscanf(fp,"%s",PATH);
        fclose(fp);
        
        //initiate the i as index at 0
        //initiate curP as Current Pointer to point to the curent index of ENVPATH array
        //initiate the start point(index) of the current path
        int i=0;
        int curP=0;
        int base=0;
        while (PATH[i]!='\0') {
            if (PATH[i]==':'){
                memcpy(ENVPATH[curP], &PATH[base], i-base);
                base = i+1;
                curP++;
            }
            i++;
        }
        memcpy(ENVPATH[curP], &PATH[base], i-base);
    
        dash_cmdLine();
    
        return EXIT_SUCCESS;
    }
    else{
        printf("This is the section for readin file.\n");
    }
}

