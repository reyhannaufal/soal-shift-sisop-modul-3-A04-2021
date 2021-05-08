#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

pthread_t tid[24];
int (*hsl)[6];
int mat_hasil[4][6], mat_b[4][6];


int faktorial(int a, int b){
    int hasil=1;
    if(a==0 || b==0){
        hasil = 0;
    }else{
        for(int i=a; i>b; i--) {
            hasil = hasil * i;
        }
    }
    return hasil;
}

void *ops_faktorial(void *arg){
    pthread_t id = pthread_self();
    int l = 0;
    for(int i=0; i<4; i++) {
        for (int j=0; j<6; j++) {
            if(pthread_equal(id, tid[l])){
                if(hsl[i][j]>=mat_b[i][j]) {
                    mat_hasil[i][j] = faktorial(hsl[i][j], (hsl[i][j] - mat_b[i][j]));
                }
                if(mat_b[i][j]>hsl[i][j]){
                    mat_hasil[i][j] = faktorial(hsl[i][j], 1);
                }
                if(mat_b[i][j] == 0 || hsl[i][j]==0){
                    mat_hasil[i][j] = faktorial(hsl[i][j], mat_b[i][j]);
                }
            }
            l++;
        }
    }
}

int main(){

    key_t key = 1234;
    int shmid = shmget(key,sizeof(int[4][6]),0666|IPC_CREAT); 
    hsl = shmat(shmid,NULL,0);  

    printf("Hasil soal2a:\n");
    for(int i=0; i<4; i++){
        for(int k=0; k<6; k++)
        {   
            printf("%d ", hsl[i][k]);
        }
        printf("\n");
    }

    int temp1[4][6] = {
        {9, 5, 10, 5, 7, 10},
        {6, 4, 5, 4, 8, 3},
        {6, 0, 0, 0, 9, 8},
        {8, 6, 3, 8, 8, 9}
    };

    printf("\nMatrix B\n");
    for(int i=0; i<4; i++) {
        for(int j=0; j<6; j++) {
            mat_b[i][j] = temp1[i][j];
            printf("%d ", mat_b[i][j]);
        }
        printf("\n");
    }


    int l=0, err;
    while(l<24){
        err = pthread_create(&(tid[l]), NULL, &ops_faktorial, NULL); //pembuatan thread
        if(err != 0){
            printf("Can't create thread : [%s]\n", strerror(err));
        }else{
            // printf("Crate thread success\n");
        }
        l++;
    }
    for (int j=0; j<l; j++) {
        pthread_join(tid[j], NULL);
    }

    printf("\nMatrik hasil\n");
    for(int i=0; i<4; i++) {
        for(int j=0; j<6; j++) {
            printf("%d ", mat_hasil[i][j]);
        }
        printf("\n");
    }

    shmdt(hsl);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
    return 0;
}