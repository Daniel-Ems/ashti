#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

///COPIED CODE///
//SOURCE: network/day03/tcp_server.c
//AUTHOR: Liam Echlin
/*DESCRIPTION: 
    The code taken is solely for the TCP server portion.
    There are a limited number of ways to write a tcp
    server, and the code taken reflects the way we were 
    taught. To try and recreate a tcp server for the sake 
    of not copying code creates the risk of being insufficient 
    in meeting the standard upon which we are expected to abide.
    changes to the code will be made an anotated appropriately. 
    furthermore, any original code that is added to the sourced
    items will be annotated appropriately. 
*/

int main(int argc, char *argv[])
{

    // User must pass a directory as a command line argument
    if(argc != 2)
    {
        fprintf(stderr, "%s < directory >\n", argv[0]);
        return 0;
    }
    
    //create an array to hold the port number, using the uid as the port. 
    char port[8];
    snprintf(port, sizeof(port), "%hu", (getuid()+1000));

    //Create structs and populate the informaton for addrinfo
    struct addrinfo *results;

    //Null out the struct so you can assign certain fields
    struct addrinfo  hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo("127.0.0.1", port, &hints, &results);
    
    printf("MEMO: Change port number before you submit: %s\n", port );
    
    //strip out the directory
    char *directory = "\0";
    directory = argv[1];
 
    //TEST
    printf("directory entered %s\n", directory);

   
    char *path = calloc(PATH_MAX, sizeof(char) );
    if(path == NULL)
    {
        perror("Out of memory \n");
        exit(0);
    }
    
    strncat(path, "~/", 3); 
    strncat(path, directory, strlen(directory));
    strncat(path, "/", sizeof(char));

    char * file = "\0";
    printf("dir : %s\n", path);
    free(path);
    return 0;

}
