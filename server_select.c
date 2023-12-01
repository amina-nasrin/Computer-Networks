#include <stdio.h>
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
	
#define TRUE 1 
#define FALSE 0 
#define PORT 8884

#define station_process 10
int checkForPositionIndf_q(char df_q[]){
	int i;
	for(i=0;i<1;i++)
		if(df_q[i] == 0)
			return i;
		else return -1;

}

void sendMsg(char addr1, char msg[]){
	int i;
	addr1 = addr1 - '0';
	for(i=0;i<station_process; i++)
		if(i == addr1){
			write(addr1, msg, 1);
			printf("Found again\n\n");
			//close(addr1);
		}
}

int main(int argc , char *argv[]){
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30], activity, i, n, next=0, valread , sd, position=-1, buffre_len, x; 
	int max_sd; 
	struct sockaddr_in address; 
		
	char buffer[1025];
    bzero(buffer, 1025);
	
	int req_queue[10];
	x=16;
	char df_queue[n], df_q[1]; //256
	bzero(df_q, 1); //256
	for(i=0;i<x;i++)
		df_queue[i]=0;
	fd_set readfds, writefds;
		
	char *message = "Welcome!!! Wait\r\n";
    char OK[] = "SEND__\n";

	for (i = 0;i<station_process;i++)
		client_socket[i] = 0; 
	
	if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	}
	
	if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}
	//printf("Listener on port %d \n", PORT); 
	if (listen(master_socket, 3) < 0){ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	addrlen = sizeof(address); 
	//puts("Waiting for connections ..."); 
		
	while(TRUE){
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
	
    	FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		for (i=0;i<station_process;i++){ 
			sd = client_socket[i]; 
			if(sd>0)
				FD_SET(sd, &readfds); 
				 
			if(sd>max_sd)
				max_sd = sd; 
		}
	
		activity = select(max_sd+1, &readfds, &writefds, NULL, NULL); 
	
		if((activity<0) && (errno!=EINTR))
			printf("select error");

		//////////////////////////////////////////////////////////////
		if(FD_ISSET(master_socket, &readfds)){
			if((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ 
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			printf("\n\nNew connection is established. socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));				
			
			bzero(buffer,256);
			read(new_socket,buffer,256);//receiving fropm SP					2.
			if((strcmp(buffer, "REQC"))==0){//received a REQ frame				3.a
				printf("%s Received from SP %d.\n", buffer, new_socket);
				
				if(df_queue[x-1] != 0){//checks if df_q is full --> yes	
					printf("GO TO THE OTHER Q\n");
					write(new_socket, "NOK", strlen("NOK"));//negative reply Wait4.b		//this line is not necessary
					req_queue[next] = new_socket;//added req to req_q
					printf("latest sock %d Queue end %d\n", req_queue[next], next);
					next++;
					position = checkForPositionIndf_q(df_q);
					if(position >=0)
						printf("Move data to df_q\n");

				} else{//df_q is not full				Send Data				4.a
					printf("\t\tSPACE in Q %s %d \n",df_queue, df_queue[x-1]);
					write(new_socket, "OK", strlen("OK"));//positive reply
					bzero(buffer,256);
					}
			} else{//Received Data Frame										3.b
				bzero(buffer,256);
				read(new_socket,buffer,256);//receiving data from SP
				printf("BUF F CLI::: %s", buffer);
		        strcpy(df_q, buffer);
				printf("\n%d", df_q[strlen(df_q)]);
				}

			for (i=0; i<station_process; i++){ 
				if( client_socket[i] == 0){
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n", i);
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket 
		for (i = 0; i<station_process; i++){
			sd = client_socket[i];

			printf("\n");
			if(FD_ISSET(sd, &readfds)){
				valread = read(sd, buffer, 1024);
				if (valread == 0){
					getpeername(sd, (struct sockaddr*)&address,(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
						
					close(sd);
					client_socket[i] = 0; 
				} else {
					printf("BUF F CLI:: %s", buffer);
					strcpy(df_queue, buffer);
					char addr1, msg[1];
					for(int j=0;j<20; j++)
						if(df_queue[j] == 't' || df_queue[j] == 'T'){
							addr1 = df_queue[j+6];
							//printf("\nAddress %c", addr1);
						}
					for(int j=0;j<20; j++)
						if(df_queue[j] == 'f' || df_queue[j] == 'F'){
							msg[0] = df_queue[j+6];
							//printf("\nMSG %c", msg[0]);
						}
					sendMsg(addr1, msg);
				}
			}
		}
	}
	return 0; 
}
