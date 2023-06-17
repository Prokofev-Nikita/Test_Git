#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "func.h"

#define SIZE_OF_CACHE 5
#define  MAX_SIZE_OF_DOMAIN 126


int main() {
    LRUCache *cache = createCache(SIZE_OF_CACHE);
    do {
        printf("Если вы хотите вывести кэш -- введите 1!\n");
        printf("Если вы хотите произвести поиск в DNS-файле по типу 1 -- введите 2!\n");
        printf("Если вы хотите произвести поиск в DNS-файле по типу 2 -- введите 3!\n");
        printf("Если вы хотите добавить запись по типу 1 в DNS-файл-- введите 4!\n");
        printf("Если вы хотите добавить запись по типу 2 в DNS-файл-- введите 5!\n");
        printf("Если вы хотите выйти из программы -- введите 6!\n");
        u_int8_t type = 0;
        while(scanf("%i", &type) != 1 || type > 6 || type < 1){
            printf("Такого варианта нет,введите ещё раз!\n");
            int ch;
            while ((ch = getchar()) != EOF && ch != '\n');
        }
        switch (type) {
            case 1:
            {
                printCache(cache);
                break;
            }
            case 2:
            {
                FILE* fp = fopen("dns.txt","r+");
                char key[MAX_SIZE_OF_DOMAIN];
                printf("Введите имя Домена:\n");
                scanf("%s",key);
                searchAndPrint(cache,fp,key,1);
                fclose(fp);
                break;
            }
            case 3:
            {
                FILE* fp = fopen("dns.txt","r+");
                char key[MAX_SIZE_OF_DOMAIN];
                printf("Введите имя Домена:\n");
                scanf("%s",key);
                searchAndPrint(cache,fp,key,2);
                fclose(fp);
                break;
            }
            case 4:
            {
                FILE* fp = fopen("dns.txt","r+");
                putInFile(fp,1);
                fclose(fp);
                break;
            }
            case 5:
            {
                FILE* fp = fopen("dns.txt","r+");
                putInFile(fp,2);
                fclose(fp);
                break;

            }
            case 6:
            {
                freeCache(cache);
                return 0;
            }
        }

    }while(1);
}