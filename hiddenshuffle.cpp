#include <iostream> 
#include <random>
#include <assert.h>
#include <unordered_set>
#include <set>
#include <chrono>
#include <algorithm>

class RandomNumbers{

	std::seed_seq seed1;
	std::mt19937 generator;
	
	std::uniform_real_distribution<double> default_real;
	std::uniform_int_distribution<size_t> default_int;
	
public:
	
	inline RandomNumbers(const std::string seed, size_t i1 = 0, size_t i2 = 1, double r1 = 0.0, double r2 = 1.0) : seed1(seed.begin(), seed.end()), generator(seed1), default_real(r1,r2), default_int(i1, i2){
	
	}
	
	inline double randomReal(){
	
		return default_real(generator);
	}
	
	inline size_t randomInteger(){
	
		return default_int(generator);
	}
	
	inline size_t randomInteger(size_t a, size_t b){
	
		return std::uniform_int_distribution<>(a, b)(generator);
	}
};

namespace SampleIterator{

	class HiddenShuffle{ // sequential sample iterator using the hidden shuffle method (Shekelyan & Cormode 2021) storing a handful values

		RandomNumbers rnd; // pseudorandom number generator
		long H; // number of low-high swaps (follows poisson-binomial distribution)
		long L; // equal to number of sampled low-items < n beginning of step 3 (follows hypergeometric distribution)
		long N; // population size
		long n; // sample size
		double a; // current order statistic of H standard uniform variables
	
		long current; // current sampled item
	
	public:
		inline HiddenShuffle(const std::string seed, long universeSize, long sampleSize) : rnd(seed){
		
			N = universeSize;
			n = sampleSize;
		
			// STEP 1
		
			H = 0; long i = 0;
		
			if (N > n){
			
				H = n;
				while (i < n){
			
					const double q = 1-1.0*(N-n)/(N-i);
				
					i += (long) floor(log(rnd.randomReal() )/log(1-q));
			
					const double pi = 1-1.0*(N-n)/(N-i);
				
					if (i < n && (rnd.randomReal() < (pi/q) )){
						--H;
					}
					++i;
				}
			}
		
			L = n-H; // (n-H) is the number of low-low swaps
			a = 1.0;
			popFront();
		}
	
		long front() const{

			return current;
		}
	
		bool empty() const {
		
			return current == N;
		}
	
		void popFront(){
		
			// STEP 2
		
			while (H > 0){
		
				const long S_old = n+(long) floor(a*(N-n));
		
				a *= pow(rnd.randomReal(), 1.0/H);
			
				const long S = n+(long) floor(a*(N-n));
			
				if (S < S_old){
			
					current = (N-1)-S;
					--H;
					return;
				
				} else {
			
					++L; // duplicate detected
					--H;
				}
			}
		
			// STEP 3
		
			while (L > 0){
			
				const double u = rnd.randomReal();
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
		
			current = N; // termination condition (N will not be reported!)
		}
	};

	class Baseline{ // (sequential) sample iterator using 
	private:	
		long pos = 0;
	
	protected:
		std::vector<size_t> values; // prepares all sampled positions to iterate over them
	
	public:
		inline Baseline(const std::string seed, long N, long n){
		}
	
		long front() const{

			return values.at(pos)-1;
		}
	
		bool empty() const {
		
			return pos >= values.size();
		}
	
		void popFront(){
	
			++pos;
		}
	};
	
	class DuplicateRejection : public Baseline{
	public:
		inline DuplicateRejection(const std::string seed, long N, long n) : Baseline(seed, N, n){
			
			RandomNumbers rnd(seed, 1, N);
			std::unordered_set<size_t> S(n);
			
			 // Often called selection sampling
			while (S.size() < n)
				S.insert(rnd.randomInteger());
			
			values = std::vector<size_t>(S.begin(), S.end() );
		}
	};
	
	
	class SequentialDuplicateRejection : public DuplicateRejection{
	public:
		inline SequentialDuplicateRejection(const std::string seed, long N, long n) : DuplicateRejection(seed, N, n){
		
			std::sort(values.begin(), values.end() );	
		}
	};
	
	
	class Floyd : public Baseline{
	public:
		inline Floyd(const std::string seed, long N, long n) : Baseline(seed, N, n){
		
			std::unordered_set<size_t> S(n);
		
			size_t M = n;
			
			RandomNumbers rnd(seed, 1, N);
			
			 // Floyd's sample algorithm (Bentley & Floyd 1987)
			for (size_t J = N-M+1; J <= N; ++J){
		
				size_t T = rnd.randomInteger(0, J-1)+1;
			
				if (S.count(T) == 0)
					S.insert(T);
				else
					S.insert(J);	
			}
		
			values = std::vector<size_t>(S.begin(), S.end() );
		}
	};
	
