#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/stat.h>

char buffExt[100];
char buffName[1000];
char buffFrom[1000];
char cwd[1000];
char buffTo[1000];

char delim[2] = "/";
char delimA[2] = ".";

void checkDirectory(char buff[]);
void *process(void *arg);
char *getExt(char *name, char buff[]);
char *getName(char *name, char buff[]);

int main(int argc, char *argv[])
{
    char *directory;
    // A
    if (strcmp(argv[1], "-f") == 0)
    {
        pthread_t tf[argc - 2];
        for (int i = 2; i < argc; i++)
        {
            if (pthread_create(&tf[i - 2], NULL, &process, (void *)argv[i]) == 1)
            {
                printf("File %d: Sad, gagal:(\n", i - 1);
            }

            printf("File %d: Berhasil Dikategorikan\n", i - 1);
        }
    }
    // c
    else if (strcmp(argv[1], "*") == 0)
    {

        char buff[1337];
        getcwd(buff, sizeof(buff));
        directory = buff;
    }
    // b
    else if (strcmp(argv[1], "-d") == 0)
    {
        DIR *dir = opendir(argv[2]);
        if (dir)
        {
            directory = argv[2];
        }
        closedir(dir);
    }

    int count = 0;
    DIR *dir = opendir(directory);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            count++;
        }
    }

    closedir(dir);

    pthread_t tid[count];
    char buff[count][1000];
    int iteration = 0;

    dir = opendir(directory);
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            sprintf(buff[iteration], "%s/%s", directory, entry->d_name);
            iteration++;
        }
    }

    closedir(dir);

    for (int i = 0; i < count; i++)
    {
        char *test = (char *)buff[i];
        // printf("%s\n", test);
        if (pthread_create(&tid[i], NULL, &process, (void *)test) == 1)
        {
            printf("Yah, gagal disimpan :(\n");
        }
    }
    printf("Direktori sukses disimpan!\n");

    for (int i = 0; i < count; i++)
    {
        pthread_join(tid[i], NULL);
    }
}

void *process(void *arg)
{

    getcwd(cwd, sizeof(cwd));
    //ngedapatin nama file nya

    strcpy(buffFrom, (char *)arg);

    if (access(buffFrom, F_OK) == -1)
    {
        // printf("File %s tidak ada\n", buffFrom);
        pthread_exit(0);
    }
    DIR *dir = opendir(buffFrom); //open directory
    //ngecek kl dia folder dan langsung kleuar. Kl argumen -F tapi malah folder jadi gini

    if (dir)
    {
        // printf("file %d: Sad, gagal\n", i);
        pthread_exit(0);
    }
    closedir(dir);

    getName(buffFrom, buffName); //misahin .extension nya
    strcpy(buffFrom, (char *)arg);

    getExt(buffFrom, buffExt); //dapetin extension
    for (int i = 0; i < sizeof(buffExt); i++)
    {
        buffExt[i] = tolower(buffExt[i]);
    }

    strcpy(buffFrom, (char *)arg);

    checkDirectory(buffExt);
    // printf("File %d: Berhasil Dikategorikan\n", i);

    sprintf(buffTo, "%s/%s/%s", cwd, buffExt, buffName);
    rename(buffFrom, buffTo);

    pthread_exit(0);
}

char *getName(char *name, char buff[])
{

    char *token = strtok(name, delim);
    while (token != NULL)
    {
        sprintf(buff, "%s", token);
        token = strtok(NULL, delim);
    }
}

char *getExt(char *name, char buff[])
{
    char *token = strtok(name, delim);
    while (token != NULL)
    {
        sprintf(buffName, "%s", token);
        token = strtok(NULL, delim);
    }
    int count = 0;
    token = strtok(buffName, delimA);

    while (token != NULL)
    {
        count++;
        sprintf(buff, "%s", token);
        token = strtok(NULL, delimA);
    }

    return buff;
}

void checkDirectory(char name[])
{
    DIR *dr = opendir(name);
    umask(0);
    if (ENOENT == errno)
    {
        mkdir(name, 0775);
        closedir(dr);
    }
}
