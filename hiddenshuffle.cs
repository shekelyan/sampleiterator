using System; using System.Collections.Generic;

class HiddenShuffle {

	static public IEnumerable<long> SampleIterator(long N, long n){

		Random r = new Random();

		long H = 0; long i = 0; // STEP 1: compute H
		if (N > n){
			H = n;
			while (i < n){
				double q = 1.0-((double)(N-n))/(N-i);
				i = i+(long)(Math.Log(r.NextDouble())/Math.Log(1.0-q) );
				double p_i = 1.0-((double)(N-n))/Math.Max(N-i, 1.0);
				if (i < n && r.NextDouble() < p_i/q){
					H = H-1;
				}
				i = i+1;
			}
		}
		long L = n-H; double a = 1.0;

		while (H > 0){ // STEP 2: draw high-items
			long S_old = n+(long)(a*(N-n) );
			a = a * Math.Pow(r.NextDouble(),1.0/H);
			long S = n+(long)(a*(N-n) );
			if (S < S_old){
				yield return (N-1)-S;
			} else {
				L = L+1;  // duplicate detected
			}
			H = H-1;
		}
	
		while (L > 0){ // STEP 3: draw low-items
			double u = r.NextDouble(); long s=0; double F=((double)L)/n;
			while (F < u && s < (n-L)){
				F = 1.0-(1.0-((double)L)/(n-s-1))*(1.0-F);
				s = s+1;
			}
			L = L-1; n = n-s-1;
			yield return (N-1) - n;
		}
	}
	  
	static void Main(string[] args) {
		
		foreach(long x in SampleIterator(1000,10)){
		
			Console.WriteLine(x);
		}
	}
}
