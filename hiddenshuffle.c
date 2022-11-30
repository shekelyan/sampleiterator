#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct HiddenShuffle
{
long H,L,N,n; double a; // internal state
double (*randomdouble)(); // pointer to function generating random double between 0 and 1
};

// move to next sampled index
long hsnext(struct HiddenShuffle* ptr)
{

	while (ptr->H > 0)
	{
		
		const long S_old = ptr->n+(long) (ptr->a*(ptr->N-ptr->n));
		ptr->a *= pow( (*(ptr->randomdouble))(), 1.0/ptr->H);
		const long S = ptr->n+(long) floor(ptr->a*(ptr->N-ptr->n));
		if (S < S_old)
		{
			ptr->H -= 1;
			return (ptr->N-1)-S;
		
		} 
		else 
		{
			ptr->L += 1; ptr->H -= 1; // duplicate detected
		}
	}
	
	while (ptr->L > 0)
	{ // STEP 3
	
		const double u = (*(ptr->randomdouble))();
		long s = 0; double F = ptr->L*1.0/ptr->n;
		
		while (F < u && s < (ptr->n-ptr->L))
		{
		
			F = 1.0-(1.0-ptr->L*1.0/(ptr->n-s-1.0))*(1.0-F);
			s += 1;
		}
	
		--ptr->L; ptr->n = ptr->n-s-1;
		return (ptr->N-1)-ptr->n;
	}
	return -1;
}

// input: sampler pointer, population size and sample size
// output: none
long hsinit(struct HiddenShuffle* ptr, long universeSize, long sampleSize){

	const long N = universeSize;
	const long n = sampleSize;
	
	if ((n <= 0) || (N < n))
	{
		return -1;
	}
	
	long H = 0; long i = 0; // STEP 1
	if (N > n)
	{
		H = n;
		while (i < n)
		{
			const double q = 1.0-1.0*(N-n)/(N-i);
			i += (long) (log( (*(ptr->randomdouble))() )/log(1.0-q));
			const double pi = 1.0-1.0*(N-n)/(N-i);
			if (i < n && ( (*(ptr->randomdouble))() < (pi/q) ))
			{
				--H;
			}
			++i;
		}
	}
	ptr->N = N; ptr->n = n; ptr->L = n-H; ptr->a = 1.0; ptr->H = H;
	return hsnext(ptr);
}

double randomdouble()
{ // e.g., see http://robotics.stanford.edu/users/sahami/cs121/code/random.c
	
	return (double) rand() / ((double) RAND_MAX + 1);;
}

int main(void)
{
	
	struct HiddenShuffle hs; // allocate sampler state on stack
	
	srand( (int) time(NULL)); // use current time as random number seed
	hs.randomdouble = &randomdouble; // use some random number generator
	
	for(long x = hsinit(&hs, 1000, 10); x != -1; x = hsnext(&hs))
	{
		
		printf("%ld\n", x );
	}
}