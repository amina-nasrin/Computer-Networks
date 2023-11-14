#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
# include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct hostent *he;

    char buffer[256];
    if(argc<3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        error("Error opening Socket.");
    }
    if ((he=gethostbyname(argv[1])) == NULL) {
      perror("gethostbyname");
      return 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
        error("Connection Failed");

    while(1){
        bzero(buffer, 256);
        printf("Client: ");
        fgets(buffer, 256, stdin);
        n = write(sockfd, buffer, strlen(buffer));

        if(n<0)
            error("Error on writing");
        bzero(buffer, 256);
        n=read(sockfd, buffer, 256);
        if(n<0)
            error("Error on reading.");
        printf("Server: %s", buffer);

        int i = strcmp("Bye", buffer);
        if(i == 0)
            break;
    }

    close(sockfd);
    return 0;
}