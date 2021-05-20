# Laporan Soal Shift Modul 3

### Kelompok A04
- Ihsannur Rahman Qalbi 05111940000090
- Muhamad Fikri Sunandar 05111940000135
- Reyhan Naufal Rahman 05111940000171

### Daftar isi
1. [NO 1](#NO1)
2. [NO 2](#NO2)
3. [NO 3](#NO3)

## NO2

### 2a
Membuat program perkalian matrix (4X3 dengan 3X6) dan menampilkan hasilnya. 
Kami membuatnya dengan konsep thread, maka kami membuat 3 variabel global.

```sh
pthread_t tid[4];
int mat_emt[4][3], mat_ten[3][6];
int (*hsl)[6];
```

Pertama tid[4] untuk menyimpan thread index dengan ukuran sesuai kebutuhan. Lalu mat_emt dan mat_ten adalah matriks untuk menyimpan nilai matriks. Dan yang terakhir (* * * hsl)[6] untuk menyimpan matriks hasil yang akan digunakan sebagai shared memory.

```sh
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
```

Membuat fungsi pengali_mat() untuk mengalikan matriks 4X3 dengan matriks 3X6. Di fungsi ini juga dibuat dengan konsep thread sehingga perkalian matriks dilakukan secara bersamaan tiap barisnya. 

```sh
key_t key = 1234;
int shmid = shmget(key,sizeof(int[4][6]),0666|IPC_CREAT); 
hsl = shmat(shmid,NULL,0);  
```

Lalu persiapan untuk membuat shared memory yang akan digunakan untuk proses selanjutnya.

```sh
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
```

Membuat while loop sebanyak thread yang akan dibuat, di dalamnya diisi fungsi pthread_create() yang berisi variabel untuk menyimpan thread, dan fungsi yang dipanggil oleh thread. Gunakan kondisi jika thread gagal didibuat.

```sh
    for (int j=0; j<i; j++) {
        pthread_join(tid[j], NULL);
    }
```

Membuat forloop sebanyak thread yang dibuat untuk melakukan join thread dengan proses utama.

```sh
    printf("\nHasil perkalian:\n");
    for(int i=0; i<4; i++){
        for(int k=0; k<6; k++)
        {
            printf("%d ", hsl[i][k]);
        }
        printf("\n");
    }
```

Menampilkan hasil perkalian kedua matriks.


2b. Menerima hasil matriks dari soal2a lalu dibandingkan dengan matriks baru dengan beberapa aturan untuk menghitung angka faktorial. Dan perhitungan tersebut dilakukan dengan konsep thread. 

[code]

Pertama tid[24] untuk menyimpan thread index dengan ukuran sesuai kebutuhan. Lalu mat_hasil dan mat_b adalah matriks untuk menyimpan nilai matriks yang nanti dihasilkan dan menyimpan nilai matriks b. Dan yang terakhir (*hsl)[6] untuk menyimpan matriks hasil yang akan digunakan sebagai shared memory.

[code]

Membuat fungsi faktorial dengan parameter int a dan int b. Parameter ini digunakan untuk menghitung nilai faktorial int a dengan pembatas int b. Serta mengembalikan hasilnya. 

[code]

Membuat fungsi ops_faktorial() yakni fungsi yang mengeksekusi matriks sesuai dengan dengan deskripsi soal dengan konsep thread. Lalu menyimpan hasil matriksnya ke variabel mat_hasil. 

[code]

Lalu persiapan untuk membuat shared memory yang akan digunakan menerima data dari proses sebelumnya.

[code]

Menampilkan matriks dari proses sebelumnya.

[code]

Membuat while loop sebanyak thread yang akan dibuat, di dalamnya diisi fungsi pthread_create() yang berisi variabel untuk menyimpan id thread, dan fungsi yang dipanggil oleh thread yakni ops_faktorial(). Gunakan kondisi jika thread gagal didibuat.

[code]

Membuat forloop sebanyak thread yang dibuat untuk melakukan join thread dengan proses utama.

[code]

Menampilkan hasil matirks yang diperoleh.

2c. Melaksanakan command “ps aux | sort -nrk 3,3 | head -5” dengan IPC Pipes. 

[[int fd1[2], fd2[2];
 
	pid_t p, m; 

	if (pipe(fd1)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 

    if (pipe(fd2)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} ]]

Membuat variabel matriks fd1 dan fd2 untuk menyimpan keperluan pembuatan pipe. Melakukan pengecekan apakah pipe berhasil dibuat dengan konsep if (kondisi). 

[[p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} ]]

Membuat fork dan mengecek apakah fork berhasil dibuat atau tidak. 

[[// Parent process 
	else if (p > 0) 
	{ 

        dup2(fd1[1], 1);

		close(fd1[0]); 
		close(fd1[1]); 
        close(fd2[0]); 
		close(fd2[1]); 

        char *argv1[] = {"ps", "aux", NULL};
        execv("/bin/ps", argv1);

	} ]]

Di dalam parent process dari fork tadi, Melakukan eksekusi "ps aux" dengan fungsi execv. Serta melaukan pengiriman data dengan fd1 dengan fungsi dup2 ke process child.

[[// child process 
	else
	{
        wait(NULL);
        m = fork(); 

        if (m < 0) 
        { 
            fprintf(stderr, "fork Failed" ); 
            return 1; 
        } ]]

Di child process, melakuakn wait data yang dikirim oleh parent process serta membuat fork baru dan mengecek apakah fork berhasil dibuat atau tidak. 

[[else if(m > 0) 
        {
            dup2(fd1[0], 0);
            close(fd1[1]); 
            close(fd1[0]); 

            dup2(fd2[1], 1);
            close(fd2[0]); 
            close(fd2[1]);
            char *argv1[] = {"sort", "-nrk", "3,3", NULL};
            execv("/bin/sort", argv1);


        }]]

Di dalam anak process baru yang dibuat tadi, melakukan penerimaan data dengan variabel fd1. Serta melakukan eksekusi command "sort", "-nrk", "3,3" dengan fungsi execv. Serta melakukan pengiriman data dengan variabel fd2 dengan fungsi dup2. 

[[else
        {
            wait(NULL);
            dup2(fd2[0], 0);

            close(fd1[1]); 
            close(fd1[0]);
            close(fd2[0]); 
		    close(fd2[1]); 

            char *argv1[] = {"head", "-5", NULL};
            execv("/bin/head", argv1);
            exit(0);
        }      ]]

Di parent process yang baru dibuat, melakukan wait data dari porcess sebelumnya. Serta menerima data dari process sebelumnya dengan fungsi dup2 dan variabel fd2. Serta melakukan perintah command "head", "-5".
