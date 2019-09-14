#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
    int test[5]={1,2,3,4,5};
    printf("The size of array is %d\n", sizeof(test));
    
    int dou[3][6];
    printf("The size of the double array is %d\n", sizeof(dou));
    
    char *input="Hello World";
    printf("The size of the string is %d\n", sizeof(input));
    
    char input2[50];
    printf("The size of the string2 is %d\n", sizeof(input2));
    strcpy(input2, "Hello World");
    printf("The size of the string2 is %d\n", sizeof(input2));
    
    return EXIT_SUCCESS;
}
