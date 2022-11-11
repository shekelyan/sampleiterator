# Errata / Corrections

Paper: http://proceedings.mlr.press/v130/shekelyan21a/shekelyan21a.pdf

* p.6, Code 1, l.10, "max(N-i,1)" instead of "N-i" to suppress ZeroDivisionError
* p.6, Code 1, l.27, "(n-s-1)" instead of "(n-s)"
* p.7, last sentence before 3.3, "n / (N-s-1)" instead of "n / (N-s)"
* (no noticeable impact on experiments in the paper)

Corrected: https://compsci.science/shekelyan/hiddenshuffle.pdf

# Implementations

* hiddenshuffle.cpp (C++)
* hiddenshuffle.py (python.org)
* hiddenshuffle.d (dlang.org)
* hiddenshuffle.jl (julialang.org)
