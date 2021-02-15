#!/usr/bin/env dub
/+ dub.sdl:
	name "hiddenshuffle"
+/

import std.math : log, exp, floor, pow, ceil;
import std.random;
import std.stdio: writeln;

class HiddenShuffle{

	Random rnd;
	long H = -1;
	long L = -1;
	long N = -1;
	long n = -1;
	double a = -1;
	long current = -1;
	
	this(Random random, long universeSize, long sampleSize){
		
		rnd = random;
		N = universeSize;
		n = sampleSize;
		
		/+ step 1 +/
		
		H = 0; long i = 0;
		
		if (N > n){
			
			H = n;
			while (i < n){
			
				double q = 1-1.0*(N-n)/(N-i);
			
				i += cast(long) floor(log(uniform01(rnd))/log(1-q));
			
				double pi = 1-1.0*(N-n)/(N-i);
				
				if (i < n && (uniform01(rnd) < (pi/q) )){
					--H;
				}
				++i;
			}
		}
		
		L = n-H;
		a = 1.0;
		popFront();
	}
	
	long front() const @property{
	
		return current;
	}
	
	bool empty() const {	 
	
    	return current == -1; 
	}
	
	void popFront(){
		
		/+ step 2 +/
		
		while (H > 0){
		
			long S_old = n+cast(long) floor(a*(N-n));
		
			a *= exp( log(uniform01(rnd))/H);
			
			long S = n+cast(long) floor(a*(N-n));
			
			if (S < S_old){
			
				current = (N-1)-S;
				--H;
				return;
			} else {
			
				++L;
				--H;
			}
		}
		
		/+ step 3 +/
		
		while (L > 0){
			
			double u = uniform01(rnd);
			long s = 0;
			double F = L*1.0/n;
			
			while (F < u && s < (n-L-1)){
				
				F = 1.0-(1.0-L*1.0/(n-s))*(1.0-F);
				++s;
			}
			
			--L;
			n = n-s-1;
			
			current = (N-1)-n; 
			return;
		}
		
		current = -1;
		return;
	}
};

void main(){

	writeln("Sequential sample (N = 10^9, n = 10000) using hidden shuffle [proposed]");

	auto sampleIterator = new HiddenShuffle(Random(123L), pow(10,9), 10000);

	foreach(long element; seqsampleHiddenshuffle){			
		writeln(element);
	}
	
	/+
	import std.range : iota;
	writeln("Sequential sample (N = 10^9, n = 10000) using Algorithm D [Vitter 84,87]");
	
	auto sampleIteratorBaseline = pow(10,9).iota.randomSample( 10000, Random(123L));
				
	foreach(long element; seqsampleAlgorithmD){
		writeln(element);
	}
	+/
	
}