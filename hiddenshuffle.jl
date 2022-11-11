# tested on Julia 1.8.2 (julialang.org)

#import Pkg; Pkg.add("StatsBase")
#(uncomment previous line if StatsBase is missing)

using StatsBase
import StatsBase: AbstractRNG, Random, randexp, sample!,seqsample_a!,seqsample_d!

using Random: Sampler, Random.GLOBAL_RNG

"""
   seqsample_hiddenshuffle!([rng], a::AbstractArray, x::AbstractArray)
Random subsequence sampling using Hidden Shuffle algorithm described in the following paper (page 3628-36):
Michael Shekelyan & Graham Cormode. "Sequential Random Sampling Revisited: Hidden Shuffle Method",
Proceedings of The 24th International Conference on Artificial Intelligence and Statistics, 
PMLR 130:3628-3636, 2021.
This algorithm consumes ``O(k)`` random numbers, with `k=length(x)`.
The outputs are ordered.
"""
function seqsample_hiddenshuffle!(rng::AbstractRNG, a::AbstractArray, x::AbstractArray, faster)
	1 == firstindex(a) == firstindex(x) ||
		throw(ArgumentError("non 1-based arrays are not supported"))
	Base.mightalias(a, x) &&
		throw(ArgumentError("output array x must not share memory with input array a"))
	N = length(a)
	n = length(x)
	n <= N || error("length(x) should not exceed length(a)")
	
	H = 0; i = 0; j = 1 # STEP 1: compute H
	if N > n
		H = n
		while i < n
			q = 1.0-float(N-n)/(N-i)
			i = i+trunc(Int,log(1-q, rand(rng)))
			p_i = 1.0-float(N-n)/(N-i)
			if i < n && rand(rng) < p_i/q
				H = H-1
			end
			i = i+1
		end
	end
	L = n-H; a_ = 1.0
	while H > 0 # STEP 2: draw high-items
		S_old = n+trunc(Int,a_*(N-n))
		a_ = a_ * rand(rng)^(1.0/H)
		S = n+trunc(Int, a_*(N-n))
		if S < S_old
			@inbounds x[j] = a[N-S]; j+=1
		else
			L = L+1
		end
		H = H-1
	end

	if faster && L > 0 && N > n # slightly faster with existing Alg A implementation
		@views seqsample_a!(rng,a[(N-n+1):N],x[(n-L+1):n]); L = 0
	end
	while L > 0 # STEP 3: draw low-items (with Vitter's Alg A)
		u = rand(rng); s=0; F=float(L)/n
		while F < u && s < (n-L)
			F = 1-(1-float(L)/(n-s-1))*(1-F)
			s = s+1
		end
		L = L-1; n = n-s-1
		@inbounds x[j] = a[N-n]; j+=1
	end
end

seqsample_hiddenshuffle!(rng::AbstractRNG, a::AbstractArray, x::AbstractArray) = seqsample_hiddenshuffle!(rng::AbstractRNG, a, x, true)
seqsample_hiddenshuffle!(a::AbstractArray, x::AbstractArray) = seqsample_hiddenshuffle!(Random.GLOBAL_RNG, a, x)

# TESTING

println("Random Subset Selection (Simple Random Sampling Without Replacement)")

sample_unordered = "sample (unordered) "
sample_ordered =   "sample             "
sample_algd =	   "sample (alg d)     "
hiddenshuffle =	   "hidden shuffle     "

println("\nLegend of Methods to Randomly Select n Unique integers Between 1 and N\n")

println(sample_unordered*" sample!(population,sample,ordered=false,replace=false)")
println(sample_ordered*" sample!(population,sample,ordered=true,replace=false)")
println(sample_algd*" seqsample_d!(population,sample)")
println(hiddenshuffle*" seqsample_hiddenshuffle!(population,sample)")

println("\nPrecompilation Time\n")

print(sample_ordered); @time precompile( sample!, (UnitRange{Int64}, Array{Int64, 1}))
print(sample_algd); @time precompile( seqsample_d!, (UnitRange{Int64}, Array{Int64, 1}))
print(hiddenshuffle); @time precompile( seqsample_hiddenshuffle!, (UnitRange{Int64}, Array{Int64, 1},Bool))

# SAMPLES

pop_pow = 3; sample_pow = 1

pop = (1:10^pop_pow); samplesize = 10^sample_pow; x = zeros(Int, samplesize)
print("\n[N = 10^"*string(pop_pow)*", n = 10^"*string(sample_pow)*"] ")
println("Sample (ordered sample unless indicated otherwise)\n")

print(sample_unordered); StatsBase.sample!(pop, x, ordered=false, replace=false  ); println(x)
print(sample_ordered); StatsBase.sample!(pop, x, ordered=true, replace=false  ); println(x)
print(sample_algd); StatsBase.seqsample_d!(pop, x); println(x)
print(hiddenshuffle); seqsample_hiddenshuffle!(pop, x); println(x)

# RUNTIMES

for pop_pow in (7:10)
	for sample_pow in (6:6)
		population = (1:10^pop_pow); sample = zeros(Int, 10^sample_pow)
		print("\n[N = 10^"*string(pop_pow)*", n = 10^")
		print(string(sample_pow)*"] Runtime (ordered sample unless indicated otherwise)\n")
		print(sample_unordered); @time StatsBase.sample!(population, sample, ordered=false, replace=false  )
		print(sample_ordered); @time StatsBase.sample!(population, sample, ordered=true, replace=false  )
		print(sample_algd); @time StatsBase.seqsample_d!(population, sample)
		print(hiddenshuffle); @time seqsample_hiddenshuffle!(population, sample)
		#print(hiddenshuffle); @time seqsample_hiddenshuffle!(Random.GLOBAL_RNG,population, sample, false)
	end
end
