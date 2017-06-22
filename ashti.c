#include <stdio.h>


int main(int argc, char *argv[])
{

    // User must pass a directory as a command line argument
    if(argc != 2)
    {
        fprintf(stderr, "%s < directory >\n", argv[0]);
        return 0;
    }
    
    //strip out the directory
    char * directory = argv[1];
 
    //TEST
    printf("directory entered %s\n", directory);


    return 0;

}
