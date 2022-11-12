# (Sequential) Random Sampling Without Replacement

The Hidden Shuffle sampling method generates a random set of integers in increasing order for simple random sampling without replacement (where one uses the integers as indexing positions). The method is comparably fast as ordinary (unsorted) sampling methods in most standard libraries, but only uses O(1) space, O(k) time and O(k) random numbers to sample a random k-subsequence (random subset of size k with preserved order).

# Implementations

* hiddenshuffle.cpp ([C++](https://en.wikipedia.org/wiki/C%2B%2B))
* hiddenshuffle.py (https://python.org)
* hiddenshuffle.d (https://dlang.org)
* hiddenshuffle.jl (https://julialang.org)

# Errata / Corrections

The original manuscript (http://proceedings.mlr.press/v130/shekelyan21a/shekelyan21a.pdf) had an off-by-one error: 

* p.6, Code 1, l.10, "max(N-i,1)" instead of "N-i" to suppress ZeroDivisionError
* p.6, Code 1, l.27, "(n-s-1)" instead of "(n-s)"
* p.7, last sentence before 3.3, "n / (N-s-1)" instead of "n / (N-s)"
* (no noticeable impact on experiments in the paper)
* corrected: https://compsci.science/shekelyan/hiddenshuffle.pdf

Relevant [XKCD](https://xkcd.com/2248/):

![Relevant XKCD](https://imgs.xkcd.com/comics/new_years_eve.png)

