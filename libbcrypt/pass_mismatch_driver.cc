/*
 * bcrypt wrapper library
 *
 * Written in 2011, 2013, 2014, 2015 by Ricardo Garcia <r@rg3.name>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "bcrypt.h"
#include "crypt_blowfish/ow-crypt.h"

#define RANDBYTES (16)

static int try_close(int fd)
{
	int ret;
	for (;;) {
		errno = 0;
		ret = close(fd);
		if (ret == -1 && errno == EINTR)
			continue;
		break;
	}
	return ret;
}

static int try_read(int fd, char *out, size_t count)
{
	size_t total;
	ssize_t partial;

	total = 0;
	while (total < count)
	{
		for (;;) {
			errno = 0;
			partial = read(fd, out + total, count - total);
			if (partial == -1 && errno == EINTR)
				continue;
			break;
		}

		if (partial < 1)
			return -1;

		total += partial;
	}

	return 0;
}

/*
 * This is a best effort implementation. Nothing prevents a compiler from
 * optimizing this function and making it vulnerable to timing attacks, but
 * this method is commonly used in crypto libraries like NaCl.
 *
 * Return value is zero if both strings are equal and nonzero otherwise.
*/
static int timing_safe_strcmp(const char *str1, const char *str2)
{
	const unsigned char *u1;
	const unsigned char *u2;
	int ret;
	int i;

	int len1 = strlen(str1);
	int len2 = strlen(str2);

	/* In our context both strings should always have the same length
	 * because they will be hashed passwords. */
	if (len1 != len2)
		return 1;

	/* Force unsigned for bitwise operations. */
	u1 = (const unsigned char *)str1;
	u2 = (const unsigned char *)str2;

	ret = 0;
	for (i = 0; i < len1; ++i)
		ret |= (u1[i] ^ u2[i]);

	return ret;
}

int bcrypt_gensalt(int factor, char salt[BCRYPT_HASHSIZE])
{
	int fd;
	char input[RANDBYTES];
	int workf;
	char *aux;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
		return 1;

	if (try_read(fd, input, RANDBYTES) != 0) {
		if (try_close(fd) != 0)
			return 4;
		return 2;
	}

	if (try_close(fd) != 0)
		return 3;

	/* Generate salt. */
	workf = (factor < 4 || factor > 31)?12:factor;
	aux = crypt_gensalt_rn("$2a$", workf, input, RANDBYTES,
			       salt, BCRYPT_HASHSIZE);
	return (aux == NULL)?5:0;
}

int bcrypt_hashpw(const char *passwd, const char salt[BCRYPT_HASHSIZE], char hash[BCRYPT_HASHSIZE])
{
	char *aux;
	aux = crypt_rn(passwd, salt, hash, BCRYPT_HASHSIZE);
	return (aux == NULL)?1:0;
}

int bcrypt_checkpw(const char *passwd, const char hash[BCRYPT_HASHSIZE])
{
	int ret;
	char outhash[BCRYPT_HASHSIZE];

	ret = bcrypt_hashpw(passwd, hash, outhash);
	if (ret != 0)
		return -1;

	return timing_safe_strcmp(hash, outhash);
}

#ifdef TEST_BCRYPT
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) 
{
	char* pass = data;
	if (strlen(pass) == 0){
		return 0;
	}
	
	// BCRYPT_HASHSIZE == 64
	char salt[BCRYPT_HASHSIZE];
	char hash[BCRYPT_HASHSIZE];
	char hash_t[BCRYPT_HASHSIZE];
	int ret;
	int salt_len = 4;

	for (salt_len = 4; salt_len <= 16; salt_len += 4){
		ret = bcrypt_gensalt(salt_len, salt);
		assert(ret == 0);
		ret = bcrypt_hashpw(pass, salt, hash);
		assert(ret == 0);

		// change hash
		hash[0] += 1;
		ret = bcrypt_hashpw(pass, hash, hash_t);
		// can't crypt password
		assert(ret == 1);
		assert(strcmp(hash, hash_t) != 0);
		// error occured
		assert(bcrypt_checkpw(pass, hash) == -1);
		
		hash[0] -= 1;
		pass[0] += 1;
		ret = bcrypt_hashpw(pass, hash, hash_t);
		// can crypt password, but hash & hash_t mismatch
		assert(ret == 0);
		assert(strcmp(hash, hash_t) != 0);
		// password mismatch
	    assert(bcrypt_checkpw(pass, hash) > 0);
	    assert(bcrypt_checkpw(pass, hash_t) == 0);
		
		printf("check finish, salt_len = %d\n", salt_len);
	}
	return 0;
}
#endif