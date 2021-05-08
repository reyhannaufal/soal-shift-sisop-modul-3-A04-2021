#include <libgen.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8080
  

int input_mode = 0;

pthread_t input;
pthread_t output;



/*void *routine_output(void *arg){
	int sock = *(int*)arg;
	char buffer[1024] = {0};
	while(1){
		if(recv(sock,buffer,1024,0)>1){
		char buffer2[1024];
		strcpy(buffer2,buffer);
		printf("%s",buffer2);
		}
	}
}*/

void *routine_input(void *arg){
	int sock = *(int*)arg;
	char input[1024];
	char buffer[1024];
	while(1){
		if(recv(sock,buffer,1024,0)>1){
		char buffer2[1024];
		strcpy(buffer2,buffer);
		printf("%s",buffer2);
		}
		
		memset(input,0,1024);
		fgets(input,1024,stdin);
		send(sock,input,1024,0);
	}
}


int main(int argc, char const *argv[]) {


    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
 
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
	
	pthread_create(&input, NULL,&routine_input, (void *)&sock);
//	pthread_create(&output, NULL,&routine_output, (void *)&sock);
	
	pthread_join(input,NULL);
//	pthread_join(output,NULL);
    return 0;
}
