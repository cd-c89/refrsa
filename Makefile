CC = gcc # or clang
CFLAGS = -std=c89 -Wall -Wextra -Werror -Wpedantic -O2
CLIBS = -lgmp # for biggmp and primes
BIGNUM = biggmp.c # or use 4096_t.c

all: bigrsa bigkey

bigrsa: bigrsa.c $(BIGNUM) 4096_t.h
	$(CC) bigrsa.c $(BIGNUM) $(CFLAGS) -o bigrsa $(CLIBS)
	
bigkey: bigkey.c $(BIGNUM) 4096_t.h
	$(CC) bigkey.c $(BIGNUM) $(CFLAGS) -o bigkey $(CLIBS)

clean:
	rm -f bigrsa bigkey unsafe.* *.txt

check: bigrsa bigkey
	./bigkey
	echo "Multiple of four chars." > m.txt
	./bigrsa -e m.txt c.txt
	./bigrsa -d c.txt n.txt
	diff m.txt n.txt
