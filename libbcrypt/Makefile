CC = $(AFL_HOME)/afl-gcc
#cc = gcc
CFLAGS = $(shell grep '^CFLAGS = ' crypt_blowfish/Makefile | cut -d= -f2-)
.PHONY: crypt_blowfish

all: pass_match_driver.a pass_match_driver pass_mismatch_driver

pass_match_driver: pass_match_driver.c crypt_blowfish
	$(CC) $(CFLAGS) -DTEST_BCRYPT -c pass_match_driver.c
	$(CC) -o pass_match_driver_test pass_match_driver.o crypt_blowfish/*.o

pass_match_driver.a: pass_match_driver.o crypt_blowfish
	ar r pass_match_driver.a pass_match_driver.o crypt_blowfish/*.o

pass_match_driver.o: pass_match_driver.c
	$(CC) $(CFLAGS) -c pass_match_driver.c

pass_mismatch_driver: pass_mismatch_driver.c crypt_blowfish
	$(CC) $(CFLAGS) -DTEST_BCRYPT -c pass_mismatch_driver.c
	$(CC) -o pass_mismatch_driver_test pass_mismatch_driver.o crypt_blowfish/*.o

pass_mismatch_driver.a: pass_mismatch_driver.o crypt_blowfish
	ar r pass_mismatch_driver.a pass_mismatch_driver.o crypt_blowfish/*.o

pass_mismatch_driver.o: pass_mismatch_driver.c
	$(CC) $(CFLAGS) -c pass_mismatch_driver.c

crypt_blowfish:
	$(MAKE) -C crypt_blowfish

clean:
	rm -f *.o pass_match_driver pass_match_driver.a pass_mismatch_driver pass_mismatch_driver.a *~ core
	$(MAKE) -C crypt_blowfish clean
