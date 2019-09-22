/*************************
 Add the description of your project here
 
 *************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

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


/*
 Define the data structure used to save count of command and parameters
 **/
struct execute_count
{
    int cmd_count;
    int para_count[MAX_CMD];
};


/*
 The built-in exit function
 Exit the shell
 **/
void builtin_exit(void)
{
    exit(0);
}


/*
 The built-in cd function
 take directory as parameter
 **/
void builtin_cd(char *directory)
{
    int result=chdir(directory);
    if (result == -1){
        printf("Error changing directory!");
    }
}


/*
 The built-in path function.
 Take path as parameters
 **/
void builtin_path(char paths[5][32])
{
    //printf("THe first path is %s\n", paths[0]);
    for (int i=0; i<16; i++){
        //strcpy(cmd_para[i][j], "\0");
        //printf("Clean the space %d\n", i);
        memset(ENVPATH[i], 0, MAX_STRING);
        strcpy(ENVPATH[i], "\0");
    }
    for (int i=0; strcmp(paths[i], "\0")!=0; i++){
        //printf("Assign the new path %d\n", i);
        strcpy(ENVPATH[i], paths[i]);
        //printf("The new Path %d is: %s\n", i, ENVPATH[i]);
    }
}


/*
 The function is used to save the output into a file
 **/
void redirectOut(char* fileName)
{
    int out = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    dup2(out,1);
    close(out);
    out = 0;
}


/*
 initialize the command array and 2 dimentional parameter array
 Whenever there is a new input line, the 2 arrays will be initialized
 **/
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


/*
 This function will parse the input line into command and parameters
    1. save the commands into the command array
    2. save the parameters into the 2 dimentinal parameter array; First level is for the command; second level is for the parameter
    3. count the number of command and number of parameter of each command while reading input line
    4. Save the count of command and corresponding parameters into data structure 'execute_cout'
    5. Then return the 'execute_cout' data structure variable
 **/
struct execute_count prepare_cmd(char *line)
{
    // declare and initiate the pointers
    int line_cursor=0; // This is the index variable of the input line
    int curCmd=0; // The index variable for current command (which position the command should be saved); also used in the parameter array
    int curPar=0;// The index variable for the current position where parameter should save
    int base=0; // The start index of the word
    int isWord=0; // flag which is used to determine if a word is reached
    struct execute_count result; // define the data structure used to save the count of command and parameters
    
    // Keep reading the character and process the result as long as the current character is not '\n'
    while (line[line_cursor]!='\n') {
        // when a space is reached
        if (line[line_cursor]==' '){
            // make sure there are characters appearing before this empty space(not just blank spaces); otherwise skip
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
            }
            // move the word base to the next character of the current index (blank space)
            base = line_cursor+1;
        }
        // when a '&' is reached, move to the next command, and re-count the parameter
        else if (line[line_cursor]=='&'){
            result.para_count[curCmd]=curPar; // save the count of parameter into the parameter array of that command
            curCmd++; // increase the index of command
            curPar=0; // reset the parameter index
            isWord=0; // reset the word flag
            base = line_cursor+1; // move the index of input line to the next position
        }
        // characters are reached
        else{
            // set this variable to true
            isWord=1;
        }
        line_cursor++; // move the index to the next
    }
    //after the '/n' is reached, if there is a word, save it to either command or parameter
    if (isWord){
        // if current command is empty, save the word into command
        if (strcmp(cmd[curCmd], "\0")==0){
            // and increase the count of command
            memcpy(cmd[curCmd++], &line[base], line_cursor-base);
        }
        else{
            // save the count of parameter into the array of that command
            result.para_count[curCmd]=curPar+1;
            // and increase the count of parameter
            memcpy(cmd_para[curCmd++][curPar++], &line[base], line_cursor-base);
        }
    }
    // print out the command and parameters captured. Can be deleted before submitting
    /*
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
    */
    // save the count of command into the array
    result.cmd_count=curCmd;
    // then return the structure result
    return result;
}


/*
        The execute command function
 This function contains the following steps:
    1. parse the input line into 2 arrays: commands array cmd; parameter array cmd_para;
    2.0 check each command from cmd
    2.1 if the command is built-in function, then execute the command directly (also passing the parameter)
    2.2 otherwise, check the command with the PATH environment variable, to see if user have access to the command
    3.0 if user doesn't have access to the command, then escape the command and parameter pair
    3.1 otherwise, execute the command, passing the parameter, in child process
    4. Keep looping through the command array till the end of array
 */
