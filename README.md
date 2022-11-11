# (Sequential) Random Sampling Without Replacement

The Hidden Shuffle method generates a random set of integers in increasing order for simple random sampling without replacement (using the integers as indexing positions). It only uses O(1) memory and is comparably fast as unsorted sampling methods (default in most standard libraries).

# Implementations

* hiddenshuffle.cpp (C++)
* hiddenshuffle.py (python.org)
* hiddenshuffle.d (dlang.org)
* hiddenshuffle.jl (julialang.org)

# Errata / Corrections

The original manuscript had a off-by-one error: http://proceedings.mlr.press/v130/shekelyan21a/shekelyan21a.pdf

* p.6, Code 1, l.10, "max(N-i,1)" instead of "N-i" to suppress ZeroDivisionError
* p.6, Code 1, l.27, "(n-s-1)" instead of "(n-s)"
* p.7, last sentence before 3.3, "n / (N-s-1)" instead of "n / (N-s)"
* (no noticeable impact on experiments in the paper)

Corrected: https://compsci.science/shekelyan/hiddenshuffle.pdf

Relevant XKCD:
![Relevant XKCD](https://imgs.xkcd.com/comics/new_years_eve.png)

