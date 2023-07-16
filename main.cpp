#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <algorithm>

#ifndef VERTICES
#define VERTICES 7
#endif

#ifndef MAX_VALUE
#define MAX_VALUE 30
#endif

void fill_edges(int* edges)
{
    srand(time(nullptr));

    for (int i=0; i<VERTICES*VERTICES; ++i)
    {
        edges[i] = rand() % (MAX_VALUE+1);
    }
}

void reduce(int* edges, int N)
{
    for (int i=0; i<N; ++i)
    {
    	//if (row_flags[i] % 2 == 0)
    	//{
		int min = INT32_MAX;

		for (int j=0; j<N; ++j)
		{
		    //if (col_flags[j] % 2 == 0)
		    	min = std::min(min, edges[i*N+j]);
		}

		for (int j=0; j<N; ++j)
		{
		    //if (col_flags[j] % 2 == 0)
		    	edges[i*N+j] -= min;
		}
	//}
    }

    for (int j=0; j<N; ++j)
    {
    	//if (col_flags[j] % 2 == 0)
    	//{
		int min = INT32_MAX;

		for (int i=0; i<N; ++i)
		{
		    //if (row_flags[i] % 2 == 0)
		    	min = std::min(min, edges[i*N+j]);
		}

		for (int i=0; i<N; ++i)
		{
		    //if (row_flags[i] % 2 == 0)
		    	edges[i*N+j] -= min;
		}
        //}
    }
}

std::ostream& operator<<(std::ostream& ss, const std::pair<int, int>& pair)
{
    ss << "(" << pair.first <<  ", " << pair.second << ")";
    return ss;
}

template <typename T, std::size_t DIM>
std::ostream& operator<<(std::ostream& ss, std::array<T, DIM> arr)
{
    for (const auto& el : arr)
    {
        ss << el << "  ";
    }

    return ss;
}

template <typename T>
void print_matrix(std::ostream& ss, const T* mat)
{
	for (int i=0; i<VERTICES; ++i)
	{
		for (int j=0; j<VERTICES; ++j)
		{
			ss << mat[i*VERTICES+j] << "  ";
		}
		
		ss << '\n';
	}
}

std::array<std::pair<int, int>, VERTICES> find_best_matching(const int* edges, int N)
{
    bool f = true;
    
    std::array<std::pair<int, int>, VERTICES> res1;
    std::array<std::pair<int, int>, VERTICES> res2;
    
    // Initialize base solutions
    for (int i=0; i<N; ++i)
    {
    	res1[i] = {i, i};
    	res2[i] = {i, N-i-1};
    }


    std::cout << "Processing Solution 1.\n\n";
    // Solution 1	
    while (f)
    {
	    f = false;
	    
	    std::cout << "Current solution: " << res1 << "\n";
	    
	    for (int i=0; i<N; ++i)
	    {
	    	std::pair<int, int> el1 = res1[i];
	    	std::pair<int, int> el2 = res1[(i+1)%N];
	    	
	    	int sum1 = edges[el1.first * N + el1.second] + edges[el2.first * N + el2.second];
	    	int sum2 = edges[el1.first * N + el2.second] + edges[el2.first * N + el1.second];
	    	
	    	// change res
	    	if (sum2 < sum1)
	    	{
	    		res1[i] = {el1.first, el2.second};
	    		res1[(i+1)%N] = {el2.first, el1.second};
	    		
	    		f = true;
	    	}
	    }
    }
    

    std::cout << "\nProcessing Solution 2.\n\n";
    
    f = true;
    while (f)
    {
    	f = false;
    	
    	std::cout << "Current solution: " << res2 << "\n";
    	
    	for (int i=0; i<N; ++i)
	    {
	    	std::pair<int, int> el1 = res2[i];
	    	std::pair<int, int> el2 = res2[(i+1)%N];
	    	
	    	int sum1 = edges[el1.first * N + el1.second] + edges[el2.first * N + el2.second];
	    	int sum2 = edges[el1.first * N + el2.second] + edges[el2.first * N + el1.second];
	    	
	    	// change res
	    	if (sum2 < sum1)
	    	{
	    		res2[i] = {el1.first, el2.second};
	    		res2[(i+1)%N] = {el2.first, el1.second};
	    		
	    		f = true;
	    	}
	    }
	    
    }
    

    std::cout << "\n";
     
    int cost1 = 0, cost2 = 0;
      
    for (int i=0; i<N; ++i)
    {
    	cost1 += edges[res1[i].first * N + res1[i].second];
    	cost2 += edges[res2[i].first * N + res2[i].second];
   	
    }
    
    std::cout << "Costs: sol1="<< cost1 << ", sol2=" << cost2 << "\n\n";
    int min = std::min({cost1, cost2});
    
if (min == cost1)
	return res1;
else
	return res2;
}

int main()
{
    int* edges = (int*) calloc(VERTICES*VERTICES, sizeof(int));
    
    /*
    // take edges as input
    std::cout << "Input Edges:\n";
    for (int i=0; i<VERTICES*VERTICES; ++i)
    {
    	std::cin >> edges[i];
    }
    */
    
    fill_edges(edges);
    
    std::cout << "Edges matrix:\n";
    print_matrix(std::cout, edges);
    std::cout << "\n";
    
    auto arr = find_best_matching(edges, VERTICES);

    std::cout << "Min-Cost Perfect Matching:\n" << arr << "\n";
    
    int total_cost = 0;
    
    std::for_each(arr.begin(), arr.end(), [&total_cost, edges](auto i){total_cost += edges[i.first*VERTICES+i.second];});
    
    std::cout << "Total cost = " << total_cost << "\n";

    free(edges);

    return 0;
}
