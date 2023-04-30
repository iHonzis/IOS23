#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

sem_t *semafor; // semafor pro kritickou sekci výpisu
sem_t *q1; //POSTem se zařadí do fronty
sem_t *qq1; //úředník tímto volá zákazníka
sem_t *qqq1; //úředník začne vyřizovat zákazníka

sem_t *q2; //POSTem se zařadí do fronty
sem_t *qq2; //úředník tímto volá zákazníka
sem_t *qqq2; //úředník začne vyřizovat zákazníka

sem_t *q3; //POSTem se zařadí do fronty
sem_t *qq3; //úředník tímto volá zákazníka
sem_t *qqq3; //úředník začne vyřizovat zákazníka

sem_t *q4; //značí úředníkům že je zavřeno

int *linecount = NULL; //číslo řádku
int *customer = NULL; //počet zákazníků
int *officer = NULL; //počet úředníků

int *fr1 = NULL; //počet lidí v 1. frontě
int *fr2 = NULL; //počet lidí v 2. frontě
int *fr3 = NULL; //počet lidí v 3. frontě
FILE *file; //proj2.out
bool *otevreno = NULL;

/**
 * @brief funkce pro inicializaci sdílených proměnných a semaforů
 *
 */
void shared(void){
    semafor = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    semafor = sem_open("/xhesja00", O_CREAT | O_EXCL, 0666, 1);

    q1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    q1 = sem_open("/xhesja00-q1", O_CREAT | O_EXCL, 0666, 0);

    qq1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qq1 = sem_open("/xhesja00-qq1", O_CREAT | O_EXCL, 0666, 0);

    qqq1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qqq1 = sem_open("/xhesja00-qqq1", O_CREAT | O_EXCL, 0666, 0);

    q2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    q2 = sem_open("/xhesja00-q2", O_CREAT | O_EXCL, 0666, 0);

    qq2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qq2 = sem_open("/xhesja00-qq2", O_CREAT | O_EXCL, 0666, 0);

    qqq2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qqq2 = sem_open("/xhesja00-qqq2", O_CREAT | O_EXCL, 0666, 0);

    q3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    q3 = sem_open("/xhesja00-q3", O_CREAT | O_EXCL, 0666, 0);

    qq3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qq3 = sem_open("/xhesja00-qq3", O_CREAT | O_EXCL, 0666, 0);

    qqq3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    qqq3 = sem_open("/xhesja00-qqq3", O_CREAT | O_EXCL, 0666, 0);

    q4 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    q4 = sem_open("/xhesja00-q4", O_CREAT | O_EXCL, 0666, 0);

    linecount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *linecount = 1;
    customer = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *customer = 1;
    officer = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *officer = 1;

    fr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *fr1 = 0;

    fr2 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *fr2 = 0;

    fr3 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *fr3 = 0;

    otevreno = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *otevreno = true;
}

/**
 * @brief funkce pro čekání
 *
 * @param cekani čas čekání v milisekundách
 */
void spanek(int cekani){
        usleep( (rand()%(cekani+1)) *1000);
    }

/**
 * @brief funkce pro zničení semaforů
 *
 */
void cleanup(void){
    munmap(semafor, sizeof(sem_t));
    munmap(q1, sizeof(sem_t));
    munmap(qq1, sizeof(sem_t));
    munmap(qqq1, sizeof(sem_t));
    munmap(q2, sizeof(sem_t));
    munmap(qq2, sizeof(sem_t));
    munmap(qqq2, sizeof(sem_t));
    munmap(q3, sizeof(sem_t));
    munmap(qq3, sizeof(sem_t));
    munmap(qqq3, sizeof(sem_t));
    munmap(q4, sizeof(sem_t));

    //deleting
    sem_close(semafor);
    sem_unlink("/xhesja00");
    sem_close(q1);
    sem_unlink("/xhesja00-q1");
    sem_close(qq1);
    sem_unlink("/xhesja00-qq1");
    sem_close(qqq1);
    sem_unlink("/xhesja00-qqq1");
    sem_close(q2);
    sem_unlink("/xhesja00-q2");
    sem_close(qq2);
    sem_unlink("/xhesja00-qq2");
    sem_close(qqq2);
    sem_unlink("/xhesja00-qqq2");
    sem_close(q3);
    sem_unlink("/xhesja00-q3");
    sem_close(qq3);
    sem_unlink("/xhesja00-qq3");
    sem_close(qqq3);
    sem_unlink("/xhesja00-qqq3");
    sem_close(q4);
    sem_unlink("/xhesja00-q4");

}

//získané argumenty
static int zakaznici = 0;
static int urednici = 0;
static int cekani = 0;
static int prestavka = 0;
static int otviracka = 0;

/**
 * @brief funkce akcí zákazníka
 *
 * @param i - číslo zákazníka
 * @param random - číslo služby
 */
