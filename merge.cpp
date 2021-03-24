#include <bits/stdc++.h>
#include <chrono>
#include <omp.h>
using namespace std;
using namespace std::chrono;
const int _merge_base = 1000;
const int _binary_search_base = 16;
high_resolution_clock::time_point stclk, endclk;
int n, *In, *Out;


static double elapse_seconds(high_resolution_clock::time_point st, high_resolution_clock::time_point end) {
	duration<double,std::ratio<1,1>> duration_s(end-st);
	return duration_s.count();
}


template <class F>
void insertion_sort(int *A, int st, int n, const F& f) {
  for (int i = st; i < st+n; i++) {
    int v = A[i], pos = i-1;
    while(pos >= st && f(v, A[pos])) {
        A[pos+1] = A[pos];
        pos--;
    }
    A[pos+1] = v;
  }
}

template <class F>
void seq_merge(int *A, int st1, int ed1, int *B, int st2, int ed2, int *R, const F &f) {
    int i = st1, j = st2, nR = st1;
    while(i < ed1 && j < ed2) {
        if(f(B[j], A[i])) 
            R[nR++] = B[j++];
        else
            R[nR++] = A[i++];
    }
    while(i < ed1) 
        R[nR++] = A[i++];
    while(j < ed2)
        R[nR++] = B[j++];
}

template <class F>
size_t linear_search(int *I, int st, int ed, int v, const F& less) {
  for (int i = st; i < ed; i++)
    if (!less(I[i], v)) return i;
  return ed;
}

// return index to first key greater or equal to v
template <class F>
size_t binary_search(int *I, int st, int ed, int v, const F& less) {
    while(ed-st > _binary_search_base) {
        int mid = st+(ed-st)/2;
        if(!less(I[mid], v))
            ed = mid;
        else
            st = mid+1;
    }
    return linear_search(I, st, ed, v, less);
}

template <class F>
void merge_(int *A, int st1, int ed1, int *B, int st2, int ed2, int *R, const F &f) {
    int nA = ed1-st1, nB = ed2-st2;
    int nR = nA+nB;
    if(nR < _merge_base)
        seq_merge(A, st1, ed1, B, st2, ed2, R, f);
    else if(nA == 0) {
        #pragma omp parallel for 
            for(int i = st2; i < ed2; i++) 
                R[i] = B[i];
    }
    else if(nB == 0) {
        #pragma omp parallel for 
            for(int i = st1; i < ed1; i++)
                R[i] = A[i];
    }
    else {
        int mA = nA/2;
        // important for stability that binary search identifies
        // first element in B greater or equal to A[mA]

        int mB = binary_search(B, st2, ed2, A[mA], f)-st2;
        if(mB == 0) mA++;   // ensures at least one on each side
        int mR = mA+mB;
        #pragma omp parallel 
        {
            merge_(A, st1, st1+mA, B, st2, st2+mB, R, f);
            merge_(A, st1+mA, st1+nA, B, st2+mB, st2+nB, R, f);
        }
    }
}

// if inplace is true then the output is placed in In and Out is just used
// as temp space.
template <class F>
void merge_sort_(int *In, int st, int ed, int *Out, const F& f, bool inplace = false) {
    int n = ed-st;
    if(n/2 < 24) {
        insertion_sort(In, st, n, f);
        //if(!inplace)  {
            for(int i = st; i < st+n; i++)
                Out[i] = In[i];
        //}
        return ;
    }
    int m = n/2;

    if(n > 64) {
        #pragma omp parallel  
        {
            merge_sort_(In, st, st+m, Out, f, !inplace);
            merge_sort_(In, st+m, st+n, Out, f, !inplace);
        }    
    }
    else {
        merge_sort_(In, st, st+m, Out, f, !inplace);
        merge_sort_(In, st+m, st+n, Out, f, !inplace);
    }

    printf("%d %d %d\n", st, st+m, st+n);
    printf("In: ");
    for(int i = st; i < st+m; i++)
        printf("%d ", In[i]);
    printf("\n");
    printf("Out: ");
    for(int i = st; i < st+m; i++)
        printf("%d ", Out[i]);
    printf("\n");
    printf("In: ");
    for(int i = st+m; i < st+n; i++)
        printf("%d ", In[i]);
    printf("\n");
    printf("Out: ");
    for(int i = st+m; i < st+n; i++)
        printf("%d ", Out[i]);
    printf("\n");
    
    if(inplace)
        merge_(Out, st, st+m, Out, st+m, st+n, In, f);
    else
        merge_(In, st, st+m, In, st+m, st+n, Out, f);
    
    printf("inplace: %d\n", inplace);
    printf("In: ");
    for(int i = st; i < st+n; i++)
        printf("%d ", In[i]);
    printf("\n");
    printf("%d %d\n", st, ed);
    printf("Out: ");
    for(int i = st; i < st+n; i++)
        printf("%d %d ", i, Out[i]);
    printf("\n\n");
}

void output() {
    for(int i = 0; i < n; i++)
        printf("%d\n", In[i]);
    printf("Running Time: %.2f\n", elapse_seconds(stclk, endclk));
}

int main(int argc, char** argv) {
    string filename, outFile;
    filename = argv[1];
    outFile = argv[2];
    freopen(filename.c_str(), "r", stdin);
    freopen(outFile.c_str(), "w", stdout);
    scanf("%d", &n);
    In = new int[n+1];
    Out = new int[n+1];
    for(int i = 0; i < n; i++)
        scanf("%d", &In[i]);

    stclk = high_resolution_clock::now();
    //omp_set_num_threads(1); 
    merge_sort_(In, 0, n, Out, std::less<int>(), true);
    endclk = high_resolution_clock::now();
    output();
}