void execute_cmd(char *line)
{
    //printf("The line passed in as: %s\n", line);
    // initialize the 2 arrays, command and parameter
    initialize_array();
    
    // define the useful variables
    char *executable[7]; // the string array which will be used in execv();
    char exe_cmd[32]; // the string variable which will be used to store the command with PATH
    char* str=NULL; // define the NULL string to attach to the end of executable
    int redirect; // the flag varaible used to mark if the result will be saved in the redirected file
    char fileName[32]; // the string variable to save the file name to be used in redirect
    
    // Prepare the command array and parameter array, and get the number of command and parameter
    struct execute_count count_set=prepare_cmd(line); // define the data structure to save cmd and parameter count
    int input_cmd=count_set.cmd_count; // save the commands count
    int *input_para=count_set.para_count; // save the parameter counts into integer array
    //printf("There is %d commands\n", input_cmd);
    
    // loop through the command array, to check if the command will be executed
    for (int i=0; i<input_cmd; i++){
        int PATH_INDEX=0; // the index used in PATH environment
        int avaliable=0; // the flag used to mark if the non-builtin function has access
        
        // Built-in function 'exit'
        if (strcmp(cmd[i], "exit") == 0){
            builtin_exit();
        }
        // Built-in function 'cd'
        else if (strcmp(cmd[i], "cd") == 0){
            if (strcmp(cmd_para[i][1], "\0") != 0){
                printf("Only 1 parameter is allowed!! This 'cd' command will not be executed\n");
            }
            else if (strcmp(cmd_para[i][0], "\0") == 0){
                printf("The 'cd' command should take 1 parameter\n");
            }
            else{
                builtin_cd(cmd_para[i][0]);
            }
        }
        // Built-in function 'path'
        else if (strcmp(cmd[i], "path") == 0){
            builtin_path(cmd_para[i]);
        }
        // non-builtin functions
        else{
            // attach the command to each PATH environment variable to see if user have the access
            while (strcmp(ENVPATH[PATH_INDEX], "\0")!=0){
                memset(exe_cmd,0,strlen(exe_cmd)); // empty the exe_cmd variable
                strcpy(exe_cmd, ENVPATH[PATH_INDEX]); // copy the PATH variable to exe_cmd
                strcat(exe_cmd, "/"); // attach '/' to exe_cmd
                strcat(exe_cmd, cmd[i]); // attach the command
                //strcat(exe_cmd, "\0");
                //printf("exe_cmd is %s\n", exe_cmd);
                // check if the /PATH/command is accessable
                if (access(exe_cmd, X_OK)==0){
                    //printf("Have access\n");
                    avaliable=1; // mark user has access to the /PATH/command
                    break; // then stop the loop
                }
                PATH_INDEX++; // otherwise, move to the next PATH
            }
            // if user has access to the /PATH/command
            if (avaliable){
                redirect=0; // initialize the 'redirect' variable, which will be used to mark if redirect will be the situation
                memset(fileName,0,strlen(fileName)); //clean the fileName string
                int exe_idx=1; // initialize the index used in executable
                //printf("Step 2.1\n");
                executable[0] = malloc(sizeof(char) * 32); // allocate memory to the first index of executable, which is used to store command
                strcpy(executable[0], exe_cmd); // save the accessable command /PATH/command to executable[0]
                //printf("executable[0] is %s\n", executable[0]);
                // Then save the parameters into the following positions of executable
                while (strcmp(cmd_para[i][exe_idx-1], "\0") != 0){
                    //printf("Step 2.%d\n", exe_idx+1);
                    
                    // Check if the parameter is '>', if yes, set redirect to 1
                    if (strcmp(cmd_para[i][exe_idx-1], ">") == 0){
                        redirect=1; // mark the redirect is true
                        exe_idx++; // move the index to the next
                        continue; // then escape the rest of this loop, and redo the loop with the next index
                    }
                    // save the normal parameter to the executable
                    if (redirect == 0){
                        executable[exe_idx] = malloc(sizeof(char) * 32); // allocate the memory to the current executable
                        strcpy(executable[exe_idx], cmd_para[i][exe_idx-1]); // Then copy the parameter for command into executable
                        //printf("executable[%d] is %s\n", exe_idx, executable[exe_idx]);
                    }
                    // otherwise, save the command parameter to fileName.
                    else{
                        strcpy(fileName, cmd_para[i][exe_idx-1]);
                    }
                    // increase the index
                    exe_idx++;
                }
                //printf("final Step\n");
                // if redirect is true, then move the index 2 position back, because it is the place to pull NULL in executable
                if (redirect==1){
                    exe_idx -=2;
                }
                // allocate memory to the last position of executable
                executable[exe_idx] = malloc(sizeof(char) * 32);
                // assign NULL to the last position of executable
                executable[exe_idx]=str;
                //printf("The final executable is: %s, %s, %s, %s\n", executable[0], executable[1],executable[2],executable[3]);
                
                /*
                 Fork processes
                 execute command in the child process
                 */
                int child_process=fork();
                if (child_process<0){ // FORK FAILLED
                    printf("Fork FAILED!!!!\n");
                }
                else if (child_process==0){ // Child Process
                    //printf("CHILD PROCESS HERE!\n");
                    //printf("Command to be execute is: %s\n", executable[0]);
                    // if redirect is true, then execute the write to file first
                    if (redirect == 1){
                        redirectOut(fileName);
                    }
                    // Then execute the command /PATH/command with parametes in child process
                    execv(executable[0], executable);
                    //printf("CHILD PROCESS FINISHED!\n");
                }
                // Save the Parent Process for further use
                else{
                    printf("\n");
                }
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


// The read command line function
char *read_line(void)
{
    char *buffer;
    size_t bufsize = CMD_BUFSIZE;
    buffer = malloc(sizeof(char) * bufsize);
    if( buffer == NULL){
        perror("Unable to allocate buffer");
        exit(1);
    }
    // read the whole line. The maximam length is 1024 characters
    getline(&buffer, &bufsize,stdin);
    return buffer;
}


/*
 The Command Line tool. This is the central of control.
 Keep looping, until the status is 0
 */
void dash_cmdLine(void)
{
    char *line;
    int status=1;
    //int pid;
    //char *args[]={"/bin/ls", "-a", "-l", NULL};
    
    do{
        printf("dash> ");
        // read line from user input
        line = read_line();
        // execute the line read into the system
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
        //status = update_status(line);
        //printf("Status is : %d\n", status);
    } while (status);
}


// THe main program. Start of the dash
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
                // whenever there is a ':', save the previous path and start reading the new onw
                memcpy(ENVPATH[curP], &PATH[base], i-base);
                base = i+1;
                curP++;
            }
            i++;
        }
        // save the last path into environment path variable
        memcpy(ENVPATH[curP], &PATH[base], i-base);
        
        //start the command line function. Keep reading the command line from user input
        dash_cmdLine();
    
        return EXIT_SUCCESS;
    }
    else{
        printf("This is the section for readin file.\n");
    }
}