	class SequentialFloyd : public Floyd{
	public:
		inline SequentialFloyd(const std::string seed, long N, long n) : Floyd(seed, N, n){
			
			std::sort(values.begin(), values.end() );
		}
	};
	
};

class Measurement{

	long min = -1;
	long max = -1;
	
	long sum = 0;
	long dummysum = 0;
	
	long num = 0;

	std::chrono::steady_clock::time_point begin;
	
	std::string name;
	
public:
	inline Measurement(std::string name) : name(name){
	
		begin = std::chrono::steady_clock::now();
	}
	
	inline void digest(long val){
	
		if (min == -1 || val < min)
			min = val;
			
		if (max == -1 || val > max)
			max = val;
			
		sum += val;
		dummysum += val % 10;
		
		++num;
	}

	inline void stop(){

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << name << " min " << min << " avg " << (sum*1.0/num) << " max " << max << " duration " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;	
	}
};

int main(int argc, char *argv[]){
	
	long N = 1000L*1000L*1000L;
	long n = 1000L*1000L;
	
	int imax = 3;
	int sel = -1;
	
	if (argc == 4){
	
		N = pow(10, atol(argv[1]));
		n = pow(10, atol(argv[2]));
		sel = atol(argv[3]);
		
		imax = 1;
	}
	
	
	for (int i = 0; i < imax; ++i){
	
		const std::string seed = "test"+std::to_string(i);
		
		std::cout << "==============================" << std::endl;
		std::cout << "N " << N << " n " << n << std::endl;
		std::cout << "------------------------------" << std::endl;
		
		for (int k = 0; k < 1; ++k){
		
			if (sel != -1 && sel != 0)
				break;
		
			Measurement t1("hiddenshuffle");

			for (SampleIterator::HiddenShuffle iterator(seed+"_"+std::to_string(k), N, n); !iterator.empty(); iterator.popFront() )
				t1.digest( iterator.front() );
				
			t1.stop();
		}
		
		
		
		for (int k = 0; k < 1; ++k){
		
			if (sel != -1 && sel != 1)
				break;
		
			Measurement t1("duplicate-rejection");

			for (SampleIterator::DuplicateRejection iterator(seed+"_"+std::to_string(k), N, n); !iterator.empty(); iterator.popFront() )
				t1.digest( iterator.front() );
				
			t1.stop();
		}
		
		for (int k = 0; k < 1; ++k){
		
			if (sel != -1 && sel != 2)
				break;
		
			Measurement t1("sorted-duplicate-rejection");

			for (SampleIterator::SequentialDuplicateRejection iterator(seed+"_"+std::to_string(k), N, n); !iterator.empty(); iterator.popFront() )
				t1.digest( iterator.front() );
				
			t1.stop();
		}
		
		for (int k = 0; k < 1; ++k){
		
			if (sel != -1 && sel != 3)
				break;
		
			Measurement t1("floyd");

			for (SampleIterator::Floyd iterator(seed+"_"+std::to_string(k), N, n); !iterator.empty(); iterator.popFront() )
				t1.digest( iterator.front() );
				
			t1.stop();
		}
		
		for (int k = 0; k < 1; ++k){
		
			if (sel != -1 && sel != 4)
				break;
		
			Measurement t1("sorted-floyd");

			for (SampleIterator::SequentialFloyd iterator(seed+"_"+std::to_string(k), N, n); !iterator.empty(); iterator.popFront() )
				t1.digest( iterator.front() );
				
			t1.stop();
		}
		
		std::cout << "------------------------------" << std::endl;
		
		n *= 10;
	}
	

	return 0;
	

	
}