#define _GNU_SOURCE
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
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/wait.h>

typedef struct parsed_request{
    char *file;
    char *HTTP;
    int flag;  
    char *mess;
}request;

struct parsed_request *parse_func(char *);
void sig_handler(int signum);
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

int sd;
int remote;  
char *path;

int main(int argc, char *argv[])
{

    char *ip = "127.0.0.1";
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

    //get the information necessary to populate results
    int err = getaddrinfo(ip, port, &hints, &results);
    if(err != 0)
    {
        fprintf(stderr, "An error occurred\n");
        return 2;
    }
   
    sd = socket(results->ai_family, results->ai_socktype,0);
    if(sd < 0)
    {
        perror("Could not create socket");
        freeaddrinfo(results);
        return 3;
    }

    int set = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
    
    err = bind(sd, results->ai_addr, results->ai_addrlen);
    if(err < 0)
    {
        perror("Could not bind");
        close(sd);
        freeaddrinfo(results);
        return 4;
    }
    
    freeaddrinfo(results);

    err = listen(sd, 5);
    if(err < 0)
    {
        perror("Could not listen");
        freeaddrinfo(results);
        return 4;
    }
    
    struct sigaction ignorer = {0};
    ignorer.sa_handler = sig_handler;
    sigaction(SIGINT, &ignorer, NULL);
     
    
    printf("MEMO: Change port number before you submit: %s\n", port );
    
    //strip out the directory
    char *directory = NULL;
    int error = -1;
    directory = argv[1];
    printf("%s\n", directory);
    err = access(directory, F_OK);
    if(err == -1)
    {
        printf("Something bad happened\n");
    }

    //TEST
    printf("directory entered %s\n", directory);

   
    path = calloc(PATH_MAX, sizeof(char) );
    if(path == NULL)
    {
        perror("Out of memory \n");
        exit(0);
    }
   
    strncpy(path, directory, strlen(directory));

    printf("dir : %s\n", path);
   
    FILE *html;
    char *fnf = "404: file not found";
    char read[256] = {0};
    char *www = "/www";
    char *cgi = "/cgi-bin/"; 
    char *GET = "GET";
    char *token;
    int strchk = '\0';
    for (;;) 
    {
        request *response;
        char buf[256] = {0};
        struct sockaddr_storage client; 
        socklen_t client_sz = sizeof(client);
         
        remote = accept(sd, (struct sockaddr *)&client, &client_sz);
        if(remote < 0)
        {
            perror("could not accept connection");
            continue;
        }

        pid_t child = fork();
        if(child == 0)
        {
            close(sd);
            
            ssize_t received = recv(remote, buf, sizeof(buf)-1, 0);
            buf[received] = '\0';        
            
            while(received > 0)
            {
                          
                response = parse_func(buf);
                if(response->flag == 1)
                {
                    send(remote, response->mess, strlen(response->mess), 0);
                    exit(0);
                }
             
                received = recv(remote, buf, sizeof(buf)-1, 0);
            }
            if (received < 0)
            {
                perror("Problem receiving");
            }

            free(response);
            close(remote);
            return 0;
        }
        else if(child < 0)
        {
            perror("Could not spwan child");
            continue;
        }
        close(remote);
    }
    //END OF COPIED CODE
}

void sig_handler(int signum)
{
        
    free(path);
    close(sd);
    close(remote);
    exit(0);
}

struct parsed_request *parse_func(char *buf)
{
    char *fnf = "404: file not found";
    char *ise = "500: Internal Server Error\n";
    char *OK = "200 OK";
    char read[256] = {0};
    char *www = "/www";
    char *cgi = "/cgi-bin/"; 
    char *GET = "GET";
    char *token;
    int strchk = '\0';
    
    request *response = malloc(sizeof(*response));        

    token = strtok(buf, " ");
    strchk = strncmp(token, GET, strlen(GET));
    if(strchk != 0)
    {
        response->flag = 1;
        response->mess = ise;
        return(response);
    }

    token = strtok(NULL, " ");
    strchk = strncmp(token, cgi, strlen(cgi));
    if(strchk == 0)
    {
        response->file = token;
    }
    else
    {
        response->file = strncpy(path, www, strlen(www));
        response->file =  strncat(path, token, strlen(token));
    }
    
    response->HTTP = strtok(NULL, "\n");
              
    response->file[strlen(response->file)] = '\0';
    return (response);
}
