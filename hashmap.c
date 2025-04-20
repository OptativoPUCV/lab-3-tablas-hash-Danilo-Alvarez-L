#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value)
{
    long posicion = hash(key, map->capacity);
    long posicion_og = posicion;

    while (map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL)
    {
        if (is_equal(map->buckets[posicion]->key, key))
        {
            return;
        }
        posicion =(posicion + 1) % map->capacity;
        if (posicion == posicion_og)
        {
            return;
        }
    }

    map->current = posicion;
    map->buckets[posicion] = createPair(key, value);
    map->size++;

    if (map->size / map->capacity > 7.0)
    {
        enlarge(map);
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)


}


HashMap * createMap(long capacity)
{
    HashMap *map = (HashMap*)malloc(sizeof(HashMap));
    map->buckets = (Pair**)calloc(capacity, sizeof(Pair**));
    map->size = 0;
    map->current = -1;
    map->capacity = capacity;
    return map;
}

void eraseMap(HashMap * map,  char * key)
{    
    Pair * Par = searchMap(map, key);
    if (Par == NULL)
    {
        return;
    }
    Par->key = NULL;
    map->size--;

}

Pair * searchMap(HashMap * map,  char * key)
{   
    long posicion = hash(key, map->capacity);
    long posicion_og = posicion;

    while (map->buckets[posicion] != NULL)
    {
        if (map->buckets[posicion]->key != NULL && is_equal(map->buckets[posicion]->key, key))
        {
            map->current = posicion;
            return map->buckets[posicion];
        }

        posicion = (posicion + 1) % map->capacity;
        if (posicion == posicion_og)
        {
            return NULL;
        }
    }

    return NULL;
}

Pair * firstMap(HashMap * map)
{

    for (long i = 0 ; i < map->capacity ; i++)
    {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL )
        {
            map->current = i;
            return map->buckets[i];
        }
    }

    return NULL;
}

Pair * nextMap(HashMap * map)
{
    for (long i = (map->current + 1) ; i < map->capacity ; i++)
    {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL )
        {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
