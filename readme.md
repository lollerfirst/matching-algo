# Maximum Cardinality, Minimum Cost matching algorithm
Took some concepts from the Hungarian Algorithm and some which I came up with to create this. Computational complexity is still `O(N^3)` in the worst case, with `N` being the dimension of the square matrix representing the edges. It should be quicker because it scans diagonals to find independent zeros and it finds surefire assignments for which elements in the same row/column get cancelled out in the next iteration.
