#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    
    signal(SIGCHLD, SIG_IGN); 

    printf("Concurrent Server started on port %d...\n", portno);

    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");

        pid = fork();
        if (pid < 0) error("ERROR on fork");

        if (pid == 0) {  
            close(sockfd); 
            
            char buffer[256];
            bzero(buffer, 256);
            int n = read(newsockfd, buffer, 255);
            if (n < 0) error("ERROR reading from socket");
            
            printf("Received message: %s\n", buffer);
            
            n = write(newsockfd, "I got your message", 18);
            if (n < 0) error("ERROR writing to socket");
            
            close(newsockfd);
            exit(0); 
        } 
        else { 
            close(newsockfd); 
        }
    }
    return 0; 
}