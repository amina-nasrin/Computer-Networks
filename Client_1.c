#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

char client_name[10]={0};

int main(int argc, char *argv[]){
    int sockfd = 0, maxfd = 0, portno, activity, n_read=0, n_client=0, n_write=0;
    char in_buffer[256], out_buffer[256];
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    struct sockaddr_in server_addr;

    if(argc>3) {
        perror("ERROR : Parameters error");
        exit(0);
    }
    
    strcpy(client_name, argv[1]);
    portno = atoi(argv[2]);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket Failed");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno); 
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))!=0){
        perror("Connect Failed");
        return -1;
    }

    if((n_client = write(sockfd, client_name, strlen(client_name)))<=0) {
        perror("Send Failed");
        return -1;
    }
    maxfd = sockfd;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(sockfd, &writefds);
        FD_SET(STDIN_FILENO, &readfds);
        fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);
        
        bzero(in_buffer, 256);
        bzero(out_buffer, 256);
        activity = select(maxfd+1, &readfds, &writefds,NULL,NULL);

        if(activity == -1 || activity == 0){
            perror("ERROR: select");
            exit(0);
        }

        if(FD_ISSET(sockfd, &readfds)){
            if((n_read = read(sockfd, in_buffer, 256))>0){
                printf("%s\n",in_buffer);
                //if(strcmp((n_read, "Reject Reply... Try Again!") ==0 )
                  //  n_write = write(sockfd, out_buffer, strlen(out_buffer));
            } else if(n_read == 0){
                printf("Client Disconnected\n");
                close(sockfd);
            } else printf("ERROR: recv failed\n");
        }

        if(FD_ISSET(STDIN_FILENO, &readfds)){
            if(read(0, out_buffer, sizeof(out_buffer))>0){
                if((n_write = write(sockfd, out_buffer, strlen(out_buffer)))<=0) {
                    perror("Send Failed");
                    return -1;
                    }
            }                   
        }
        maxfd = sockfd;
    }
    close(sockfd);
    return 0;
}
