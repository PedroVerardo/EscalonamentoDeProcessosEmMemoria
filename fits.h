#define _FITS_H

int first_fit(int process_tam, short *stack);
int best_fit(int process_tam, short *stack);
int worst_fit(int process_tam, short *stack);
void deallocate(int tam_in_kb, int qtd_shifts, short *stack);
void printBits(int num);