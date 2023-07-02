#include <stdio.h>
#include <stdlib.h>
#include "partition.h"




////////////
partition_list *insereI(partition_list *lista, partition val) {
  partition_list *novo = (partition_list *)malloc(sizeof(partition_list));
  novo->val = val;
  novo->prox = lista;
  lista = novo;
  return lista;
}

partition create_partition(int ini_time, int size_in_bytes, int name){
  partition p;

  p.ini = ini_time;
  p.name = name;
  p.tam = size_in_bytes;

  return p;
}

////////////
void printa(partition_list *lista) {
  for (partition_list *p = lista; p != NULL; p = p->prox) {
    printf("Nome: %d\nInicio: %d\nTamanho: %d\n", p->val.name, p->val.ini, p->val.tam);
  }
}

//////////
partition_list *retira(partition_list *lista, int name) {
  partition_list *ant = NULL;
  partition_list *p = lista;
  while (p != NULL && p->val.name != name) {
    ant = p;
    p = p->prox;
  }
  if (p == NULL) {
    return lista;
  }
  if (ant == NULL) {
    lista = p->prox;
  } else {
    ant->prox = p->prox;
  }

  free(p);
  return lista;
}
