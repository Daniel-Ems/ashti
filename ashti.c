#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{

    // User must pass a directory as a command line argument
    if(argc != 2)
    {
        fprintf(stderr, "%s < directory >\n", argv[0]);
        return 0;
    }
    
    //strip out the directory
    char *directory = "\0";
    directory = argv[1];
 
    //TEST
    printf("directory entered %s\n", directory);

   
    char *path = calloc(PATH_MAX, sizeof(char) );
    strncat(path, "~/", 3);
    strncat(path, directory, strlen(directory));

    printf("dir : %s\n", path);
    free(path);
    return 0;

}
