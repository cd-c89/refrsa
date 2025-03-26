/* biggmp.c - a drop in 4096_t replacement based on the GMP library */

/*
 * 1. Run on Ubuntu (like lib-gmp on Alpine)
 *     apt install libgmp3-dev
 * 2. Add flag to gcc
 *     gcc ... -lgmp
 */

/*
 *  If truncating is needed, something like this before export should work.
 *
 *  mpz_set_ui(m1, 1);
 *  mpz_mul_2exp(m1, m1, (mp_bitcnt_t)4096);
 *  mpz_mod(m0, m0, m1);
 */

#include <gmp.h>
#include "4096_t.h"

#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#define MIN(A,B) (((A) < (B)) ? (A) : (B))

/* print the big value as a string */
void seebig(uint64_t *a) {
    size_t i;
    for (i = S-1; i < S ; i--) {
        fprintf(stderr, "%016lx ", a[i]); 
        if ((i % 8 == 0 && i)) {
            fprintf(stderr, "\n");
        }       
    }
    fprintf(stderr, "\n\n");
    return;
}

void lambda(uint64_t *a, uint64_t *b, uint64_t *c, void (f)(mpz_t, const mpz_t, const mpz_t)) {
    mpz_t m0, m1;
    memset(c, 0, BYTES);
    mpz_inits(m0, m1, NULL);
    mpz_import(m0, S, -1, sizeof(uint64_t), 0, 0, a);
    mpz_import(m1, S, -1, sizeof(uint64_t), 0, 0, b);
    f(m0,m0,m1);
    mpz_export(c, NULL, -1, sizeof(uint64_t), 0, 0, m0);
    mpz_clears(m0, m1, NULL);
    return;
}

uint64_t bigadd(uint64_t *in0, uint64_t *in1, uint64_t *sum) {
    lambda(in0, in1, sum, mpz_add);
    return 0;
}

uint64_t bigsub(uint64_t *min, uint64_t *sub, uint64_t *dif) {
    lambda(min, sub, dif, mpz_sub);
    return 0;
}

uint64_t bigmul(uint64_t *in0, uint64_t *in1, uint64_t *out) {
    lambda(in0, in1, out, mpz_mul);
    return 0;
}

uint64_t bigdiv(uint64_t *num, uint64_t *den, uint64_t *quo, uint64_t *rem) {
    /* This duplicates work but does work. */
    lambda(num, den, quo, mpz_tdiv_q);
    lambda(num, den, rem, mpz_tdiv_r);
    return 0;
}

uint64_t bigquo(uint64_t *num, uint64_t *den, uint64_t *quo) {
    lambda(num, den, quo, mpz_tdiv_q);
    return 0;
}

uint64_t bigrem(uint64_t *num, uint64_t *den, uint64_t *rem) {
    lambda(num, den, rem, mpz_tdiv_r);
    return 0;
}
