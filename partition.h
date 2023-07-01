#ifndef _PARTITION_H
#define _PARTITION_H


typedef struct Partition{
    int ini;
    int tam;
    int name;
}partition;

struct Partition_list {
  partition val;
  struct Partition_list *prox;
};
typedef struct Partition_list partition_list;

#endif