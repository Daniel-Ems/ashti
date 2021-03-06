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

//struct to handle the request information
typedef struct parsed_request{
    char *file;
    char *HTTP;
    int flag;  
    char *mess;
}request;

//parse requests
struct parsed_request *parse_func(char *);

//handle signal
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
    snprintf(port, sizeof(port), "%hu", (getuid()));

    //Create structs and populate the informaton for addrinfo
    struct addrinfo *results;
    

    //Null out the struct so you can assign certain fields
    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;

    //get the information necessary to populate results
    int err = getaddrinfo(ip, port, &hints, &results);
    if(err != 0)
    {
        fprintf(stderr, "An error occurred\n");
        return 2;
    }
   
    //Create a socket   
    sd = socket(results->ai_family, results->ai_socktype,0);
    if(sd < 0)
    {
        perror("Could not create socket");
        freeaddrinfo(results);
        return 3;
    }

    int set = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
    
    //bind socket to address and port
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
    
    // handle control c
    struct sigaction ignorer = {0};
    ignorer.sa_handler = sig_handler;
    sigaction(SIGINT, &ignorer, NULL);
   //END OF COPIED CODE
     
   //ORIGINAL CODE
    printf("MEMO: Change port number before you submit: %s\n", port );
    
    //strip out the directory
    char *directory = NULL;

    directory = argv[1];
    err = access(directory, F_OK);
    if(err == -1)
    {
        printf("Not a valid directory\n");
        exit(0);
    }

    //Create room for the path
    path = calloc(PATH_MAX, sizeof(char) );
    if(path == NULL)
    {
        perror("Out of memory \n");
        exit(0);
    }
   
    //Add the directory to the path
    strncpy(path, directory, strlen(directory));

   
    char *good = "HTTP/1.1 200 OK\n";
    char *content = "Content-type: text/html\n\n";
    int nmemb;
    FILE *html;
    char *finish = "\n\n";
    char read[256] = {0};
    int strchk = '\0';
    char *fnf = "HTTP/1.1 404 Not Found";
    //Partially Copied Code
    //Author: Liam Echlin
    //Source: networking/day03/tcp_server.c
    //Descrption: the for loop and child fork was taken from the tcp_server.c and modified 
    //to fit this particular project. Much was added, and some remains.
    for (;;) 
    {
        request *response;
        char buf[256] = {0};
        struct sockaddr_storage client; 
        socklen_t client_sz = sizeof(client);
        
        //accpet block to accpet connnecitons 
        remote = accept(sd, (struct sockaddr *)&client, &client_sz);
        if(remote < 0)
        {
            perror("could not accept connection");
            continue;
        }
        //fork on new connections 
        pid_t child = fork();
        if(child == 0)
        {
            close(sd);
            //receive the information and then parse it.
            //This should be in a while loop and read until /n/n is read, however, this was
            //not apparent until too late in the project and as a result it was not done. 
            ssize_t received = recv(remote, buf, sizeof(buf)-1, 0);
             if (received < 0)
            {
                perror("Problem receiving");
            }
            buf[received] = '\0';        
            
            //parse out the received data
            response = parse_func(buf);
            
            //print server error code. doesn't work with browser, dont know why
            if(response->flag < 0 )
            {
                
                send(remote, response->mess, strlen(response->mess), 0);
                send(remote, content, strlen(content), 0);
                exit(0);
            }
            //its in the cgi bin -> does work with browser.. dont know why
            else if(response->flag > 0)
            {
                html = popen(response->file, "r");                    
            }
            //if its a html -> doens't work with browser, don't know why.
            else
            {
                html = fopen(response->file, "r");
            }
            //if the file isnt found
            if(!html)
            {
                send(remote, fnf, strlen(fnf), 0);
                send(remote, content, strlen(content), 0);
                free(response);
                close(remote);
                exit(0);
            }
            //send the header. 
            send(remote, good, strlen(good), 0);
            send(remote, content, strlen(content), 0);

            //read from the file stream
            while((nmemb = fread(read, sizeof(char), 254, html))>0) 
            {
                read[strlen(read)] = '\0';
                send(remote, read, nmemb, 0);
            }
            //try and send the two /n/n the browser looks for... doesn't work
            send(remote, finish, strlen(finish), 0); 
            fclose(html); 
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

}

//close as best as possible
void sig_handler(int signum)
{   
    free(path);
    close(sd);
    close(remote);
    exit(0);
}

//The function is supposed to take the request, parse it and then return it for sending
struct parsed_request *parse_func(char *buf)
{
    char *ise = "HTTP/1.1 500 Internal Server Error\n";
    char read[256] = {0};
    char *www = "/www";
    char *cgi = "/cgi-bin/"; 
    char *GET = "GET";
    char *token;
    int strchk = '\0';
    
    request *response = malloc(sizeof(*response));        

    response->flag = 0;

    token = strtok(buf, " ");
    strchk = strncmp(token, GET, strlen(GET));
    if(strchk != 0)
    {
        response->flag = -1;
        response->mess = ise;
        return(response);
    }

    token = strtok(NULL, " ");
    strchk = strncmp(token, cgi, strlen(cgi));
    if(strchk == 0)
    {
        response->flag = 1;
        response->file =  strncat(path, token, strlen(token));
    }
    else
    {
        response->file = strncat(path, www, strlen(www));
        response->file = strncat(path, token, strlen(token));
    }    
    printf("response->file %s\n", response->file);
    response->file[strlen(response->file)] = '\0';
    return (response);
}
