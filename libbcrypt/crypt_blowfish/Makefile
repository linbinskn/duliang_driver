CC = $(AFL_HOME)/afl-gcc
#cc = gcc
AS = $(CC)
LD = $(CC)
RM = rm -f
CFLAGS = -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wmissing-prototypes -Wstrict-prototypes -Wshadow -Wundef -Wpointer-arith -O2 -fomit-frame-pointer -funroll-loops
ASFLAGS = -c
LDFLAGS = -s

BLOWFISH_OBJS = \
	crypt_blowfish.o x86.o

CRYPT_OBJS = \
	$(BLOWFISH_OBJS) crypt_gensalt.o wrapper.o

TEST_OBJS = \
	$(BLOWFISH_OBJS) crypt_gensalt.o crypt_test.o

TEST_THREADS_OBJS = \
	$(BLOWFISH_OBJS) crypt_gensalt.o crypt_test_threads.o

EXTRA_MANS = \
	crypt_r.3 crypt_rn.3 crypt_ra.3 \
	crypt_gensalt.3 crypt_gensalt_rn.3 crypt_gensalt_ra.3

all: $(CRYPT_OBJS)

man: $(EXTRA_MANS)

$(EXTRA_MANS):
	echo '.so man3/crypt.3' > $@

crypt_blowfish.o: crypt_blowfish.h
crypt_gensalt.o: crypt_gensalt.h
wrapper.o: crypt.h ow-crypt.h crypt_blowfish.h crypt_gensalt.h

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.S.o:
	$(AS) $(ASFLAGS) $*.S

clean:
	$(RM) crypt_test crypt_test_threads *.o $(EXTRA_MANS) core
