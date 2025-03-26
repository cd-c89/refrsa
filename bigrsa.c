#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "4096_t.h"

#define LINE_LEN 512

/* bignul should be factored into a library but I didn't add it to 4096_t in time :( */
uint64_t bignul(uint64_t *a) {
    size_t i;
    for  (i = 0 ; i < S && !a[i]; i++) { }
    return i != S;
}

void bighlf(uint64_t *a) {
    size_t i;
    for  (i = 0 ; i < S; i++) { 
        a[i] = a[i] / 2;
        if (i+1 < S) {
            a[i] += a[i+1] << 63;
        }
    }
}

void modexp_h(uint64_t *x, uint64_t *e, uint64_t *n, uint64_t *r) {
    uint64_t t[S];
    if (!bignul(e)) {
        return;
    }
    if (e[0] & 1) {
        e[0] -= 1;
        bigmul(r, x, t);
        bigrem(t, n, r);
        modexp_h(x, e, n, r);
        return;
    } else {
        bighlf(e);
        bigmul(x, x, t);
        bigrem(t, n, x);
        modexp_h(x, e, n, r);
        return;
    }
}

void modexp(uint64_t *x, uint64_t *e, uint64_t *n, uint64_t *r) {
    memset(r, 0, BYTES);
    r[0] = 1;
    modexp_h(x, e, n, r);
}

uint64_t bigh2i(char *h, uint64_t *n) {
	size_t l, i = 0, o; 
	memset(n, 0, BYTES);
	l = strlen(h);
    if (l > 16) {
        o = l - 16;
        while (o > 0) {
            sscanf(h + o, "%lx", &n[i]);
            h[o] = 0;
            i++;
            o -= 16;
        }
    } 
    sscanf(h, "%lx", &n[i]);
	return 0;
}

int is_hex(char c) {
    if (c >= '0' && c <= '9') {
        return 1;
    }
    if (c >= 'a' && c <= 'f') {
        return 1;
    }
    if (c >= 'A' && c <= 'F') {
        return 1;
    }
    return 0;
}

void myscanf(FILE *fp, char *line) {
    size_t i = 0;
    char letter;
    memset(line, 0, LINE_LEN);
    do {
        letter = fgetc(fp);

        if (!is_hex(letter)) {
            line[++i] = 0;
            if (letter != '\n') {
                exit(2);
            }
            return;
        }
        line[i++] = letter;
    } while (i < LINE_LEN);
}

void gonext(FILE *fp) {
    char letter = '!';
    while (letter != '\n') {
        letter = fgetc(fp);
    }
}

int main(int argc, char **argv) {
    uint64_t n[S], e[S], m[S], c[S];
    size_t i = 0;
    FILE *fp = 0;
    char line[LINE_LEN];
    if (argc != 4) {
        exit(1);
    }
    memset(n, 0, BYTES);
    memset(e, 0, BYTES);
    memset(m, 0, BYTES);
    memset(c, 0, BYTES);
    if (argv[1][1] == 'e') {
        fp = fopen("unsafe.pub", "r");
        gonext(fp); /* header, discard */
        myscanf(fp, line); /* n, convert */
        bigh2i(line, n);
    } else if (argv[1][1] == 'd') {
        fp = fopen("unsafe.bad", "r");
        gonext(fp); /* header, discard */
        myscanf(fp, line); /* n, convert */
        bigh2i(line, n);
        gonext(fp); /* e, discard */
    } else {
        exit(3);
    }
    myscanf(fp, line); /* e or d, convert */
    bigh2i(line, e);
    fclose(fp);
    fp = fopen(argv[2], "r");
    argc = fread(m, LINE_LEN, 1, fp);
    fclose(fp);
    modexp(m, e, n, c);
    fp = fopen(argv[3], "w");
    while (c[i]) {
        fwrite(c+i, sizeof(uint64_t), 1, fp);
        i++;
    }
    fclose(fp);
    return 0;
}
