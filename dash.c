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
char PATH[256];
char ENVPATH[16][32];


char *read_line(void)
{
    char *buffer;
    size_t bufsize = CMD_BUFSIZE;
    size_t characters;
    
    buffer = malloc(sizeof(char) * bufsize);
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    characters = getline(&buffer, &bufsize,stdin);
    printf("You typed: '%s'\n", buffer);
    return buffer;
    
}


int update_status(char *line)
{
    if (strcmp(line, "exit"))
    {
        return 1;
    }
    else
    {
        return 0;
    }
            
}

void dash_cmdLine(void)
{
    char *line;
    char **args;
    int status;
    
    do{
        printf("dash> ");
        line = read_line();
        status = update_status(line);
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
        fscanf(fp,"%s",PATH);
        int i=0;
        int curP=0;
        int base = 0;
        char temp[20];
        while (PATH[i]!='\0') {
            if (PATH[i]==':'){
                memcpy(ENVPATH[curP], &PATH[base], i-base);
                base = i+1;
                curP++;
            }
            i++;
        }
        memcpy(ENVPATH[curP], &PATH[base], i-base);
        printf("The Path is: %s\n", PATH);
        for (int j=0; j<=curP; j++){
            printf("The Env Path is: %s\n", ENVPATH[j]);
        }
    
        fclose(fp);
    
        dash_cmdLine();
    
        return EXIT_SUCCESS;
    }
    else{
        printf("This is the section for readin file.\n");
    }
}

