#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>


#define PORT 8080

typedef struct user{
	char username[256], password[256],input[256];
	int sock,logged;
	
}user;

/*
typedef struct buku{
	char nama[256],publisher[256],tahun[256],path[256], ext[256];
}buku;
*/


user *client[5];
int active_client= 0;
char path[256]; 



//buku *buffer_buku;[0]


void regis(user *client){
	FILE *fp = fopen("akun.txt","a");
	fprintf(fp,"%s.%s\n",client->username,client->password);
	fclose(fp);
}

void login(user *client){
	ssize_t read;
	FILE* fp = fopen("akun.txt","r");
	size_t len;
	char auth[256];
	sprintf(auth,"%s.%s\n",client->username,client->password);
	char *string;
	while((read = getline(&string,&len,fp))!= -1){
		if (strcmp(auth,string)==0){
			client->logged = 1;
			break;
		}
	}
	fclose(fp);
}

void tambah(char buff[]){
	FILE *fp = fopen("files.tsv","a");
	fprintf(fp,"%s",buff);
	fclose(fp);
}


void tambahF(char path[]){//makefile must make dir first
	FILE *fp = fopen(path,"a");
	fclose(fp);
}

void rek_mkdir(char path[]){
	char *tok = strrchr(path,'/');
	if(tok != NULL){
		*tok = 0;
		rek_mkdir(path);
		*tok = '/';
	}
	mkdir(path,0777);
}

void print_regis(){				
	FILE *fp =fopen("akun.txt","r");
	printf("(uname).(password)");
	char buffer[256];
	while(fgets(buffer,256,fp) != NULL){
		printf("%s",buffer);
	}
	fclose(fp);
}

void writelog(char cmd[], char nama_file[], user *client){
	FILE *fp = fopen("running.log","a");
	fprintf(fp,"%s : %s (%s:%s)\n",cmd,nama_file,client->username,client->password);
	fclose(fp);
}

bool file_exist(char nama[256]){
	FILE *fp = fopen("files.tsv","r");
	char buffer[256];
	char* token;
	while(fgets(buffer,256,fp)){
		token = strtok(buffer,"\t");
		if(strcmp(token,nama)==0){
			fclose(fp);
			return true;
		}
	}
	fclose(fp);
	return false;
}



void see(){	//change parameter to char nama_file[] for find
	int i = active_client;
	FILE *fp = fopen("files.tsv","r");
	char buffer[256];
	char line[256];
	char* token;
	char* ext;
	char* token_ext;
	while(fgets(line,256,fp)){	
		ext = strdup(line);
		token_ext=strtok(ext,".");
		token_ext=strtok(NULL,"\t");
		
		token = strtok(line,"\t");
		sprintf(buffer,"Nama : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Publisher : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Tahun Publishing : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		sprintf(buffer,"Ekstensi File : %s \n",token_ext);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Filepath : %s \n",token);
		send(client[i]->sock,buffer,256,0);

	}
	fclose(fp);
}

void download(char nama[256]){		//done
	int i = active_client;
	FILE *fp = fopen("files.tsv","r");
	char buffer[256], tokbuffer[256];
	const char* token;
	char* chr;
	
	int len;

	while(fgets(buffer,256,fp)){
		
		strcpy(tokbuffer,buffer);
		token = strtok(tokbuffer,"\t");
		if(strcmp(token,nama)==0){
		
			fclose(fp);
			token = strtok(NULL,"\t");
			token = strtok(NULL,"\t");
			token = strtok(NULL,"\t");

			asprintf(&chr,"%s",token);

			if(chr[strlen(chr)-1]=='\n'){
				len =strlen(chr);
				chr[--len] = '\0';
			}
			if(chr[strlen(chr)-1]=='/'){
				len =strlen(chr);
				chr[--len] = '\0';
			}
			sprintf(tokbuffer,"%s/%s",chr,nama);
			send(client[i]->sock,nama,256,0);
			fp = fopen(tokbuffer,"r");
			while (fgets(buffer,256,fp)){
				send(client[i]->sock,buffer,256,0);
			}
			
			break;
		}
	}
}

