#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
const int _merge_base = 1000;
const int _binary_search_base = 16;

template <typename seq, typename F>
void insertion_sort(seq* A, int st, int n, const F& f) {
  for (int i = st; i < st+n; i++) {
    seq v = A[i];
    int pos = i-1;
    while(pos >= st && f(v, A[pos])) {
        A[pos+1] = A[pos];
        pos--;
    }
    A[pos+1] = v;
  }
}

template <typename seq, typename F>
void seq_merge(seq* A, int st1, int ed1, seq* B, int st2, int ed2, seq* R, int st3, const F &f) {
    int i = st1, j = st2, nR = st3;
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

template <typename seq, typename F>
size_t linear_search(seq* I, int st, int ed, seq v, const F& f) {
  for (int i = st; i < ed; i++)
    if (!f(I[i], v)) return i;
  return ed;
}

// return index to first key greater or equal to v
template <typename seq, typename F>
size_t binary_search(seq* I, int st, int ed, seq v, const F& f) {
    while(ed-st > _binary_search_base) {
        int mid = st+(ed-st)/2;
        if(!f(I[mid], v))
            ed = mid;
        else
            st = mid+1;
    }
    return linear_search(I, st, ed, v, f);
}

template <typename seq, typename F>
void merge_(seq* A, int st1, int ed1, seq* B, int st2, int ed2, seq* R, int st3, int ed3, const F &f) {
    int nA = ed1-st1, nB = ed2-st2;
    int nR = nA+nB;
    if(nR < _merge_base)
        seq_merge(A, st1, ed1, B, st2, ed2, R, st3, f);
    else if(nA == 0) {
        #pragma omp parallel for 
            for(int i = st2; i < ed2; i++) 
                R[st3+i-st2] = B[i];
    }
    else if(nB == 0) {
        #pragma omp parallel for 
            for(int i = st1; i < ed1; i++)
                R[st3+i-st1] = A[i];
    }
    else {
        int mA = nA/2;
        // important for stability that binary search identifies
        // first element in B greater or equal to A[mA]

        int mB = binary_search(B, st2, ed2, A[st1+mA], f)-st2;
        if(mB == 0) mA++;   // ensures at least one on each side
        int mR = mA+mB;
        //printf("%d  %d  %d\n", st1+mA, st2+mB, st3+mR);
        #pragma omp parallel sections  
        {
            #pragma omp section
            merge_(A, st1, st1+mA, B, st2, st2+mB, R, st3, st3+mR, f);
            #pragma omp section
            merge_(A, st1+mA, st1+nA, B, st2+mB, st2+nB, R, st3+mR, ed3, f);
        }
    }
}

// if inplace is true then the output is placed in In and Out is just used
// as temp space.
template <typename seq, typename F>
void merge_sort_(seq* In, int st, int ed, seq* Out, const F& f, bool inplace = false) {
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
        #pragma omp parallel sections 
        {
            #pragma omp section
            merge_sort_(In, st, st+m, Out, f, !inplace);
            #pragma omp section
            merge_sort_(In, st+m, st+n, Out, f, !inplace);
        }    
    }
    else {
        merge_sort_(In, st, st+m, Out, f, !inplace);
        merge_sort_(In, st+m, st+n, Out, f, !inplace);
    }
    
    if(inplace)
        merge_(Out, st, st+m, Out, st+m, st+n, In, st, st+n, f);
    else
        merge_(In, st, st+m, In, st+m, st+n, Out, st, st+n, f);
}


template <typename seq, typename F>
void merge_sort(seq* In, int st, int ed, const F& f) {  
    seq* Out = new seq[ed];
    
    omp_set_num_threads(8);
    merge_sort_(In, st, ed, Out, f, true);

    delete[] Out;
}
