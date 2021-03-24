#include <bits/stdc++.h>
#include <chrono>
#include <cilk/cilk.h>
#include "cilk/cilk_stub.h"
#include <omp.h>
using namespace std;
using namespace std::chrono;
int n, m, *pre;
vector<vector<int>> edge, connected; 
high_resolution_clock::time_point stclk, endclk;

static double elapse_seconds(high_resolution_clock::time_point st, high_resolution_clock::time_point end) {
	duration<double,std::ratio<1,1>> duration_s(end-st);
	return duration_s.count();
}

bool compare_and_swap(int *mem, int oldVal, int newVal) {
	if (*mem == oldVal) {
		*mem = newVal;
		return true;
	}
	return false;
}

int Find(int i) {
    int j = i;
    if(pre[j] == j)
        return j;
    do {
        j = pre[j];
    } while(pre[j] != j);

    int tmp;
    while((tmp = pre[i]) > j) {
        pre[i] = j;
        i = tmp;
    }
    return j;
}

void join(int u_orig, int v_orig) {
    int u = u_orig, v = v_orig;
    while(1) {
        u = Find(u);
        v = Find(v);
        if(u == v)
            break;
        else if(u > v && pre[u] == u && compare_and_swap(&pre[u], u, v)) {
            return ;
        }
        else if(v > u && pre[v] == v && compare_and_swap(&pre[v], v, u)) {
            return ;
        }
    }
}

void solve() {
    pre = new int[n];

    //omp_set_num_threads(1); 
#pragma omp parallel for
    for(int i = 0; i < n; i++) 
        pre[i] = i;

#pragma omp parallel for
    for(int i = 0; i < m; i++) 
        join(edge[i][0], edge[i][1]);

#pragma omp parallel for
    for(int i = 0; i < n; i++) {
        pre[i] = Find(i);
        //printf("%d %d\n", i, pre[i]);
    }
}

void output() {
    connected.resize(n);
    for(int i = 0; i < n; i++) 
        connected[pre[i]].push_back(i);

    int cnt = 0;
    for(int i = 0; i < n; i++) {
        if(connected[i].size()) {
            for(auto x : connected[i])
                cout << x << " ";
            cout << endl;
            cnt++;
        }
    }
    printf("%d\n", cnt);
    printf("Running Time: %.2f\n", elapse_seconds(stclk, endclk));
}

void readGraph() {
    scanf("%d%d", &n, &m);
    for(int i = 0; i < m; i++) {
        int x, y;
        scanf("%d%d", &x, &y);
        edge.push_back({x, y});
    }
}

int main(int argc, char** argv) {
    string filename;
    filename = argv[1];
    freopen(filename.c_str(), "r", stdin);
    readGraph();
    stclk = high_resolution_clock::now();
    solve();
    endclk = high_resolution_clock::now();
    output();
}