void zakaznik(int i, int random){
            sem_wait(semafor);
            fprintf(file, "%d: Z %d: started\n", (*linecount)++, i+1);
            fflush(file);
            sem_post(semafor);

        srand(time(NULL)); //čekání náhodný čas
            int ceka = rand() % (cekani+1);
        spanek(ceka);

            if(*otevreno == true){

                    if(otevreno == false){
                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: going home\n", (*linecount)++, i+1);
                    fflush(file);
                    sem_post(semafor);
            }
                //první služba
                if(random == 1){
                    (*fr1)++;
                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: entering office for a service %d\n", (*linecount)++, i+1, random);
                    fflush(file);
                    sem_post(semafor);

                    sem_post(q1);


                    sem_wait(qq1);
                    (*fr1)--;
                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: called by office worker\n", (*linecount)++, i+1);
                    sem_post(semafor);

                    sem_post(qqq1);
                    int pockej = rand() % 10+1;
                    spanek(pockej);


                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: going home\n", (*linecount)++, i+1);
                    fflush(file);
                    sem_post(semafor);
                } //druhá služba
                else if(random == 2){
                    (*fr2)++;
                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: entering office for a service %d\n", (*linecount)++, i+1, random);
                    fflush(file);
                    sem_post(semafor);

                    sem_post(q2);
                    

                    sem_wait(qq2);
                    (*fr2)--;

                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: called by office worker\n", (*linecount)++, i+1);
                    sem_post(semafor);
                    sem_post(qqq2);
                    int pockej = rand() % 10+1;
                    spanek(pockej);


                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: going home\n", (*linecount)++, i+1);
                    fflush(file);
                    sem_post(semafor);
                } //třetí služba
                else if(random == 3){
                    (*fr3)++;
                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: entering office for a service %d\n", (*linecount)++, i+1, random);
                    fflush(file);
                    sem_post(semafor);

                    sem_post(q3);


                    sem_wait(qq3);
                    (*fr3)--;

                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: called by office worker\n", (*linecount)++, i+1);
                    sem_post(semafor);
                    sem_post(qqq3);
                    int pockej = rand() % 10+1;
                    spanek(pockej);


                    sem_wait(semafor);
                    fprintf(file, "%d: Z %d: going home\n", (*linecount)++, i+1);
                    fflush(file);
                    sem_post(semafor);
                }
                return;
            }
            else{
                sem_wait(semafor);
                fprintf(file, "%d: Z %d: going home\n", (*linecount)++, i+1);
                fflush(file);
                sem_post(semafor);
                return;
            }
}

/**
 * @brief funkce akcí úředníka
 *
 * @param i - číslo úředníka
 */
void urednik(int i){
            sem_wait(semafor);
            fprintf(file, "%d: U %d: started\n",(*linecount)++, i+1);
            fflush(file);
            sem_post(semafor);

            // výběr služby
            while((*customer) != 0)
            {   //první služba
                if(*fr1 > 0){
                    sem_wait(q1);

                    sem_post(qq1);

                    sem_wait(qqq1);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: serving a service of type 1\n",(*linecount)++, i+1);
                    sem_post(semafor);
                    //už je de faccto zpracován
                    (*customer)--;
                    int pockej1 = rand() % 10+1;
                    //simulace zpracování
                    spanek(pockej1);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: service finished\n", (*linecount)++, i+1);
                    sem_post(semafor);
                    //vrací se do fronty
                    continue;
                } //druhá služba
                else if(*fr2 > 0){
                    sem_wait(q2);

                    sem_post(qq2);

                    sem_wait(qqq2);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: serving a service of type 2\n",(*linecount)++, i+1);
                    sem_post(semafor);
                    // už je de faccto zpracován
                    (*customer)--;
                    int pockej2 = rand() % 10+1;
                    //simulace zpracování
                    spanek(pockej2);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: service finished\n", (*linecount)++, i+1);
                    sem_post(semafor);
                    //vrací se do fronty
                    continue;
                }   //třetí služba
                else if(*fr3 > 0){
                    sem_wait(q3);

                    sem_post(qq3);

                    sem_wait(qqq3);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: serving a service of type 3\n",(*linecount)++, i+1);
                    sem_post(semafor);
                    //už je de faccto zpracován
                    (*customer)--;
                    int pockej3 = rand() % 10+1;
                    //simulace zpracování
                    spanek(pockej3);
                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: service finished\n", (*linecount)++, i+1);
                    sem_post(semafor);
                    //vrací se do fronty
                    continue;
                }
                else if (*otevreno == false){ //občas se stane, že úředník zůstane sám a musí se ukončit

                    sem_wait(semafor);
                    fprintf(file, "%d: U %d: going home\n", (*linecount)++, i+1);
                    sem_post(semafor);
                }
            else { //zachycení úředníků, kteří by neměli z čeho brát
                    if(*otevreno == true){
                                sem_wait(semafor);
                                fprintf(file, "%d: U %d: taking break\n", (*linecount)++, i+1);
                                sem_post(semafor);
                                srand(time(NULL));
                                int ceka = rand() % (prestavka+1);
                                spanek(ceka);
                                sem_wait(semafor);
                                fprintf(file, "%d: U %d: break finished\n", (*linecount)++, i+1);
                                sem_post(semafor);

                    if(*customer - *officer <= 1){
                        if(*otevreno == true){
                                sem_wait(semafor);
                                fprintf(file, "%d: U %d: taking break\n", (*linecount)++, i+1);
                                sem_post(semafor);
                                srand(time(NULL));
                                int ceka = rand() % (prestavka+1);
                                spanek(ceka);
                                sem_wait(semafor);
                                fprintf(file, "%d: U %d: break finished\n", (*linecount)++, i+1);
                                sem_post(semafor);
                        }
                    }
                                continue;
                    }
                }
            }
            // čeká na zavření
            sem_wait(q4);
                sem_wait(semafor);
                fprintf(file, "%d: U %d: going home\n", (*linecount)++, i+1);
            sem_post(semafor);
            sem_post(q4);
}

