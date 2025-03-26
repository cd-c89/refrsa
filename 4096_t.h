/* 4096_t.h */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define S (size_t)(4096 / 64)
#define BYTES S * sizeof(uint64_t)

void seebig(uint64_t *a); 
uint64_t bigadd(uint64_t *in0, uint64_t *in1, uint64_t *sum); 
uint64_t bigsub(uint64_t *min, uint64_t *sub, uint64_t *dif); 
uint64_t bigmul(uint64_t *in0, uint64_t *in1, uint64_t *out); 
uint64_t bigdiv(uint64_t *num, uint64_t *den, uint64_t *quo, uint64_t *rem); 
uint64_t bigquo(uint64_t *num, uint64_t *den, uint64_t *quo);
uint64_t bigrem(uint64_t *num, uint64_t *den, uint64_t *rem);
