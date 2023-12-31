#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fits.h"


// func to convert the size to bit ocuppation O(1)
int quadfunc(int value) { return  pow(2,(value)) - 1; }

// print the bit occupation
void printBits(int num) {
  unsigned short size = sizeof(unsigned short);
  unsigned int maxPow = 1 << (size * 8 - 1);
  int i = 0;

  for (; i < size * 8; ++i) {
    printf("%u ", num & maxPow ? 1 : 0);
    num = num << 1;
  }

  printf("\n");
}

/*check if it is possible to allocate in the stack and return the quantities of
 * bitshifts needed to do it, or -1 if it is not possible to allocate.
 */
int first_fit(int process_tam, short *stack) {
  int bits_translocated, bits_allocated, result;
  int loop_tam = 16 - process_tam + 1;

  bits_translocated = quadfunc(process_tam);

  for (int i = 0; i < loop_tam; i++) {
    result = *stack & bits_translocated;

    if (result == 0) {
      *stack |= bits_translocated;
      return i;
    }

    //printf("\nTentativa %d nao e possivel Result: %d\n", i, result);
    bits_translocated <<= 1;
  }

  return -1;
}

//deve receber o process_tam já tratado pelo tamanho da partição
void fit(short* stack, int qtd_shifts, int bits_translocated){ 
    *stack |= (bits_translocated << qtd_shifts); }

int best_fit(int process_tam, short *stack) {
  int min = 17;
  int bits_translocated = quadfunc(process_tam);
  int pos_to_allcate = -1;
  short copy = *stack;
  int cont = 0;
  int ini = 0;

  for (int i = 0; i < 17; i++) {
    if ((copy & 0x0001) == 0 && i != 16) {
      cont += 1;
    }

    else {
      // printf("%d\n", cont);
      if (cont < min && cont >= process_tam) {
        min = cont;
        pos_to_allcate = ini;
      }

      ini = i + 1;
      cont = 0;
    }

    copy = copy >> 1;
  }

  fit(stack, pos_to_allcate, bits_translocated);

  return pos_to_allcate;
}

int invert_bits(int val) { return val ^ 0xffff; }

int worst_fit(int process_tam, short *stack) {
  int pos_to_allcate = -1;
  int max = 0;
  short copy = *stack;
  int cont = 0;
  int ini = 0;
  int bits_translocated = quadfunc(process_tam);

  for (int i = 0; i < 17; i++) {
    if ((copy & 0x0001) == 0 && i != 16) {
      cont += 1;
    }

    else {
      // printf("%d\n", cont);
      if (cont > max && cont >= process_tam) {
        max = cont;
        pos_to_allcate = ini;
      }

      ini = i + 1;
      cont = 0;
    }

    copy = copy >> 1;
  }

  fit(stack, pos_to_allcate, bits_translocated);


  return pos_to_allcate;
}

void deallocate(int tam_in_kb, int qtd_shifts, short *stack) {
  int bits_allocated = quadfunc(tam_in_kb);
  int bits_in_position = (bits_allocated << qtd_shifts) ^ 0xffff;
  *stack &= bits_in_position;
}

// int main(void) {
//   // short stack = 0x1fff;
//   // printBits(stack);
//   // // int qtd_bitshifts = first_fit(2, &stack);
//   // int result = best_fit(3, &stack);
//   // printf("\nResult: %d\n", result);
//   // printBits(stack);

//   return 0;
// }