int main(int argc, char *argv[]) {

        if(argc != 6){
            fprintf(stderr,"špatný počet argumentů, očekává se 5\n");
            cleanup();
            return 1;}

        if((file = fopen("proj2.out", "w")) == NULL){
            fprintf(stderr,"nepodařilo se otevřít soubor\n");
            cleanup();
            return 1;}

        char *end;
        zakaznici = strtol(argv[1], &end, 10); //převedení na číslo
        if(*end != '\0'){
            fprintf(stderr,"1. argument není celé číslo\n");
            cleanup();
        return 1;}
        else {
            if(zakaznici <= 0){
                fprintf(stderr,"počet zákazníků musí být kladné číslo\n");
                cleanup();
                return 1;}
        }
        urednici = strtol(argv[2], &end, 10); //převedení na číslo
        if(*end != '\0'){
            fprintf(stderr,"2. argument není celé číslo\n");
            cleanup();
        return 1;}
        else {
            if(urednici <= 0){
                fprintf(stderr,"počet úředníků musí být kladné číslo\n");
                cleanup();
                return 1;}
        }

        cekani = strtol(argv[3], &end, 10); //převedení na číslo
        if(*end != '\0'){
            fprintf(stderr,"3. argument není celé číslo\n");
            cleanup();
        return 1;}
        else {
            if(cekani <= 0 || cekani > 100000){
                fprintf(stderr,"čekací doba není v rozsahu 1-10 000\n");
                cleanup();
                return 1;}
        }

        prestavka = strtol(argv[4], &end, 10); //převedení na číslo
        if(*end != '\0'){
            fprintf(stderr,"4. argument není celé číslo\n");
            cleanup();
        return 1;}
        else {
            if(prestavka <= 0 || prestavka > 100){
                fprintf(stderr,"přestávka není v rozsahu 1-100\n");
                cleanup();
                return 1;}
        }

        otviracka = strtol(argv[5], &end, 10); //převedení na číslo
        if(*end != '\0'){
            fprintf(stderr,"5. argument není celé číslo\n");
            cleanup();
        return 1;
        }
        else {
            if(otviracka <= 0 || otviracka > 100000){
                fprintf(stderr,"otvírací doba není v rozsahu 1-10 000\n");
                cleanup();
                return 1;}
        }

    setbuf(file, NULL); //nulování bufferu

    shared();

    srand(time(NULL));

    //gettimeofday(&cas, NULL);
    (*customer) =zakaznici;
    (*officer) = urednici;
    pid_t pid = fork(); // tvorba procesu pro zákazníky
    if(pid == 0){
        for(int i = 0; i < zakaznici; i++){
            int random = rand()%3 + 1;
            pid_t zak = fork();
            if(zak == 0){
                zakaznik(i, random);
                exit(0);
            }
            else if(zak < 0){
                fprintf(stderr,"nepodařilo se vytvořit proces\n");
                exit (1);
            }
        }
        exit(0);
    }
    else if(pid > 0){
        for(int i = 0; i < urednici; i++){
            pid_t ured = fork(); // tvorba procesu pro úředníky
            if(ured == 0){
                urednik(i);
                exit(0);
            }
            else if(ured < 0){
                fprintf(stderr,"nepodařilo se vytvořit proces\n");
                exit (1);
            }
        }
    // výpočet jak dlouho bude otevřeno
    int pulka = otviracka/2;
    int open = pulka + rand()%(otviracka - pulka + 1);
    usleep(open*1000); //usleep v milisekundách
    sem_wait(semafor);
    fprintf(file, "%d: closing\n", (*linecount)++);
    fflush(file);
    sem_post(semafor);
    sem_post(q4);
    *otevreno = false;

        cleanup();
        fclose(file);
        exit(0);
    }
    else if(pid < 0){
        fprintf(stderr,"nepodařilo se vytvořit proces\n");
        exit (1);
    }

    while (wait(NULL) > 0); //čekání na děti

    fclose(file);
    cleanup();

    return 0;
}
