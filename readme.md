# Maximum Cardinality, Minimum Cost matching algorithm

Algorithm I came up with by thinking about localized (2x2) swaps between independent elements and how they might help finding a minimum cost matching. I haven't demonstrated its correctness but it should be significantly faster than the Hungarian which is O(N^3). It seems to work well. It HAS to calculate 2 solutions that start from the 2 main different diagonals:

*{(0,0), (1,1), (2,2), (3,3)}* and *{(0,3), (1,2), (2,1), (3,0)}.*
