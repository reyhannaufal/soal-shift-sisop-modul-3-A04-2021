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
int sock;

int printing;


void download(char nama_file[256]){
	FILE *fp = fopen(nama_file,"w");
	fclose(fp);
	fp = fopen(nama_file,"a");
	char buffer[256];
	
	while(1){
		memset(buffer,0,256);
		recv(sock,buffer,256,0);
		if(strcmp(buffer,"_download_done_")==0){
			break;
		}
		fprintf(fp,"%s",buffer);
		
	}
	fclose(fp);
}

void *routine_output(void *arg){
	int sock = *(int*)arg;
	char buffer[256] = {0};
	while(1){
		memset(buffer,0,256);
		if(recv(sock,buffer,256,0)>1){
		printing = 1;
		char buffer2[256];
		strcpy(buffer2,buffer);
		if(strcmp(buffer2,"_download_start_")==0){
			recv(sock,buffer,256,0);
			strcpy(buffer2,buffer);
			download(buffer2);
			pthread_cancel(input);
			printing = 0;
			continue;
		}
		pthread_cancel(input);
		printf("%s",buffer2);
		printing = 0;
		}
	}
}

void *routine_input(void *arg){
	int sock = *(int*)arg;
	char input[256];
	char buffer[256];
	while(1){
		if(printing == 0){
			memset(input,0,256);
			fgets(input,256,stdin);
			if(input[strlen(input)-1] == '\n'){
				int len = strlen(input);
		
				input[--len] = '\0';
			}
			send(sock,input,256,0);
		}
	}
}


int main(int argc, char const *argv[]) {


    struct sockaddr_in address;
    int valread;
    sock = 0;
    struct sockaddr_in serv_addr;
 
    char buffer[256] = {0};
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
	pthread_create(&output, NULL,&routine_output, (void *)&sock);
	while(1){
		if(pthread_join(input,NULL)==0){
			pthread_create(&input,NULL,&routine_input,(void *)&sock);
		}
	}
	
	pthread_join(output,NULL);
    return 0;
}
