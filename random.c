#include "random.h"

#include "platform.h"

int random()
{
	static HCRYPTPROV provider;
	
	if (provider == NULL)
	{
		CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	}
	
	int value;

	CryptGenRandom(provider, sizeof(int), &value);

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
