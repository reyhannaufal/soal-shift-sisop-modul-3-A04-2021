#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 

pthread_t tid[4];
int mat_emt[4][3], mat_ten[3][6];
int (*hsl)[6];

void *pengali_mat(void *arg){
    pthread_t id = pthread_self();

    for(int i=0; i<4; i++) {
        if(pthread_equal(id, tid[i])){
            for(int j=0; j<6; j++) {
                hsl[i][j] = (mat_emt[i][0] * mat_ten[0][j]) + (mat_emt[i][1] * mat_ten[1][j]) + (mat_emt[i][2] * mat_ten[2][j]);
            }
        }
    }

}

int main(){

    key_t key = 1234;
    int shmid = shmget(key,sizeof(int[4][6]),0666|IPC_CREAT); 
    hsl = shmat(shmid,NULL,0);  

    int temp1[4][3] = {
        {4, 1, 2},
		{3, 0, 2},
        {5, 0, 0},
        {3, 1, 2}
    };


    int temp2[3][6] = {
        {1, 2, 2, 0, 3, 1},
        {2, 5, 6, 2, 4, 1},
        {1, 3, 3, 1, 2, 2}
    };

    printf("Matriks 1\n");
    for(int i=0; i<4; i++) {
        for(int j=0; j<3; j++){
            mat_emt[i][j] = temp1[i][j];
            printf("%d ", mat_emt[i][j]);
        }
        printf("\n");
    }

    printf("\nMatriks 2\n");
    for(int i=0; i<3; i++) {
        for(int j=0; j<6; j++){
            mat_ten[i][j] = temp2[i][j];
            printf("%d ", mat_ten[i][j]);
        }
        printf("\n");
    }

    int i=0, err;

    while(i<4)
    {
        err = pthread_create(&(tid[i]), NULL, &pengali_mat, NULL); //pembuatan thread
        if(err != 0){
            printf("Can't create thread : [%s]\n", strerror(err));
        }else{
            //printf("Crate thread success\n");
        }
        i++;
    }
    for (int j=0; j<i; j++) {
        pthread_join(tid[j], NULL);
    }

    printf("\nHasil perkalian:\n");
    for(int i=0; i<4; i++){
        for(int k=0; k<6; k++)
        {
            printf("%d ", hsl[i][k]);
        }
        printf("\n");
    }

    shmdt(hsl);
    exit(0);

    return 0;
}