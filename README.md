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

Basic Intuition:

- If you shuffle a deck of *N* cards and take the top *n* cards, you get a random subset of *n* cards.
- Shuffling a deck of *N* cards with conventional algorithms unfortunately takes *O(N)* operations and requires *O(N)* memory.
- What if we could do the shuffling and taking the top *n* cards in *O(n)* operations requiring *O(1)* memory by avoiding the explicit shuffling?
- That's what the HiddenShuffle algorithm does!
- The [Fisher-Yates/Knuth shuffle](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle) algorithm works by:
  - going through the cards from top to bottom (the very top being position *0* and the very bottom being position *N-1*)
  - for each position *i* (between *0* and *N-1*): swap the card at position *i* with a card at a random position between *i* and *N-1*
- Let's define "hot" positions as the *n* positions between *0* and *n-1* at the of the top of the deck.
- Let's define "cold" positions as the *N-n* positions between *n* and *N-1* at the bottom of the deck.
- The shuffling algorithm will initially do either
  - hot<->hot swaps (swapping the cards at two hot positions)
  - hot<->cold swaps (swapping the cards at a hot and a cold position).
- The shuffling algorithm at the end does only:
  - cold<->cold swaps (swapping the cards at two cold positions)
- As we go from lower positions to higher positions:
  - after we visit position *i* we never revisit or swap with positions smaller than *i*
  - that means once we swap a card into position *i*, it will stay there!
  - that also means once a card is swapped from a cold into a hot position, that card will stay in that hot position and be selected for our random subset!
- For sake of convenience the HiddenShuffle goes from bottom (position *N-1*) to the top (position *0*):
  - random positions in descending order are simply mirrored to get random positions in ascending order

Each card can therefore only take one of the following paths through the shuffling:

- 'hot->hot' (card from originally hot position gets selected without ever entering a cold position)
- 'cold->hot' (card from originally cold position gets selected)
- 'cold->cold' (card from originally cold position does not get selected)
- 'hot->cold->hot' (card from originally hot position gets selected after entering a cold position)
- 'hot->cold' (card from originally hot position does not get selected)

Step 1 of the Hidden Shuffle Algorithm counts the number of hot<->cold swaps (sampling one number from poisson-binomial distribution):

- exactly *H* cards go 'cold->hot' or 'hot->cold->hot'
  - *H* can be determined by generating a random number that follows the appropriate [distribution](https://en.wikipedia.org/wiki/Poisson_binomial_distribution)
- exactly *n-H* cards go 'hot->hot' (they never enter a cold position)

Step 2 of the Hidden Shuffle Algorithm determines which cold positions are involved in hot<->cold swaps (sampling *H* cold positions with-replacement):

- there are *H* random cold positions in hot<->cold swaps, as they are all equally likely we can take a with-replacement sample of size *H* from the cold positions:
  - generate *H* independent random numbers between *0* and *1* in descending order using [order statistics](https://en.wikipedia.org/wiki/Order_statistic#Order_statistics_sampled_from_a_uniform_distribution)
  - scaling (and rounding) those *0* to *1* values to random cold positions in descending order
    - (those random cold positions correspond to cards taking the path 'cold->hot')
  - counting repeated cold positions towards cards taking the path 'hot->cold->hot' (increasing the *L* number)
  - (for involved cold position *c* the first hot<->cold swap means that *c* takes 'cold->hot' path, but additional hot<->cold swaps mean that the current initially hot card at *c* takes a 'hot->cold->hot' path through cold position *c* as it is being swapped back into a hot position)

Step 3 of the Hidden Shuffle Algorithm determines which hot positions end up again in a hot position (sampling *L* hot positions without-replacement):

- (exploits that any hot position is equally likely such that one just need to pick random subset of hot positions to fill up sample)
- the number *L* then counts the number of cards either going 'hot->hot' (counted in step 1) or 'hot->cold->hot' (counted in step 2)
- select random subset of size *L* from the original hot positions
  - using any method that selects *L* random positions out of the *n* hot positions
    - the current algorithm uses Vitter's algorithm A from the [literature](https://dl.acm.org/doi/pdf/10.1145/23002.23003)
    - the not-selected hot positions all went 'hot->cold' without returning to a hot position

A more detailled explanation has been presented at the 24th International Conference on
Artificial Intelligence and Statistics (AISTATS 2021) and links to the full paper can be found below.
In the paper low-items are cards that begin in 'hot' positions and high-items are cards that begin in 'cold' positions.

# Errata / Corrections

The original manuscript (http://proceedings.mlr.press/v130/shekelyan21a/shekelyan21a.pdf) had an off-by-one error in the implementation of Vitter's algorithm A for step 3: 

* p.6, Code 1, l.10, "max(N-i,1)" instead of "N-i" to suppress ZeroDivisionError
* p.6, Code 1, l.27, "(n-s-1)" instead of "(n-s)"
* p.7, last sentence before 3.3, "n / (N-s-1)" instead of "n / (N-s)"
* (no noticeable impact on experiments in the paper)
* corrected: https://compsci.science/shekelyan/hiddenshuffle.pdf

Relevant [XKCD](https://xkcd.com/2248/):

![Relevant XKCD](https://imgs.xkcd.com/comics/new_years_eve.png)

