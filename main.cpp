#include <bits/stdc++.h>
#include "integerSort/blockRadixSort.h"
#include "mergeSort/mergeSort.h"
//#include "merge/merge_sort.h"
#include <chrono>
using namespace std::chrono;
high_resolution_clock::time_point stclk, endclk;
int *A, *B, n;
struct node {
    int x, y;
};
node *AA, *BB;

static double elapse_seconds(high_resolution_clock::time_point st, high_resolution_clock::time_point end) {
	duration<double,std::ratio<1,1>> duration_s(end-st);
	return duration_s.count();
}

bool cmpInt(int a, int b) {
    return a < b;
}

bool cmpNode(node a, node b) {
    return a.y < b.y;
}

void SortInt() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < n; j++) 
            B[j] = A[j];
        
        if(i == 0) {
            stclk = high_resolution_clock::now();
            sort(B, B+n);
            endclk = high_resolution_clock::now();
            printf("QuickSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }
        else if(i == 1) {
            stclk = high_resolution_clock::now();
            merge_sort(B, 0, n, cmpInt);
            endclk = high_resolution_clock::now();
            printf("MergeSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }
        else if(i == 2) {
            stclk = high_resolution_clock::now();
            integerSort(B, n);
            endclk = high_resolution_clock::now();
            printf("IntegerSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }

        // for(int j = 0; j < n; i++) 
        //     printf("%d ", B[i]);
        // printf("\n");
    }
}

void SortNode() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < n; j++) 
            BB[j] = AA[j];

        if(i == 0) {
            stclk = high_resolution_clock::now();
            sort(BB, BB+n, cmpNode);
            endclk = high_resolution_clock::now();
            printf("QuickSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }
        else if(i == 1) {
            stclk = high_resolution_clock::now();
            merge_sort(BB, 0, n, cmpNode);
            endclk = high_resolution_clock::now();
            printf("MergeSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }
        else if(i == 2) {
            stclk = high_resolution_clock::now();
            integerSort(BB, n, [](const node& a) { return a.y; });
            endclk = high_resolution_clock::now();
            printf("IntegerSort Time: %.2f\n", elapse_seconds(stclk, endclk));
        }

        // for(int i = 0; i < n; i++) 
        //     printf("%d %d\n", BB[i].y, BB[i].x);
    }
}


int main(int argc, char** argv) {
    string filename, outFile;
    filename = argv[1];
    outFile = argv[2];
    freopen(filename.c_str(), "r", stdin);
    freopen(outFile.c_str(), "w", stdout);
    scanf("%d", &n);
    A = new int[n];
    B = new int[n];
    AA = new node[n];
    BB = new node[n];

    for(int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
        AA[i].x = A[i];
        AA[i].y = (A[i]+A[i]*100)%100;
    }

    SortInt();
    printf("\n");
    SortNode();

    
}