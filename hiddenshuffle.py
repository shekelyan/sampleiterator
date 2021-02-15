from math import log,exp,floor
from random import uniform
def sampleiterator(N, n): # draw WOR from 0..N-1
	H = 0; i = 0 # STEP 1: compute H
	if N > n:
		H = n
		while i < n:
			q = 1.0-float(N-n)/(N-i)
			i = i+int(log(uniform(0,1), 1-q) )
			p_i = 1.0-float(N-n)/(N-i)
			if i < n and uniform(0,1) < p_i/q:
				H = H-1
			i = i+1
	L = n-H; a = 1.0
	while H > 0: # STEP 2: draw high-items
		S_old = n+int(a*(N-n) )
		a = a * uniform(0,1)**(1.0/H)
		S = n+int(a*(N-n) )
		if S < S_old:
			yield (N-1) - S
		else:
			L = L+1  # duplicate detected
		H = H-1
	while L > 0: # STEP 3: draw low-items
		u = uniform(0,1); s=0; F=float(L)/n
		while F < u and s < (n-L-1):
			F = 1-(1-float(L)/(n-s))*(1-F)
			s = s+1
		L = L-1; n = n-s-1
		yield (N-1) - n

# How to iterate over any random sample.
		
print("Sequential simple random sample (N = 10^9, n = 10000) using hidden shuffle [proposed]")

for x in sampleiterator(10**9, 10000):
	print(x)

import random, sys

# Note: the following commented-out code assumes Python 3.x, for Python 2.x "xrange" has to be used instead of "range"

#print("Non-sequential simple random sample (N = 10^9, n = 10000) using python's random.sample")

#for x in random.sample(range(10**9),10000):
#	print(x)

#print("Sequential simple random sample (N = 10^9, n = 10000) using python's random.sample")

#	for x in sorted(random.sample(range(10**9),10000)):
#		print(x)
