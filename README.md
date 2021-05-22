# Laporan Soal Shift Modul 3

### Kelompok A04
- Ihsannur Rahman Qalbi 05111940000090
- Muhamad Fikri Sunandar 05111940000135
- Reyhan Naufal Rahman 05111940000171

### Daftar isi
1. [NO 1](#NO1)
2. [NO 2](#NO2)
3. [NO 3](#NO3)

### NO1
### 1a
Membuat server yang dapat menerima multiple client yang hanya melayani satu client pada satu saat, dan membuat fungsi registrasi dan login untuk client.

```sh
....
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
.....
```
Saat server dijalankan, akan membuat beberapa file(akun.txt,files.tsv,running.log) yang diperlukan.
```sh
.....
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
....
```
Dari sisi server menerima socket dari client (asumsi maksimal client yang diterima adalah 5) kemudian menjalankan 2 buah thread untuk masing masing socket setelah diterima , satu thread untuk menerima input dari client(server_scan_routine), dan yang lain untuk mengirim output ke client(server_main_routine).

```sh
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
....
```
Dalam server_main_routine, jika thread adalah milik client yang bukan client aktif, server akan terus mengirim pesan "not active client" ke client.
Jika thread adalah milik client yang aktif, server akan memberikan opsi kepada client untuk melakukan register atau login.
```sh

```

## NO2

### 2a
Membuat program perkalian matrix (4X3 dengan 3X6) dan menampilkan hasilnya. 
Kami membuatnya dengan konsep thread, maka kami membuat 3 variabel global.

```sh
pthread_t tid[4];
int mat_emt[4][3], mat_ten[3][6];
int (*hsl)[6];
```

Pertama tid[4] untuk menyimpan thread index dengan ukuran sesuai kebutuhan. Lalu mat_emt dan mat_ten adalah matriks untuk menyimpan nilai matriks. Dan yang terakhir (* hsl)[6] untuk menyimpan matriks hasil yang akan digunakan sebagai shared memory.

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

## Output:
![Output_soal2a](screenshot/soal2/out_soal2a.png)


### 2b
Menerima hasil matriks dari soal2a lalu dibandingkan dengan matriks baru dengan beberapa aturan untuk menghitung angka faktorial. Dan perhitungan tersebut dilakukan dengan konsep thread. 

```sh
pthread_t tid[24];
int (*hsl)[6];
long long int mat_hasil[4][6];
int mat_b[4][6];
```

Pertama tid[24] untuk menyimpan thread index dengan ukuran sesuai kebutuhan. Lalu mat_hasil dan mat_b adalah matriks untuk menyimpan nilai matriks yang nanti dihasilkan dan menyimpan nilai matriks b. Dan yang terakhir (* hsl)[6] untuk menyimpan matriks hasil yang akan digunakan sebagai shared memory.

```sh
long long int faktorial(int a, int b){
    long long int hasil=1;
    //if(a==0 || b==0){
     //   hasil = 0;
   // }else{
   for(int i=a; i>b; i--) {
   	hasil = hasil * i;
   }
    //}
    return hasil;
}
```

Membuat fungsi faktorial dengan parameter int a dan int b. Parameter ini digunakan untuk menghitung nilai faktorial int a dengan pembatas int b. Serta mengembalikan hasilnya. 

```sh
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
                    //mat_hasil[i][j] = faktorial(hsl[i][j], mat_b[i][j]);
                    mat_hasil[i][j] = 0;
                }
            }
            l++;
        }
    }
}
```

Membuat fungsi ops_faktorial() yakni fungsi yang mengeksekusi matriks sesuai dengan dengan deskripsi soal dengan konsep thread. Lalu menyimpan hasil matriksnya ke variabel mat_hasil. 

```sh
    key_t key = 1234;
    int shmid = shmget(key,sizeof(int[4][6]),0666|IPC_CREAT); 
    hsl = shmat(shmid,NULL,0);  
```

Lalu persiapan untuk membuat shared memory yang akan digunakan menerima data dari proses sebelumnya.

```sh
    printf("Hasil soal2a:\n");
    for(int i=0; i<4; i++){
        for(int k=0; k<6; k++)
        {   
            printf("%d ", hsl[i][k]);
        }
        printf("\n");
    }
```

Menampilkan matriks dari proses sebelumnya.

```sh
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
```

Membuat while loop sebanyak thread yang akan dibuat, di dalamnya diisi fungsi pthread_create() yang berisi variabel untuk menyimpan id thread, dan fungsi yang dipanggil oleh thread yakni ops_faktorial(). Gunakan kondisi jika thread gagal didibuat.

```sh
    for (int j=0; j<l; j++) {
        pthread_join(tid[j], NULL);
    }
```
Membuat forloop sebanyak thread yang dibuat untuk melakukan join thread dengan proses utama.

```sh
    printf("\nMatrik hasil\n");
    for(int i=0; i<4; i++) {
        for(int j=0; j<6; j++) {
            printf("%llu ", mat_hasil[i][j]);
        }
        printf("\n");
    }
```

Menampilkan hasil matirks yang diperoleh.

## Output:
![Output_soal2b](screenshot/soal2/out_soal2b.png)

### 2c
Melaksanakan command “ps aux | sort -nrk 3,3 | head -5” dengan IPC Pipes. 

```sh
	int fd1[2], fd2[2];
 
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
	} 
```
Membuat variabel matriks fd1 dan fd2 untuk menyimpan keperluan pembuatan pipe. Melakukan pengecekan apakah pipe berhasil dibuat dengan konsep if (kondisi). 

```sh
	p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 
```
Membuat fork dan mengecek apakah fork berhasil dibuat atau tidak. 

```sh
	// Parent process 
	else if (p > 0) 
	{ 

        dup2(fd1[1], 1);

		close(fd1[0]); 
		close(fd1[1]); 
        close(fd2[0]); 
		close(fd2[1]); 

        char *argv1[] = {"ps", "aux", NULL};
        execv("/bin/ps", argv1);

	} 
```
Di dalam parent process dari fork tadi, Melakukan eksekusi "ps aux" dengan fungsi execv. Serta melaukan pengiriman data dengan fd1 dengan fungsi dup2 ke process child.

```sh
	// child process 
	else
	{
        wait(NULL);
        m = fork(); 

        if (m < 0) 
        { 
            fprintf(stderr, "fork Failed" ); 
            return 1; 
        } 
```
Di child process, melakuakn wait data yang dikirim oleh parent process serta membuat fork baru dan mengecek apakah fork berhasil dibuat atau tidak. 

```sh
	else if(m > 0) 
        {
            dup2(fd1[0], 0);
            close(fd1[1]); 
            close(fd1[0]); 

            dup2(fd2[1], 1);
            close(fd2[0]); 
            close(fd2[1]);
            char *argv1[] = {"sort", "-nrk", "3,3", NULL};
            execv("/bin/sort", argv1);


        }
```
Di dalam anak process baru yang dibuat tadi, melakukan penerimaan data dengan variabel fd1. Serta melakukan eksekusi command "sort", "-nrk", "3,3" dengan fungsi execv. Serta melakukan pengiriman data dengan variabel fd2 dengan fungsi dup2. 

```sh
	else
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
        }
```
Di parent process yang baru dibuat, melakukan wait data dari porcess sebelumnya. Serta menerima data dari process sebelumnya dengan fungsi dup2 dan variabel fd2. Serta melakukan perintah command "head", "-5".

## Output:
![Output_soal2c](screenshot/soal2/out_soal2c.png)

### Kendala:
- Error 1: Tidak bisa melakukan passing parameter ke fungsi melalui thread. Solusinya menggunakan global variabel.
![Eror_01](screenshot/soal2/Eror 01.png)
- Error 2: Tidak bisa menjalankan pipe sebagaimana mestinya. Solusinya menggunakan nested fork.
![Eror_02](screenshot/soal2/Eror 02.png)

## NO3

Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

### `Opsi -f`
Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. 

Output yang dikeluarkan adalah seperti ini :
```
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```

<img width="1383" alt="Screen Shot 2021-05-22 at 13 58 52" src="https://user-images.githubusercontent.com/59334824/119217926-bb82aa80-bb07-11eb-9dbc-f3aef990d6e2.png">

### `Opsi -d`
Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. Contohnya adalah seperti ini:
```bash
$ ./soal3 -d /path/to/directory/
```
<img width="1431" alt="Screen Shot 2021-05-22 at 14 00 20" src="https://user-images.githubusercontent.com/59334824/119218019-2fbd4e00-bb08-11eb-9372-e913b6edf9ce.png">


### ` Opsi \* `
Selain menerima opsi-opsi di atas, program ini menerima opsi `\*`, contohnya ada di bawah ini:
```bash
$ ./soal3 \*
```
<img width="1429" alt="Screen Shot 2021-05-22 at 14 01 30" src="https://user-images.githubusercontent.com/59334824/119217947-e0771d80-bb07-11eb-9b86-93ba535fbeab.png">

### Pembahasan
Pembahasan fungsi dan logika dari soal 3.

`main`
```c
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
```
* Memproses input file dengan argumen `-f`, lalu membuat thread sebanyak file yang dinputkan oleh user. Jika return dari fungsi create itu 1, maka thread yang dijankan rusak dan akan mengeluarkan output `File %d: Sad, gagal:(\n` apabila input berhasil dieksekusi `"File %d: Berhasil Dikategorikan\n`. Program dibuatkan thread nya dan akan menjalankan fungsi process dan akan mempasing argumen `(void *)argv[i]`

```c
else if (strcmp(argv[1], "*") == 0)
    {

        char buff[1337];
        getcwd(buff, sizeof(buff));
        directory = buff;
    }
```
* Memproses input `\*` dan akan mengambil string dari current directory dan akan menyimpannya di variabel directory

```c
else if (strcmp(argv[1], "-d") == 0)
    {
        DIR *dir = opendir(argv[2]);
        if (dir)
        {
            directory = argv[2];
        }
        closedir(dir);
    }
```
* Memproses input -d dan akan mengambil directory yang diinput user dari parameter program dan akan menyimpannya di variabel directory.



`Fungsi getName()`
``` c
char *getName(char *name, char buff[])
{

    char *token = strtok(name, delim);
    while (token != NULL)
    {
        sprintf(buff, "%s", token);
        token = strtok(NULL, delim);
    }
}
```
Fungsi mendefinisikan dua paramater yaitu `name` dan pointernya yaitu `buff[]`
untuk store hasil dari fungsi tersebut.

token akan memecah string dengan delimeter `/` lalu while loop aka berjalan
hingga selesai dan dimasukan ke dalam buffer.


`Fungsi getExt()`
```c

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
```
Fungsi mendefinisikan dua paramater yaitu `name` dan pointernya yaitu `buff[]`
untuk store hasil dari fungsi tersebut dan akan mereturn ekstensi dari sebuah file.

`Fungsi checkDirectory()`
```c
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
```
Fungsi akan membiat directory baru jika menggunakan fungsi `mkdir()`
dengan nama yang di passing dan dengan permission `0775` yang artinya
read dan execute lalu akan menutup kembali.

`Fungsi process()`
```c
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
```
* Pertama-tama menggunakan fungsi cwd dari linux akan mengembalikan string yang akan
di sotre pada variabel cwd.
* Lalu akan meng-copy dari parameter yang dipassing ke variabel buffFrom
* Cek bentuk file yang diinputkan dengan menggunakan `dir`, dimana jika terbuka sebagai directory akan menampilkan error dan thread akan selesai `pthread_exit(0)`
* menutup directory
* mengambil nama file dan mendapatkan extensinya
* lalu mengkonversikan ke huruf kecil dengan fungsi `toLower`
* lalu akan merename ` sprintf(buffTo, "%s/%s/%s", cwd, buffExt, buffName)` dengan fungsi rename `rename(buffFrom, buffTo)`


