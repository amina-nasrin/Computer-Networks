#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

#define LISTEN_BACKLOG 5
#define station_process 10

static int sockfd = 0;

struct client{
    char client_name[10];
    char dest_name[10];
    int dest_fd;
    int file_des;
    int port;
    char ip[INET_ADDRSTRLEN];
};

struct server_data{
    int total_client;
    struct client client_list[station_process];
};

struct server_data server;

void server_delete_client(int socket_fd_del);
void cleanup(void);

int serachClient(int socket){
    int index = 0;
    for(int i = 0; i<server.total_client; i++) {
           if(server.client_list[i].file_des == socket){
               index =i;
           }
    }
    return index;
}

int serachClientByName(char*name) {
    int index = 0;
    for(int i = 0; i<server.total_client; i++) {
           if(strcmp(server.client_list[i].client_name,name) == 0) {
               index =i;
           }
    }
    return index;
}
int dataToCli(int client_socket, char *send_buffer){
    int n_write = 0;
    if((n_write = write(client_socket, send_buffer, strlen(send_buffer)))>0){
        printf("\nClient%d sent %s\n",client_socket, send_buffer);
    }

    return n_write;
}

int ProcessData(int socket,char*buffer){
    char userip[256];
    char buffer_send[256] = {0};
    int index_sender = 0;
    int index_receiver = 0;
    index_sender = serachClient(socket);

    if((strncmp(buffer, "LIST",4)==0) || (strncmp(buffer, "List",4)==0) || (strncmp(buffer, "list",4)==0)){
         memset(buffer,0,sizeof(buffer));
         for(int i=0;i<server.total_client;i++) {
             strcat(buffer,server.client_list[i].client_name);
             strcat(buffer,"; ");
         }
        dataToCli(socket, buffer);
        goto out;
    }
    if((strncmp(buffer, "Req", 3) == 0) || (strncmp(buffer, "REQ", 3) == 0) || (strncmp(buffer, "Request", 7) == 0)){      
        sscanf(buffer,"%*[^:]: %s", userip);
        strcpy(server.client_list[index_sender].dest_name, userip);
       
        index_receiver = serachClientByName(server.client_list[index_sender].dest_name);
        server.client_list[index_sender].dest_fd = server.client_list[index_receiver].file_des;
        dataToCli(server.client_list[index_sender].file_des, "");
        goto out;
    }

    if(strlen(server.client_list[index_sender].dest_name) != 0){
        snprintf(buffer_send,sizeof(buffer_send),"[%s] : %s",server.client_list[index_sender].client_name,buffer);
        dataToCli(server.client_list[index_sender].dest_fd,buffer_send);
    }

out:
    return 0;
}

int dataFromCli(int client_socket, char *cli_buffer){
    int n_read = 0;
    int q_client[5], top = 0;
    bzero(cli_buffer, strlen(cli_buffer));
    for(int j=0;j<5;j++)
        q_client[j]=0;
    
    if((n_read = read(client_socket, cli_buffer, 256))>0){
        if(buffer[strlen(buffer)] != 0){
                printf("Reject Reply: Try Again!");
                write(client_socket, "Reject Reply: Try Again!", 24);
                q_client[top] = client_socket;
                top++;
                return 0;
            }
            if( ((top>0) || (top ==0)) && (q_client[top-1] != 0)){
                client_socket = q_client[top];
                top--;
            }
            ProcessData(client_socket, cli_buffer);
    } else if(n_read == 0) {
           printf("Client %d Disconnected\n", client_socket);
           server_delete_client(client_socket);  
    } else printf("Read Failed\n");
    
    return 0;
}

int main(int argc, char *argv[]){
    int sockfd = 0, maxfd= 0, portno, sockfd_fd = 0, activity, port;
    char in_buffer[256], out_buffer[256], ip[INET_ADDRSTRLEN] = {0}, buffer[256] = {0};
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    portno = atoi(argv[1]);

    memset(&server, 0, sizeof(struct server_data));
    
    struct sockaddr_in server_addr, client_addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket Failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno); 
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if( 0!=bind(sockfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))){
         perror("Bind failed");
         return -1;
    }

    if(0!=listen(sockfd, LISTEN_BACKLOG)) {
         perror("Listen failed");
         return -1;
    }
    printf("Server is running...");

    maxfd = sockfd;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO,&readfds);
        fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);
        for(int i=0; i<server.total_client; i++) {
           FD_SET(server.client_list[i].file_des, &readfds);
            maxfd++;
        }

        bzero(in_buffer, 256);
        bzero(out_buffer, 256);

        activity = select(maxfd+1, &readfds, &writefds, NULL, NULL);
        if(activity == -1 || activity == 0){
            perror("ERROR: select");
            exit(0);
        }

        if(FD_ISSET(sockfd, &readfds)){
            int len ;
            len = sizeof(struct sockaddr);
            bzero(&client_addr,sizeof(client_addr));
            if((sockfd_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len))<0) {
                perror("Accept Failed");
                return -1;
            }
            printf("New socket created = %d\n", sockfd_fd);
            dataFromCli(sockfd_fd, buffer);
    
            port = ntohs(client_addr.sin_port);
            inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
            printf("New Client: port = %d ip = %s\n", port, ip);

            strncpy(server.client_list[server.total_client].client_name, buffer, strlen(buffer));
            server.client_list[server.total_client].port = port;
            strcpy(server.client_list[server.total_client].ip, ip);
            server.client_list[server.total_client].file_des = sockfd_fd;
    
            server.total_client++;
        }

        if(FD_ISSET(STDIN_FILENO, &readfds)){
            if(read(0,out_buffer,sizeof(out_buffer))>0){
                for(int i = 0;i<server.total_client;i++)
                    dataToCli(server.client_list[i].file_des, out_buffer);
                }             
        }
     
        for(int i = 0; i<server.total_client; i++){    
            if(FD_ISSET(server.client_list[i].file_des, &readfds)) {
               dataFromCli(server.client_list[i].file_des, in_buffer); 
            } 
        }
    }

    cleanup();
    printf("Bye From server\n");
    return 0;
}

void server_delete_client(int socket_fd_del){
    int i = 0;
    int index = 0;

    for(i=0;i<station_process;i++) {
        if(server.client_list[i].file_des == socket_fd_del){
            for(index = i; index<station_process;index++ ){
                server.client_list[index] = server.client_list[index+1];
            }
        }
    }

    server.total_client--;
    printf("Deleted Socket [%d]\n",socket_fd_del);
    close(socket_fd_del);
}

void cleanup(){
    close(sockfd);
    for(int i = 0; i<server.total_client; i++){
           close(server.client_list[i].file_des);
    }
}
