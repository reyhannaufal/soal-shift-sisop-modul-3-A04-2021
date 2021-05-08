#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <pthread.h>
#define PORT 8080

typedef struct user{
	char username[255], password[255];
}user;

//typedef struct

void regis(char username[], char password[]){
	FILE *fp = fopen("akun.txt","a");
	fprintf(fp,"%s.%s\n",username,password);
	fclose(fp);
}

void print_regis(){				//todo make send
	FILE *fp =fopen("akun.txt","r");
	sprintf("(uname).(password)");
	char buffer[1024];
	while(fgets(buffer,1024,fp) != NULL){
		printf("%s",buffer);
	}
	fclose(fp);
}

void writelog(char cmd[], char target[], user *player){
	FILE *fp = fopen("running.log","a");
	fprintf(fp,"%s : %s (%s:%s)",cmd,target,player->username,player->password);
	fclose(fp);
}

int state = 0; //0 not logged in //1 logged in
char input[1024]; //input from client
int new_socket;

void *server_main_routine(void *arg){

	char buffer[1024];
	while(1){
		if (state == 0){
			if(strcmp(input,"register")==0){
			//todo make register 
			}
			if(strcmp(input,"login")==0{
			//todo make input|if login state = 1
			}
		}
		if (state == 1){	
			if(strcmp(input,"add")==0){
			//todo make add
			}
			if(strcmp(input,"download")==0){
			//todo make download
			}
			if(strcmp(input,"delete")==0){
			//todo make delete
			}
			if(strcmp(input,"see")==0){
			//todo make see
			}
		}
	}
}

void *server_scan_routine(void *arg){

	char buffer[1024];
	while (1){
		if(recv(new_socket,buffer,1024,0)>0){
			strcpy(input, buffer);
		}
	}
}


int main(int argc, char const *argv[]) {
	FILE *fp = fopen("akun.txt","a");
	fclose(fp);
	
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

	
	//code below
	
	pthread_t socket_thread[2];	//0 for main, 1 for scan
	
	
	pthread_create(&socket_thread[0],NULL,&server_scan_routine,NULL);
	printf("foo\n");
	pthread_create(&socket_thread[1],NULL,&server_main_routine,NULL);
	printf("foo\n");
	
	pthread_join(socket_thread[0], NULL);
	pthread_join(socket_thread[1], NULL);
	printf("bar\n");
}
