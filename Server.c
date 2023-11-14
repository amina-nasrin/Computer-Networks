#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<ctype.h>
#include<string.h>

//#include<netinet.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc<2){
        fprintf(stderr, "Port No not provided. Program terminated\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    char buffer[256], chkstr[256], buffernotint[] = "You didn't send me an integer value!\n", buffereven[] = "You send me an even integer\n", bufferodd[] = "You send me an odd integer\n";

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        error("Error opening Socket.");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
        error("Binding Failed.");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd<0)
        error("Error on Accept");

    while(1){
        bzero(buffer,256);
        n = read(newsockfd,buffer,256);//256?
        if (n < 0) 
            error("ERROR reading from socket");

        printf("Client: %s\n",buffer);
        
        bzero(chkstr, 256);
        strcpy(chkstr, buffer);
        
        int val = atoi(chkstr);
        if(val ==0)
            n = write(newsockfd, buffernotint, strlen(buffernotint));
        else {
            if(val%2 == 1)
                n = write(newsockfd, bufferodd, strlen(buffernotint));
            else n = write(newsockfd, buffereven, strlen(buffernotint));
        }
        bzero(buffer, 256);
        /*if(n<0)
            error("ERROR writing to socket");
        int i = strcmp("Bye", buffer);
        if(i == 0)
            break;*/
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}