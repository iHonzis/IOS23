#include <stdio.h>
#include <stdlib.h>

//TODO odstranit na konci -Werror z Makefile

static int zakaznici = 0;
static int urednici = 0;
static int cekani = 0;
static int prestavka = 0;
static int otviracka = 0;

int main(int argc, char *argv[]) {

        if(argc > 6){
            printf("nadbytek argumentů\n"); //TODO změnit na fprintf(stderr, "příliž mnoho argumentů)
            return 1;
        }

        if(argc < 6){
            printf("nedostatek argumentů\n"); //TODO změnit na fprintf(stderr, "příliž mnoho argumentů)
            return 1;
        }

        char *end;
        zakaznici = strtol(argv[1], &end, 10); //převedení na číslo
        if(*end != '\0'){
            printf("1. argument není celé číslo\n");
        return 0;
        }
        else {
            if(zakaznici <= 0){
                printf("počet zákazníků musí být kladné číslo\n");
                return 1;
            }
        }
        urednici = strtol(argv[2], &end, 10); //převedení na číslo
        if(*end != '\0'){
            printf("2. argument není celé číslo\n");
        return 0;
        }
        else {
            if(urednici <= 0){
                printf("počet úředníků musí být kladné číslo\n");
                return 1;
            }
        }

        cekani = strtol(argv[3], &end, 10); //převedení na číslo
        if(*end != '\0'){
            printf("3. argument není celé číslo\n");
        return 0;
        }
        else {
            if(cekani <= 0 || cekani > 100000){
                printf("čekací doba není v rozsahu 1-10 000\n");
                return 1;
            }
        }

        prestavka = strtol(argv[4], &end, 10); //převedení na číslo
        if(*end != '\0'){
            printf("4. argument není celé číslo\n");
        return 0;
        }
        else {
            if(prestavka <= 0 || prestavka > 100){
                printf("přestávka není v rozsahu 1-100\n");
                return 1;
            }
        }

        otviracka = strtol(argv[5], &end, 10); //převedení na číslo
        if(*end != '\0'){
            printf("5. argument není celé číslo\n");
        return 0;
        }
        else {
            if(otviracka <= 0 || otviracka > 100000){
                printf("otvírací doba není v rozsahu 1-10 000\n");
                return 1;
            }
        }

    return 0;
}
