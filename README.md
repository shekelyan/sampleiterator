# (Sequential) Random Sampling Without Replacement

The Hidden Shuffle sampling method generates a random set of integers in increasing order for simple random sampling without replacement (where one uses the integers as indexing positions). The method is comparably fast as ordinary (unsorted) sampling methods in most standard libraries, but only uses *O(1)* space, *O(n)* time and *O(n)* random numbers to sample a random *n*-subsequence (random subset of size *n* with preserved order) from a sequence of size *N* (the "population" size *N* has to be known ahead of time).

# Implementations

* hiddenshuffle.c ([C](https://en.wikipedia.org/wiki/C_(programming_language)))
* hiddenshuffle.cpp ([C++](https://en.wikipedia.org/wiki/C%2B%2B))
* hiddenshuffle.cs ([C#](https://learn.microsoft.com/en-us/dotnet/csharp))
* hiddenshuffle.html ([JavaScript](https://en.wikipedia.org/wiki/JavaScript))
* HiddenShuffle.java (https://www.java.com)
* hiddenshuffle.py (https://python.org)
* hiddenshuffle.d (https://dlang.org)
* hiddenshuffle.jl (https://julialang.org)
* hiddenshuffle.rs (https://www.rust-lang.org)
* hiddenshuffle.go (https://go.dev)

# Basic Idea

Overall:

- the task is selecting a random subset of *n* positions out of *N* positions
  - split all *N* positions into *n* "hot" positions and *N-n* "cold" positions
  - bring all integers between *0* and *N-1* into a random order with a shuffling algorithm
  - after shuffling: the *n* hot positions hold a random subset of size *n*
  - "hidden shuffle" is a fast simulation of this (hidden) shuffling and selecting integers at hot positions
- variation of [swap-based shuffling algorithm](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle) can be chosen such that:
  - 'cold<->cold' swaps are executed last
  - 'cold<->hot' swaps guarantee that switched integer from cold position will stay in hot position (making 'cold->hot->cold' impossible)
  - ('hot<->hot' swaps do not really matter in this context)
- internally operate in descending order and then mirror positions to switch it to ascending order
  - more of a quirk of current algorithm than anything deeply related to the method

Note that the integers throughout the simulated/hidden shuffling algorithm cannot take the path of positions 'cold->hot->cold' and only the following scenarios are possible:

- 'cold->hot' (integer from originally cold position gets selected)
- 'cold->cold' (integer from originally cold position does not get selected)
- 'hot->...->hot' (integer from originally hot position gets selected)
- 'hot->...->cold' (integer from originally hot position does not get selected)

Step 1:
  
- determine the number *H* that counts the random number of cold<->hot swaps by generating a random number that follows the appropriate [distribution](https://en.wikipedia.org/wiki/Poisson_binomial_distribution)
- that means that exactly *n-H* integers never enter a cold position in this shuffle simulation 

Step 2:

- determine the random *H* cold positions involved in 'cold<->hot' swaps by:
  - using [order statistics](https://en.wikipedia.org/wiki/Order_statistic#Order_statistics_sampled_from_a_uniform_distribution) to generate *H* independent random numbers between *0* and *1* in descending order
  - scaling (and rounding) those *0* to *1* values to random cold positions in descending order
    - (those random cold positions correspond to integers taking the path 'cold->hot')
  - counting repeated cold positions towards integers taking the path 'hot->cold->...->hot' (increasing the *L* number)

Step 3:

- the number *L* then counts the number of integers either going 'hot->hot' (counted in step 1) or 'hot->cold->...->hot' (counted in step 2)
- select random subset of size *L* from the original hot positions
  - using any method that selects *L* random positions out of the *n* hot positions
    - the current algorithm uses Vitter's algorithm A from the [literature](https://dl.acm.org/doi/pdf/10.1145/23002.23003)

A more detailled explanation has been presented at the 24th International Conference on
Artificial Intelligence and Statistics (AISTATS 2021) and links to the full paper can be found below.
In the paper initially low-items are in the 'hot' positions and initially high-items are in the 'cold' positions.

# Errata / Corrections

The original manuscript (http://proceedings.mlr.press/v130/shekelyan21a/shekelyan21a.pdf) had an off-by-one error in the implementation of Vitter's algorithm A for step 3: 

* p.6, Code 1, l.10, "max(N-i,1)" instead of "N-i" to suppress ZeroDivisionError
* p.6, Code 1, l.27, "(n-s-1)" instead of "(n-s)"
* p.7, last sentence before 3.3, "n / (N-s-1)" instead of "n / (N-s)"
* (no noticeable impact on experiments in the paper)
* corrected: https://compsci.science/shekelyan/hiddenshuffle.pdf

Relevant [XKCD](https://xkcd.com/2248/):

![Relevant XKCD](https://imgs.xkcd.com/comics/new_years_eve.png)