void delete(char nama[256]){
	FILE* fp_lama=fopen("files.tsv","r");
	FILE* fp_baru = fopen("baru.tsv","w");
	fclose(fp_baru);
	fp_baru = fopen("baru.tsv","a");
	char* token;
	char buffer[256],tokbuffer[256],chr[256],chr2[256];
	int len;
	while(fgets(buffer,256,fp_lama)){
		strcpy(tokbuffer,buffer);
		token = strtok(tokbuffer,"\t");
		if(strcmp(nama,token)==0){
			token = strtok(NULL,"\t");
			token = strtok(NULL,"\t");
			token = strtok(NULL,"\t");
			sprintf(chr,"%s",token);

			if(chr[strlen(chr)-1]=='\n'){
				len =strlen(chr);
				chr[--len] = '\0';
			}
			if(chr[strlen(chr)-1]=='/'){
				len =strlen(chr);
				chr[--len] = '\0';
			}
			sprintf(tokbuffer,"%s/%s",chr,nama);
			sprintf(chr2,"%s/old-%s",chr,nama);
			rename(tokbuffer,chr2);
			continue;
		}
		fprintf(fp_baru,"%s",buffer);
	}
	remove("files.tsv");
	rename("baru.tsv","files.tsv");
	fclose(fp_lama);
	fclose(fp_baru);
}


void find(char keyword[256]){
	int i = active_client;
	FILE *fp = fopen("files.tsv","r");
	char buffer[256];
	char line[256];
	char* token;
	char* ext;
	char* token_ext;
	int cursor = 0;
	regex_t regex;
	while(fgets(line,256,fp)){	
	//	add for to read substring
		regcomp(&regex,keyword,0);
		
		ext = strdup(line);
		token_ext=strtok(ext,".");
		token_ext=strtok(NULL,"\t");
		
		token = strtok(line,"\t");
		if(regexec(&regex,token,0,NULL,0)!= 0){
			continue;
		}
		sprintf(buffer,"Nama : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Publisher : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Tahun Publishing : %s \n",token);
		send(client[i]->sock,buffer,256,0);
		sprintf(buffer,"Ekstensi File : %s \n",token_ext);
		send(client[i]->sock,buffer,256,0);
		token = strtok(NULL,"\t");
		sprintf(buffer,"Filepath : %s \n",token);
		send(client[i]->sock,buffer,256,0);

	}
	fclose(fp);
}

char* getdata(char buffer[],int index) {

	memset(client[index]->input,0,256);
	while(strlen(client[index]->input)==0);
	strcpy(buffer,client[index]->input);
	memset(client[index]->input,0,256);
	return buffer;
}

