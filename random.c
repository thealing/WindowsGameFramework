#include "random.h"

#include "platform.h"

static HCRYPTPROV s_provider;

void random_init()
{
	CryptAcquireContext(&s_provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
}

int random()
{
	int value;

	CryptGenRandom(s_provider, sizeof(int), &value);

	value &= RANDOM_MAX;

	return value;
}

int random_below(int x)
{
	return random() % x;
}

int random_in_range(int l, int h)
{
	return l + random_below(h - l + 1);
}

double random_fraction()
{
	return (double)random() / RANDOM_MAX;
}

double random_fraction_below(double x)
{
	return random_fraction() * x;
}

double random_fraction_in_range(double l, double h)
{
	return l + random_fraction_below(h - l);
}
