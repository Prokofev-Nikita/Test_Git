#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "func.h"

#define  MAX_SIZE_OF_DOMAIN 126
#define  MAX_SIZE_OF_VALUE 64
#define  MAX_SIZE_OF_TYPE 10
#define  MAX_SIZE_OF_IP 19
#define  MAX_SIZE_OF_MASK 32
#define  MAX_SIZE_OF_CELL_IP 255
#define SIZE_OF_CACHE 5

Node* createNode(char *key, char *value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = strdup(key);
    node->value = strdup(value);
    node->prev = node->next = NULL;
    return node;
}

LRUCache* createCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->count = 0;
    cache->head = cache->tail = NULL;
    cache->map = (Node**)calloc(capacity, sizeof(Node*));
    return cache;
}

int hashFunction(const char* key) {
    return (*key) % SIZE_OF_CACHE;
}

void addToHead(LRUCache* cache, Node* node) {
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head != NULL) {
        cache->head->prev = node;
    }
    cache->head = node;
    if (cache->tail == NULL) {
        cache->tail = cache->head;
    }
}

void removeNode(LRUCache* cache, Node* node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        cache->head = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        cache->tail = node->prev;
    }
}

void put(LRUCache* cache, char *key, char *value) {
    Node* node = createNode(key, value);
    int index = abs((int)hashFunction(key));

    if (cache->count >= cache->capacity) {
        Node* tail = cache->tail;
        cache->map[hashFunction(tail->key)] = NULL;
        removeNode(cache, tail);
        free(tail->key);
        free(tail->value);
        free(tail);
    } else {
        cache->count++;
    }

    addToHead(cache, node);
    cache->map[index] = node;
}

char* get(LRUCache* cache, char *key) {
    int index = abs((int)hashFunction(key));
    Node *node = cache->map[index];
    if(node == NULL) {
        return NULL;
    }
    removeNode(cache, node);
    addToHead(cache, node);
    return node->value;
}

char* getKey(LRUCache* cache, char *key) {
    int index = abs((int)hashFunction(key));
    Node *node = cache->map[index];
    if(node == NULL) {
        return NULL;
    }
    removeNode(cache, node);
    addToHead(cache, node);
    return node->key;
}

void freeCache(LRUCache* cache) {
    Node *node = cache->head;
    while (node != NULL) {
        Node *temp = node;
        node = node->next;
        free(temp->key);
        free(temp->value);
        free(temp);
    }
    free(cache->map);
    free(cache);
}

bool validateIp(const char *ip) {
    char ip_copy[MAX_SIZE_OF_IP];  // Создаем копию IP-адреса для безопасного разбора
    strcpy(ip_copy, ip);

    char *ip_address = strtok(ip_copy, "/");  // Разбиваем IP-адрес и маску

    int num_dots = 0;
    int num_digits = 0;

    for (int i = 0; ip_address[i] != '\0'; i++) {
        if (ip_address[i] == '.') {
            num_dots++;
            num_digits = 0;
        } else if (isdigit(ip_address[i])) {
            num_digits++;
            if (num_digits > 3) return false;
        } else {
            return false;
        }
    }

    if (num_dots != 3) return false;

    unsigned long ip_parts[4];
    int num_parts = sscanf(ip_address, "%lu.%lu.%lu.%lu", &ip_parts[0], &ip_parts[1], &ip_parts[2], &ip_parts[3]);

    if (num_parts != 4) return false;

    for (int i = 0; i < 4; i++) {
        if (ip_parts[i] > MAX_SIZE_OF_CELL_IP) return false;
    }

    char *subnet_mask = strtok(NULL, "/");  // Получаем маску подсети

    if (subnet_mask == NULL) return false;  // Проверяем наличие маски

    int mask = atoi(subnet_mask);  // Преобразуем маску в целое число

    if (mask < 0 || mask > MAX_SIZE_OF_MASK) return false;  // Проверяем допустимый диапазон маски

    return true;
}

void putInFile(FILE* fp,int mode){
    fseek(fp,0,2);
    char key[MAX_SIZE_OF_DOMAIN];
    if(mode == 1){
        char ip[MAX_SIZE_OF_IP];
        printf("Введите  Доменное имя,а затем IP адрес(формат IP адреса: 255.255.255.255/32)\n");
        scanf("%s %s",key,ip);
        if(!validateIp(ip)){
            printf("Неверный формат IP-адреса\nВведите его ещё раз\n");
            while(scanf("%s",ip) != 1 && !validateIp(ip)){
                printf("Неверный формат IP-адреса\nВведите его ещё раз\n");
                int ch;
                while ((ch = getchar()) != EOF && ch != '\n');
            }
        }
        fprintf(fp,"%s",key);
        fprintf(fp, " IN");
        fprintf(fp," A ");
        fprintf(fp,"%s\n",ip);
    }
    if(mode == 2) {
        char value[MAX_SIZE_OF_DOMAIN];
        printf("Введите Доменное имя,а затем на какое оно указывает\n");
        scanf("%s %s",key,value);
        fprintf(fp,"%s",key);
        fprintf(fp, " IN");
        fprintf(fp," CNAME ");
        fprintf(fp,"%s\n",value);
    }
}

int searchInFile(LRUCache* cache,FILE* fp,char* key, int mode){
    do {
        char buffer_domen[MAX_SIZE_OF_DOMAIN];
        char buffer_value[MAX_SIZE_OF_VALUE];
        char buffer_type[MAX_SIZE_OF_TYPE];
        fscanf(fp, "%s %*s %s %s", buffer_domen, buffer_type, buffer_value);
        if (strcmp(buffer_domen, key) == 0 &&(mode == 1 && strcmp(buffer_type,"A") == 0 || mode == 2 && strcmp(buffer_type, "CNAME") == 0)){
            if(mode == 1) {
                put(cache, key, buffer_value);
            }
            char *value = get(cache, key);
            printf("%s : %s\n", key, value);
            rewind(fp);
            return 1;
        }
    } while (!feof(fp));
    return 0;
}

void searchAndPrint(LRUCache* cache,FILE* fp, char* key,int mode){
    char *name = getKey(cache, key);
    if(name == NULL)
    {
        if(searchInFile(cache,fp,key,mode))
            return;
        printf("Такого имени нет и мне грутсно :(\nДобавь меня и мне станет веселее :)\n");
        return;
    }
    if(strcmp(name,key) == 0)
    {
        char *value = get(cache, key);
        if(mode == 1 && validateIp(value)){
            printf("Взято из кэша %s: %s\n", key, value);
            return;
        }
        if(mode == 2 && !validateIp(value)){
            printf("Взято из кэша %s: %s\n", key, value);
            return;
        }
    }
    fseek(fp,0,0);
    if(searchInFile(cache,fp,key,mode))
        return;
    printf("Такого имени нет и мне грутсно :(\nДобавь меня и мне станет веселее :)\n");
}
void printCache(LRUCache* cache) {
    if(cache == NULL){
        printf("Кэш пуст!");
        return;
    }
    Node* node = cache->head;
    printf("Содержимое кэша:\n");
    if(node == NULL) {
        printf("Кэш пуст!\n");
        return;
    }
    while (node != NULL) {
        printf("Ключ: %s, Значение: %s\n", node->key, node->value);
        node = node->next;
    }
}