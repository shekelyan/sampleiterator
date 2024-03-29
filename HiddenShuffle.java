// Sequential sample iterator using the hidden shuffle method (Shekelyan & Cormode 2021) storing a handful values
public class HiddenShuffle implements java.lang.Iterable<Long>, java.util.Iterator<Long>{

	private java.util.Random rnd = new java.util.Random(); // pseudorandom number generator
	private long H = -1; // number of low-high swaps (follows poisson-binomial distribution)
	private long L = -1; // equal to number of sampled low-items < n beginning of step 3 (follows hypergeometric distribution)
	private long N = -1; // population size
	private long n = -1; // sample size
	private double a = -1; // current order statistic of H standard uniform variables
	private long current = -1; // current sampled item
	
	public HiddenShuffle(long universeSize, long sampleSize){
		N = universeSize; n = sampleSize;
		H = 0; long i = 0; // STEP 1
		if (N > n){
			H = n;
			while (i < n){
				final double q = 1-1.0*(N-n)/(N-i);
				i += (long) Math.floor(Math.log(rnd.nextDouble() )/Math.log(1-q));
				final double pi = 1-1.0*(N-n)/(N-i);
				if (i < n && (rnd.nextDouble() < (pi/q) )){
					H -= 1;
				}
				i += 1;
			}
		}
		L = n-H; a = 1.0; // (n-H) is the number of low-low swaps
	}
	@Override
	public boolean hasNext(){
		while (H > 0){ // STEP 2
			final long S_old = n+(long) Math.floor(a*(N-n));
			a *= Math.pow(rnd.nextDouble(), 1.0/H);
			final long S = n+((long) Math.floor(a*(N-n)));
			if (S < S_old){
				H -= 1;
				current = (N-1)-S; return true;
			} else {
				L += 1; H -= 1; // duplicate detected
			}
		}
		while (L > 0){ // STEP 3
			final double u = rnd.nextDouble();
			long s = 0; double F = L*1.0/n;
			while (F < u && s < (n-L)){
				F = 1.0-(1.0-L*1.0/(n-s-1))*(1.0-F);
				s += 1;
			}
			L -= 1; n = n-s-1;
			current = (N-1)-n; return true;
		}
		return false;
	}
	@Override
	public Long next(){
		return current;
	}
	@Override
	public java.util.Iterator<Long> iterator() {
		return this;
	}
	
	public static void main(String[] arsg){
	
		// Generate 10 unique random indices between 0 and 999 in ascending order
		for( final Long x : new HiddenShuffle(1000,10)){
		
			System.out.println(x);
		}
	}	
}
