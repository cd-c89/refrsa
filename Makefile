CC = clang # or gcc
CFLAGS = -std=c89 -Wall -Wextra -Werror -Wpedantic -O2 -lgmp
BIGNUM = biggmp.c # or use 4096_t.c

all: bigrsa bigkey

bigrsa: bigrsa.c $(BIGNUM) 4096_t.h
	$(CC) bigrsa.c $(BIGNUM) $(CFLAGS) -o bigrsa -lgmp
	
bigkey: bigkey.c $(BIGNUM) 4096_t.h
	$(CC) bigkey.c $(BIGNUM) $(CFLAGS) -o bigkey -lgmp

clean:
	rm -f bigrsa bigkey unsafe.* *.txt

check: bigrsa bigkey
	./bigkey
	echo "Multiple of four chars." > m.txt
	./bigrsa -e m.txt c.txt
	./bigrsa -d c.txt n.txt
	diff m.txt n.txt