void *server_main_routine(void *arg){
	int i = *(int*) arg-1;
	char buffer[256];
	char buffer_name[256];
	while(1){
		memset(buffer,0,256);	
		if(i != active_client){
			send(client[i]->sock,"not active client",256,0);
			getdata(buffer,i);
			continue;
			
		}
		if(client[i]->logged == 0){
			sprintf(buffer,"pilih opsi :\n1. register\n2. login\n");
			send(client[i]->sock,buffer,256,0);
			getdata(buffer,i);
			if(strcmp(buffer,"1")==0){
				send(client[i]->sock,"New Username:",256,0);
				getdata(client[i]->username,i);
				send(client[i]->sock,"New Password:",256,0);
				getdata(client[i]->password,i);
				regis(client[i]);
			}
			else if(strcmp(buffer,"2")==0){
				send(client[i]->sock,"Username:",256,0);
				getdata(client[i]->username,i);
				send(client[i]->sock,"Password:",256,0);
				getdata(client[i]->password,i);
				login(client[i]);
				if (client[i]->logged == 1){
					send(client[i]->sock,"Login success\n",256,0);
				}
				else{
					send(client[i]->sock,"invalid username or password\n",256,0);
				}
			}
			else{
				send(client[i]->sock,"Invalid Command\n",256,0);
			}
		}
		if(client[i]->logged == 1){
			sprintf(buffer,"\nMasukkan command \n(add,download,delete,see,find,exit)\n");
			send(client[i]->sock,buffer,256,0);
			getdata(buffer,i);
			for(int strindex=0;buffer[strindex];strindex++){
				buffer[strindex] = tolower(buffer[strindex]);
			}
			if(strcmp(buffer,"add")== 0){
				send(client[i]->sock,"Nama buku: ",256,0);
				getdata(buffer_name,i);		
				if(file_exist(buffer_name)){
					send(client[i]->sock,"Buku dengan nama ini sudah ada",256,0);
					continue;
				}
				tambah(buffer_name);
				writelog("Tambah",buffer_name,client[i]);
				tambah("\t");
				send(client[i]->sock,"Publisher: ",256,0);
				getdata(buffer,i);
				tambah(buffer);
				tambah("\t");
				send(client[i]->sock,"Tahun Publikasi: ",256,0);
				getdata(buffer,i);
				tambah(buffer);
				tambah("\t");
				send(client[i]->sock,"File Path: ",256,0);
				getdata(buffer,i);
				if(buffer[strlen(buffer)-2]=='/'){
					send(client[i]->sock,"foota ",256,0);
					int len =strlen(buffer);
					buffer[--len] = '\0';
				}
			//	tambah(buffer);
				getcwd(path,256);
				sprintf(path,"%s/FILES/%s/",path,buffer);
				rek_mkdir(path);
				tambah(path);
				sprintf(path,"%s/%s",path,buffer_name);
				
				tambahF(path);
				tambah("\n");
			}
			else if(strcmp(buffer,"delete")== 0){
				send(client[i]->sock,"Nama buku akan didelete: ",256,0);
				getdata(buffer,i);
				if(file_exist(buffer)){
					delete(buffer);
					writelog("Hapus",buffer,client[i]);
				}
				else{
					send(client[i]->sock,"File tidak ada\n",256,0);
				}
			}
			else if(strcmp(buffer,"download")== 0){
				send(client[i]->sock,"Nama buku akan didownload: ",256,0);
				getdata(buffer,i);
				if(file_exist(buffer)){
					send(client[i]->sock,"_download_start_",256,0);
					download(buffer);
					send(client[i]->sock,"_download_done_",256,0);
				}
				else{
					send(client[i]->sock,"File tidak ada\n",256,0);
				}
		
			}
			else if(strcmp(buffer,"see")== 0){
				see();
			}
			else if(strcmp(buffer,"find")==0){
				send(client[i]->sock,"Masukkan keyword: ",256,0);
				getdata(buffer,i);
				find(buffer);
			}
			else if(strcmp(buffer,"exit")==0){
				active_client++;
				if(active_client>5){
					active_client = 0;
				}
			}
			else{
				send(client[i]->sock,"Invalid Command\n",256,0);
			}
		}	
	}	
}


void *server_scan_routine(void *arg){
	int i = *(int*) arg-1;
	char buffer[256];
	while (1){
			recv(client[i]->sock,buffer,256,0);
			strcpy(client[i]->input, buffer);
	}
}


int main(int argc, char const *argv[]) {
	FILE *fp = fopen("akun.txt","a");
	fclose(fp);
	fp = fopen("files.tsv","a");
	fclose(fp);
	fp = fopen("running.log","a");
	//fprintf
	fclose(fp);
	memset(path,0,256);
	getcwd(path,256);
	sprintf(path,"%s/FILES",path);
	struct stat st = {0};
	if (stat(path, &st) == -1){
		mkdir(path,0777);
	}
	
	int opt = 1;
    int server_fd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

      
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

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
	pthread_t socket_thread[5][2];
  	int index_client;
	for(index_client=0;index_client<5;index_client++){
//		printf("\n%d\n",index_client);
		client[index_client] = (user*)malloc(sizeof(user));
		if ((client[index_client]->sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
		    perror("accept");
		    exit(EXIT_FAILURE);
		}
		client[index_client]->logged = 0;
		pthread_create(&socket_thread[index_client][0],NULL,&server_scan_routine,(void*)&index_client);
		pthread_create(&socket_thread[index_client][1],NULL,&server_main_routine,(void*)&index_client);
		
	}
	for(index_client=0;index_client<5;index_client++){
		pthread_join(socket_thread[index_client][0], NULL);
		pthread_join(socket_thread[index_client][1], NULL);
	}
	

	
}
