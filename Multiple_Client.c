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

void reqSend(int sockfd){
    int n;
    char req[] = "REQC";
    printf("\nSending Data Req to CSP...\n");
    n = write(sockfd, req, strlen(req));
    if(n<0)
        error("Error on writing"); 
}

void readFromSocket(){

}

void dataSend(int sockfd){
    int n;
    char data[16];

    printf("CSP sent OK.\nInput Data to Send...\n");
    printf("User: %s", data);
    fgets(data, 16, stdin);
    fgets(data, 16, stdin);
    n = write(sockfd, data, 16);                                          //3. Sending Data
    if(n<0)
        error("Error on writing");
    else printf("Data Frame Sent to CSP.\n");
}

int main(int argc, char *argv[]){
    
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct hostent *he;

    char buffer[256], req[] = "REQC", data[16], data1[16], d[]=" Frame 1, to SP6", y, w;
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
    else printf("Connection Established.");
    
    while(buffer){
        printf("\nPress y to send Req to CSP. Press w to sit idle.\n");//sit idle means wait for data
        scanf("%c", &y);

        if(y == 'y'){
            reqSend(sockfd);  
            printf("\nWaiting for reply from CSP...");
            bzero(buffer, 256);
            n=read(sockfd, buffer, 256);                                              //2. Reading Reply from server. OK--> Send data. NOK--> Resend Req. 1-->Received
            if(strcmp(buffer, "OK") == 0){
                dataSend(sockfd);
                                    printf("Listening...");
                    bzero(buffer, 256);
                printf("Buffer %s", buffer);
                                    n=read(sockfd, buffer, 256);
                printf("Buffer %s", buffer);

                //printf("Press y to Send Data again. Press w to Enter Idle mode.");
                //scanf("%c", &w);
                //scanf("%c", &w);
//                printf("Press y to Send Data again. Press w to Enter Idle mode.");
                //printf("Press y to Send Data again. Press w to Enter Idle mode.");
                if(y == 'w'){
                    printf("Listening...");
                    bzero(buffer, 256);
                    n=read(sockfd, buffer, 256);
                    if(strcmp(buffer, "1") == 0){
                        printf("Received");                                                                 ///////////start working from client. Client does not work send and receive 2 at once.                                                                                ///////////read the 4 Simulation Result first. Then start working.
                    //return 0;
                    }
                }
                
            } else if(strcmp(buffer, "NOK") == 0){
                //reqSend(sockfd);
                printf("\nSending Data Req to CSP AGAIN...\n");
                n = write(sockfd, req, strlen(req));                                      //1. Req frame sent 
                bzero(buffer, 256);
                n=read(sockfd, buffer, 256);                
            }
        }
    }
    close(sockfd);
    return 0;
}
