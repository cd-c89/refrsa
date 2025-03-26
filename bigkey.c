#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "4096_t.h"
#include <gmp.h>

#define E 0x10001

#define KEYSIZE 512
#define P_WORDS ((KEYSIZE / 64) / 2)

void big_p_(uint64_t *big, size_t words) {
    /* populate from buffer */
    mpz_t m;
    FILE *fp = fopen("/dev/random", "r");
    memset(big, 0, BYTES);
    if (words != fread(big, sizeof(uint64_t), words, fp)) {
        fprintf(stderr, "Failed to read enough bytes\n");
        exit(1);
    }
    fclose(fp);
    mpz_init(m);
    mpz_import(m, S, -1, sizeof(uint64_t), 0, 0, big);
    mpz_nextprime(m, m);
    mpz_export(big, NULL, -1, sizeof(uint64_t), 0, 0, m);
    mpz_clear(m);
    return;
}

uint64_t big_gt(uint64_t *a, uint64_t *b) {
    size_t i = 1;
    for  (i = 1 ; i > 0 && a[i] == b[i] ; i--) { }
    return a[i] > b[i];
}

/* a - b -> c */
/* return the sign bit */
uint64_t signed_sub(uint64_t *a, uint64_t as, uint64_t *b, uint64_t bs, uint64_t *r) {
    if (as == bs) {
        if (big_gt(b, a)) {
            bigsub(b, a, r);
            return !as;
        } else {
            bigsub(a, b, r);
            return as;
        }
    } else {
        bigadd(a, b, r);
        return as;
    }
}

/* bignul should be factored into a library but I didn't add it to 4096_t in time :( */
uint64_t bignul(uint64_t *a) {
    size_t i;
    for  (i = 0 ; i < S && !a[i]; i++) { }
    return i != S;
}


void biggcd(uint64_t *in_a, uint64_t *in_b, uint64_t *r, uint64_t *x) {
    /* https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Pseudocode */
    /* q->quotient, t->temp, p->product (just needed names)*/

    /* okay, we need to use the inputs as workspace and then create local buffers as well */
    uint64_t a[S], b[S], c[S], d[S], q[S], t[S], p[S], *rs[2][2]; /* r,s / old,new / +,- */
    /* ned qtp signs and the sign array */
    uint64_t qs, ts, ps, i, signs[2][2];
    memcpy(a, in_a, BYTES);
    memcpy(b, in_b, BYTES);
    memset(c, 0, BYTES);
    memset(d, 0, BYTES);
    c[0] = 1;
    rs[0][0] = a ;
    rs[0][1] = b ;
    rs[1][0] = c ;
    rs[1][1] = d ;
    /* All inputs were nats, to no sign ~= + sign */
    signs[0][0] = 0 ;
    signs[0][1] = 0 ;
    signs[1][0] = 0 ;
    signs[1][1] = 0 ;

    /* Variable declaration looks good - now need to zero check and add big ops */

    while ( bignul(rs[0][1]) ) {/* uh how do i zero check lol */
        /* div */
        bigquo(rs[0][0], rs[0][1], q); /* q[0] = rs[0][0] / rs[0][1] ; */
        qs = signs[0][0] != signs[0][1];
        
        
        for ( i = 0 ; i < 2 ; i++ ) {

            memcpy(t, rs[i][1], BYTES); /*t[0] = rs[i][1]; */
            ts = signs[i][1] ;

            bigmul(q, rs[i][1], p); /* p[0] = q[0] * rs[i][1]; */
            
            ps = qs != signs[i][1];
            
            signs[i][1] = signed_sub(rs[i][0], signs[i][0], p, ps, rs[i][1]); /* signed_sub(rs[i][0],p,rs[i][1]); */

            memcpy(rs[i][0], t, BYTES); /*rs[i][0] = t[0] ; */
            signs[i][0] = ts ;
        }
    }
    if (x) {/* MMI */
        if (signs[1][0]) {
            bigsub(in_b, rs[1][0], x); /* rs[1][0] = b - rs[1][0]; */
        } else {
            memcpy(x, rs[1][0], BYTES);/* *x = rs[1][0]; */
        }
        
    }
    memcpy(r, rs[0][0], BYTES);/* *r = rs[0][0]; */
}

void biglcm(uint64_t *a, uint64_t *b, uint64_t *l) {
    uint64_t r[S], t[S];
    memset(r, 0, BYTES);
    if (big_gt(a, b)) {
        biggcd(a, b, r, NULL);
    } else {
        biggcd(b, a, r, NULL);
    }
    bigmul(a, b, t);
    bigquo(t, r, l);
}

void bigmmi(uint64_t *l, uint64_t *d) {
    uint64_t r[S], e[S];
    memset(r, 0, BYTES);
    memset(e, 0, BYTES);
    e[0] = E;
    biggcd(e, l, r, d);
}


void toline(FILE *fp, uint64_t *a) {
    size_t i = S-1;
    while (!a[i]) {
        i--;
    }
    for (; i < S ; i--) {
        fprintf(fp, "%016lx", a[i]);
    }
    fprintf(fp, "\n");
    return;
}

void bigkey() {
    uint64_t d[S], n[S], p[S], q[S], l[S], i;
    char *fn, *adj;
    FILE *fp;    
    big_p_(p, P_WORDS); 
    big_p_(q, P_WORDS);
    bigmul(p, q, n);
    /* they're odd so this is known safe */
    p[0]--;
    q[0]--;
    biglcm(p, q, l);
    bigmmi(l, d);
    fn = "unsafe.bad";
    adj = "PRIVATE";
    for (i = 2; i ; i--) {
        fp = fopen(fn, "w");
        fprintf(fp, "-----BEGIN UNSAFE %s KEY-----\n", adj);
        toline(fp, n);
        fprintf(fp, "%016lx\n", (uint64_t)0x10001);
        if (i == 2) {
            toline(fp, d);
        }
        fprintf(fp, "-----END UNSAFE %s KEY-----\n", adj);
        fclose(fp);
        fn = "unsafe.pub";
        adj = "PUBLIC";
    }
    fp = fopen("", "w");
}


int main(){
    bigkey();
    return 0 ;
}
