#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>

struct args
{
    char *buffer;
};

char buffExt[100];
char buffFileName[1000];
char buffFrom[1000];
char buffTo[1000];
char cwd[1000];

void *process(void *arg);
void checkDir(char buff[]);
char *getExtension(char *name, char buff[]);
char *getFileName(char *name, char buff[]);

int main(int argc, char *argv[])
{
    char *directory;
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
    else if (strcmp(argv[1], "-d") == 0)
    {
        DIR *dir = opendir(argv[2]);
        if (dir)
        {
            directory = argv[2];
        }
        else if (ENOENT == errno)
        {
            printf("Directory tidak ada\n");
            exit(1);
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
    char buff[count][1337];
    int iter = 0;

    dir = opendir(directory);
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            sprintf(buff[iter], "%s/%s", directory, entry->d_name);
            iter++;
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

    getFileName(buffFrom, buffFileName); //misahin .extension nya
    strcpy(buffFrom, (char *)arg);

    getExtension(buffFrom, buffExt); //dapetin extension
    for (int i = 0; i < sizeof(buffExt); i++)
    {
        buffExt[i] = tolower(buffExt[i]);
    }

    strcpy(buffFrom, (char *)arg);

    checkDir(buffExt);
    // printf("File %d: Berhasil Dikategorikan\n", i);

    sprintf(buffTo, "%s/%s/%s", cwd, buffExt, buffFileName);
    rename(buffFrom, buffTo);

    pthread_exit(0);
}

char *getFileName(char *name, char buff[])
{
    char *token = strtok(name, "/");
    while (token != NULL)
    {
        sprintf(buff, "%s", token);
        token = strtok(NULL, "/");
    }
}

char *getExtension(char *name, char buff[])
{
    // char buffFileName[1337];
    char *token = strtok(name, "/");
    while (token != NULL)
    {
        sprintf(buffFileName, "%s", token);
        token = strtok(NULL, "/");
    }
    int count = 0;
    token = strtok(buffFileName, ".");
    while (token != NULL)
    {
        count++;
        sprintf(buff, "%s", token);
        token = strtok(NULL, ".");
    }
    if (count <= 1)
    {
        strcpy(buff, "unknown");
    }

    return buff;
}

void checkDir(char buff[])
{
    DIR *dr = opendir(buff);
    if (ENOENT == errno)
    {
        mkdir(buff, 0775);
        closedir(dr);
    }
}
