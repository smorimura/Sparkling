/*
 * private.c
 * Sparkling, a lightweight C-style scripting language
 *
 * Created by Árpád Goretity on 14/09/2013
 * Licensed under the 2-clause BSD License
 *
 * Private parts of the Sparkling API
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "private.h"

int nth_arg_idx(spn_uword *ip, int idx)
{
	int wordidx = idx / SPN_WORD_OCTETS;
	int shift = 8 * (idx % SPN_WORD_OCTETS);

	spn_uword word = ip[wordidx];
	int regidx = (word >> shift) & 0xff;

	return regidx;
}

void *spn_malloc(size_t n)
{
	void *ptr = malloc(n);

	if (ptr == NULL && n > 0) {
		unsigned long uln = n;
		spn_die("memory allocation of %lu bytes failed", uln);
	}

	return ptr;
}

void *spn_realloc(void *ptr, size_t n)
{
	void *ret = realloc(ptr, n);

	if (ret == NULL && n > 0) {
		unsigned long uln = n;
		spn_die("reallocation of pointer %p to size %lu failed", ptr, uln);
	}

	return ret;
}

void *spn_calloc(size_t nelem, size_t elsize)
{
	void *ptr = calloc(nelem, elsize);

	if (ptr == NULL && nelem * elsize > 0) {
		unsigned long uln = nelem * elsize;
		spn_die("allocation of %lu zero-init'ed bytes failed", uln);
	}

	return ptr;
}

void spn_die(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	spn_diev(fmt, args);
	va_end(args);
}

void spn_diev(const char *fmt, va_list args)
{
	fprintf(stderr, "fatal error in Sparkling: ");
	vfprintf(stderr, fmt, args);
	fflush(stderr);
	abort();
}

/* implementation of the symbol stub class */

static int symstub_equal(void *lhs, void *rhs)
{
	SymbolStub *ls = lhs, *rs = rhs;
	return strcmp(ls->name, rs->name) == 0;
}

static unsigned long symstub_hash(void *obj)
{
	/* I couldn't be bothered to implement caching */
	SymbolStub *symstub = obj;
	const char *name = symstub->name;
	size_t length = strlen(name);
	return spn_hash_bytes(name, length);
}

static const SpnClass SymbolStub_class = {
	sizeof(SymbolStub),
	symstub_equal,
	NULL,
	symstub_hash,
	NULL
};


/* constructor */
SpnValue make_symstub(const char *name)
{
	SymbolStub *obj = spn_object_new(&SymbolStub_class);
	obj->name = name;
	return makestrguserinfo(obj);
}

int is_symstub(const SpnValue *val)
{
	if (isstrguserinfo(val)) {
		SpnObject *obj = objvalue(val);
		return obj->isa == &SymbolStub_class;
	}

	return 0;
}
