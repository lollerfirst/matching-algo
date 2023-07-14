#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <algorithm>

#ifndef VERTICES
#define VERTICES 7
#endif

enum assign_t
{
	UNASSIGNED,
	CERTAIN,
	COLLISION
};

void fill_edges(int* edges)
{
    srand(time(nullptr));

    for (int i=0; i<VERTICES*VERTICES; ++i)
    {
        edges[i] = rand() % 31;
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

int subtract_min(int* edges, int N, std::array<int, VERTICES>& zero_count, const std::array<assign_t, VERTICES>& row_flags, const std::array<assign_t, VERTICES>& col_flags)
{
	int min = INT32_MAX;
	
	//Retrieve min
	for (int i=0; i<N; ++i)
	{
		if (row_flags[i] != CERTAIN)
		{
			for (int j=0; j<N; ++j)
			{
				min = (col_flags[j] != CERTAIN && edges[i*N+j] > 0) ? std::min(min, edges[i*N+j]) : min;
			}
		}
	}
	
	//Update values and zero-count accordingly.
	for (int i=0; i<N; ++i)
	{
		if (row_flags[i] != CERTAIN)
		{
			for (int j=0; j<N; ++j)
			{
				if (col_flags[j] != CERTAIN)
				{
					if (edges[i*N+j] > 0)
						edges[i*N+j] -= min;
					
					// By updating the count here we
					// cut back on another search
					if (edges[i*N+j] == 0)
					{
						++zero_count[i];
					}
				}
			}
		}
	}
	
	return min;
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

void count_row_zeros(int* edges, std::array<int, VERTICES>& row_zeros, int N)
{
	for (int i=0; i<N; ++i)
	{
		row_zeros[i] = 0;
		for (int j=0; j<N; ++j)
		{
			if (edges[i*N+j] == 0)
				++row_zeros[i];
		}
	}
}

/* ** CORE FUNCTION OF THE ALGORITHM **

DESCRIPTION: the algorithm starts by subtracting the minima across rows and columns, just like the Hungarian Algorithm.
The algorithm keeps track of the status of each row and column that can be: UNASSIGNED, ASSIGNED WITH CERTAINTY
or ASSIGNED WITH COLLISION. The algorithm scans 2 times symmetric groups in search for zeros. When it finds a zero on a UNASSIGNED row,
it checks whether the corrisponding flag on the column is ASSIGNED or else. If it's assigned, it checks a table of pre-calculated number of zeros along each row and substitutes in the solution the edge with the greater associated number of zeros, modifying the flags along row and column to represent ASSIGNED WITH COLLISION; if it's UNASSIGNED, it simply includes the edge in the solution.

Upon ending the set of iterations that span 2 times the matrix, the algorithm checks whether the cardinality of the assignment/solution is sufficient, i.e. cardinality >= N, with N := dimension of the square matrix. If the cardinality of the assignment is sufficient, it STOPS, otherwise it proceeds after having calculated the minimum of all the elements that are NOT in any row or column flagged ASSIGNED WITH CERTAINTY and subtracts it from said elements, while also calculating a new number of zeros along each row that reflects the negated elements.

*/

std::array<std::pair<int, int>, VERTICES> find_max_zero_set(int* edges, int N)
{
    int card = 0;
    
    std::array<std::pair<int, int>, VERTICES> res;
    std::for_each(res.begin(), res.end(), [](auto& i){i.first = -1;});
    
    std::array<assign_t, VERTICES> row_flags;
    memset(row_flags.data(), 0, sizeof(int)*N);

    std::array<assign_t, VERTICES> col_flags;
    memset(col_flags.data(), 0, sizeof(int)*N);
    
    std::array<int, VERTICES> row_zeros;
    count_row_zeros(edges, row_zeros, N);
	
    while (true)
    {
	    
	    std::cout << "Row Zeros: " << row_zeros << "\n\n";

	    for (int l=0; l<2*N && card<N; ++l)
	    {
		int i = l % N;
		std::cout << "Processing Diag " << i << "\n";

		for (int k=0; k<N && card<N; ++k)
		{

		    int sel = (i+k) % N;

		    if (edges[sel*N+k] == 0 && !row_flags[sel])
		    {
		        if (!col_flags[k])
		        {
		            res[k] = {sel, k};
		            col_flags[k] = CERTAIN;
		            row_flags[sel] = CERTAIN;
		            ++card;
		        }
		        else
		        {

		            auto prev = res[k];
		            std::cout << "[!] Collision elements " << std::pair<int, int>({sel, k}) << " and " << prev << "\n";

		            
		            if (row_zeros[sel] < row_zeros[prev.first])
		            {
		            	res[k] = {sel, k};
		            	row_flags[prev.first] = UNASSIGNED; // Previous row becomes selectable
		            	row_flags[sel] = COLLISION; // Current row becomes unselectable
		            }
		            else
		            {
		            	row_flags[prev.first] = COLLISION;
		            }

			    col_flags[k] = COLLISION;
		            std::cout << "nzeros2 = " << row_zeros[sel] << ", nzeros1 = " <<  row_zeros[prev.first] << ". " << res[k] << " won the spot.\n";
		        }
		    }
		}

		std::cout <<  "Current Matching: " << res << "\n";
		std::cout << "Row Flags: " << row_flags << "\n";
		std::cout << "Col Flags: " << col_flags << "\n\n";
	    }
	    
	    if (card >= N)
	    	break;
	    	
	    memset(row_zeros.data(), 0, sizeof(int)*VERTICES);
	    
	    for (int i=0; i<N; ++i)
	    {
	    	if (row_flags[i] == COLLISION)
	    		row_flags[i] = UNASSIGNED;
	    		
	    	if (col_flags[i] == COLLISION)
	    	{
	    		col_flags[i] = UNASSIGNED;
	    		--card;
	    	}
	    }
	    
	    int min = subtract_min(edges, N, row_zeros, row_flags, col_flags);
            std::cout << "Min-Subtracted Edges matrix:\tmin=" << min <<"\n";
            print_matrix(std::cout, edges);
	    std::cout << "\n";
    }
    
    return res;
    
}

int main()
{
    int* edges = (int*) calloc(VERTICES*VERTICES, sizeof(int));
    int* edges_copy = (int*) calloc(VERTICES*VERTICES, sizeof(int));
    
    fill_edges(edges);
    memcpy(edges_copy, edges, VERTICES*VERTICES*sizeof(int));
    
    std::cout << "Edges matrix:\n";
    print_matrix(std::cout, edges);
    std::cout << "\n";
    
    reduce(edges_copy, VERTICES);
    std::cout << "Reduced Edges matrix:\n";
    print_matrix(std::cout, edges_copy);


    auto arr = find_max_zero_set(edges_copy, VERTICES);

    std::cout << "Min-Cost Perfect Matching:\n" << arr << "\n";
    
    int total_cost = 0;
    
    std::for_each(arr.begin(), arr.end(), [&total_cost, edges](auto i){total_cost += edges[i.first*VERTICES+i.second];});
    
    std::cout << "Total cost = " << total_cost << "\n";

    free(edges);
    free(edges_copy);
    return 0;
}
