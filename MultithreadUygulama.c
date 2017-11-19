#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define BOYUT 1000  
#define THREAD_SAYISI 3

void SayiUret();  //sayıları random üretecek olan fonksiyon.
void *Siralama(void *parametre);  //dizi elemanlarını sıralayan fonksiyon.
void *Birlesme(void *parametre); //dizileri sıralı şekilde birleştiren fonksiyon.
void ThreadYarat();  //threadlerin yaratıldığı fonksiyon.
void DosyayaYaz();  //dosyaya yazma işlemini gerçekleştiren fonksiyon.

pthread_mutex_t lock;  //mutex değişkeni tanımlandı.
int liste[BOYUT];  //elemanları karışık olan listeyi tutan dizi.
int sonuc[BOYUT];  //elemanları sıralanmış olan listeyi tutan dizi.

FILE *dosya;

typedef struct
{
    int baslangic; 
    int bitis;
} parametreler;

int main (int argc, const char * argv[])
{
	dosya = fopen("dizileriGoster.txt", "w+");
	if(dosya == NULL)
	{
		printf("dizileriGoster.txt açılamadı..\n");
		exit(1);
	}
	SayiUret();	
	ThreadYarat();
	DosyayaYaz();
	return 0;
}
void SayiUret()
{
int rnd;
int flag;  //random oluşturulan değerlerin unique olmasını sağlayan algoritma için kullanılan bir değişken.
int i, j;

printf("***RANDOM DİZİ***\n");
fprintf(dosya,"%s","****RANDOM DİZİ***");
    fputc('\n',dosya);

for(i = 0; i < BOYUT; i++) {
     do {
        flag = 1;
        rnd = rand() % (BOYUT) + 1; //BOYUT + 1 olmasının sebebi, sıralandığında ardışık sayılar elde ederek sıralama işleminin doğruluğunu kontrol etmede kolaylık sağlamak.

        for (j = 0; j < i && flag == 1; j++) {  
           if (liste[j] == rnd) {
              flag = 0;
           }
        }
     } while (flag != 1);  //bu döngüde listenin bütün elemanlarına bakılıp unique verilmesi kontrolü sağlanıyor.
     liste[i] = rnd;
    printf("%d.sayi : %d\t",i+1,liste[i]);
	if(i % 10 == 0)
		printf("\n");
	fprintf(dosya,"%d %s %d" ,i+1,".eleman:",liste[i]);
	   fputc('\n',dosya);
}
}

void ThreadYarat()
{
   int i;
   if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex başlatılamadı\n");
        exit(0);
    }

    pthread_t threadler[THREAD_SAYISI];

  //  printf("***DİZİNİN İLK 500 ELEMANI***\n");
    parametreler *veri = (parametreler *) malloc (sizeof(parametreler)); //parametreler tipinde veri değişkeni tanımlamak ve alan tahsisi.
    //veri değişkeni, dizinin ilk 500 elemanının başlangıç ve bitiş indislerini tutuyor.
    veri->baslangic = 0;
    veri->bitis = (BOYUT/2) - 1;
    pthread_create(&threadler[0], 0, Siralama, veri);

  //  printf("***DİZİNİN SON 500 ELEMANI***\n");
    parametreler *veri2 = (parametreler *) malloc (sizeof(parametreler));
    veri2->baslangic = (BOYUT/2);
    veri2->bitis = BOYUT - 1;
    pthread_create(&threadler[1], 0, Siralama, veri2);

   for (i = 0; i < THREAD_SAYISI-1; i++)
   {
 	 pthread_join(threadler[i], NULL);
  	 
   }
    pthread_mutex_destroy(&lock); //mutex nesnesi yok ediliyor. 
    
    parametreler *veri3 = (parametreler *) malloc(sizeof(parametreler));
    veri3->baslangic = 0;
    veri3->bitis = BOYUT;
    pthread_create(&threadler[2], 0, Birlesme, veri3);
    pthread_join(threadler[2], NULL);

}

void *Siralama(void *parametre)
{
    parametreler *p = (parametreler *)parametre;
    int ilk = p->baslangic;
    int son = p->bitis+1;

    int z;

    fprintf(dosya,"%s","****SIRALANMAMIŞ ELEMANLAR***");
    fputc('\n',dosya);
    for(z = ilk; z < son; z++){
     //   printf("%d.Sıralanmamış Eleman : %d\n", z+1, liste[z]);
	fprintf(dosya,"%d %s %d" ,z+1,".eleman:",liste[z]);
	   fputc('\n',dosya);
    }

    printf("\n");
    int i,j,t,k;


    //aşağıdaki işlemin sadece tek thread tarafından gerçekleştirilmesi amacıyla bu blok mutex kilidiyle kitlenmiştir.
    pthread_mutex_lock(&lock);
    //dizi elemanlarını sıralama
    for(i=ilk; i< son; i++)
    {
        for(j=ilk; j< son-1; j++)
        {
            if(liste[j] > liste[j+1])
            {
                t = liste[j];
                liste[j] = liste[j+1];
                liste[j+1] = t;

            }
        }
    }
    pthread_mutex_unlock(&lock);   //mutex kilidi açılır.


    fprintf(dosya,"%s","****SIRALANMIŞ ELEMANLAR***");
    fputc('\n',dosya);
    for(k = ilk; k< son; k++){
          //  printf("%d. Sıralanmış Eleman : %d\n", k+1,liste[k]);
	fprintf(dosya,"%d %s %d" ,k+1,".eleman:",liste[k]);
	   fputc('\n',dosya);
    }

    int x;
    for(x=ilk; x<son; x++)
    {
            sonuc[x] = liste[x];
    }
  printf("\n");
  return NULL;
}

void *Birlesme(void *parametre)
{
    parametreler *p = (parametreler *)parametre;

    int ilk = p->baslangic;
    int son = p->bitis-1;

    int i,j,t;

    //MERGE algoritması kullanılarak sıralama ve birleştirme işlemleri gerçekleştirilir.
    for(i=ilk; i< son; i++)
    {
        for(j=ilk; j< son-i; j++)
        {

            if(sonuc[j] > sonuc[j+1])
            {
                t = sonuc[j];
                sonuc[j] = sonuc[j+1];
                sonuc[j+1] = t;

            }
        }
    }

    int d;
  /*  printf("***SIRALANMIŞ TÜM ELEMANLAR***\n");
	
    for(d=0; d<BOYUT; d++)
    {
        printf("%d.Eleman : %d\t",d+1, sonuc[d]);
	if(d % 10 == 0)
		printf("\n");
    }
*/

    pthread_exit(0);
}

void DosyayaYaz()
{
	int i = 0;
	FILE *fp ;
	fp = fopen("son.txt","w+");
	fprintf(fp,"%s","****SIRALANMIŞ ELEMANLAR***");
	fputc('\n',fp);
	for(i = 0; i<BOYUT; i++)
	{
	   fprintf(fp,"%d %s %d" ,i+1,".eleman:",sonuc[i]);
	   fputc('\n',fp);
	}
	fclose(fp);
}
