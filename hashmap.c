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
    long posicion_og = posicion; //guardamos la posicion original en caso de que demos la vuelta (ya que es arreglo circular)

    //para buscar una casilla vacia
    while (map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL)
    {
        if (is_equal(map->buckets[posicion]->key, key)) //si existe un par con esa key terminamos la funcion sin cambiar nada
        {
            return;
        }
        posicion =(posicion + 1) % map->capacity; //resolucin de colisiones en un arreglo circular
        if (posicion == posicion_og) //este caso ocurre si da una vuelta al arreglo sin encontrar una casilla vacia
        {
            return;
        }
    }

    map->current = posicion;
    map->buckets[posicion] = createPair(key, value);
    map->size++;

    if (map->size / map->capacity > 7.0) //para aumentar el tamaño en caso de que las casillas ocupadas sea > al 70%
    {
        enlarge(map);
    }
}

void enlarge(HashMap * map)
{
    enlarge_called = 1; //no borrar (testing purposes)

    Pair ** arreglo_antiguo = map->buckets; //guardamos el arreglo que vamos a cambiar (auxiliar)
    long capacidad_antigua = map->capacity; //guardamos la capacidad antes de aumentarla * 2

    map->capacity *= 2; //aumentamos la capacidad
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *)); //despues de aumentarla * 2 usamos calloc para que todas las casillas inicien en NULL
    map->size = 0; //aqui ya que dejamos todas las casillas en NULL entonces no hay elementos

    for (long i = 0 ; i < capacidad_antigua ; i++)
    {
        if (arreglo_antiguo[i] != NULL && arreglo_antiguo[i]->key != NULL)
        {
            insertMap(map, arreglo_antiguo[i]->key, arreglo_antiguo[i]->value); //insertamos los elementos que estaban antes de aumentar el arreglo en * 2
        }
    }

    free(arreglo_antiguo); //liberamos la memoria del arreglo que creamos como auxiliar
}


HashMap * createMap(long capacity)
{
    HashMap *map = (HashMap*)malloc(sizeof(HashMap)); //reservar memoria para el nuevo map
    map->buckets = (Pair**)calloc(capacity, sizeof(Pair**)); // calloc lo usamos para que todas las casiilas inicen en NULL
    map->size = 0;
    map->current = -1; // para que el current no empieze desde ninguna casilla (ya que no se recorre nada)
    map->capacity = capacity;
    return map;
}

void eraseMap(HashMap * map,  char * key)
{    
    Pair * Par = searchMap(map, key); //buscar el par
    if (Par == NULL) // si no encontro el par
    {
        return;
    }
    Par->key = NULL; //lo dejamos NULL ya que podemos usarlo en otras inserciones (no es necesario eliminarlo)
    map->size--;//le quitamos uno al size ya que sacamos un elemento

}

Pair * searchMap(HashMap * map,  char * key)
{   
    long posicion = hash(key, map->capacity);
    long posicion_og = posicion; //guardamos la posicion inicial

    while (map->buckets[posicion] != NULL)
    {
        //verificamos si es un par valido y comparamos para saber si es la key que buscamos
        if (map->buckets[posicion]->key != NULL && is_equal(map->buckets[posicion]->key, key))
        {
            map->current = posicion; //si es la key que buscamos actualizamos el current
            return map->buckets[posicion]; //retornamos el bucket ya que contiene la key con el valor
        }

        posicion = (posicion + 1) % map->capacity;
        if (posicion == posicion_og) //dio la vuelta al arreglo circular
        {
            return NULL;
        }
    }

    return NULL; //no estaba la key en el map
}

Pair * firstMap(HashMap * map) //para buscar el primer Par valido
{

    for (long i = 0 ; i < map->capacity ; i++) //recorre el arreglo
    {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL ) //pregunta si el Par valido
        {
            map->current = i; //si encuentra el primer Par valido le asigna su posicion al current asi el nextMap u otra funcion sabe desde donde empezar a buscar
            return map->buckets[i]; //retorna el primer Par encontrado
        }
    }

    return NULL; //no encontro ningun Par valido
}

Pair * nextMap(HashMap * map) //para buscar el siguiente Par valido
{
    for (long i = (map->current + 1) ; i < map->capacity ; i++) //empieza desde el ultimo par valido pero aumentando en 1 su posicion para que no lo tome en cuenta
    {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL ) //pregunta si el Par es valido
        {
            map->current = i; //si lo es guarda el current en la posicion i
            return map->buckets[i]; //retorna el Par
        }
    }

    return NULL; //en caso de no encontrar Par valido
}