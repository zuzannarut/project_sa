#include <string>
#include <iostream>
#include <vector>
#include "sa_radix.h"

using namespace std;

int main(int argc, char** argv) {
    // sample usage;
    vector<int> sequence = {1, 3, 2, 2, 1, 3, 4, 0};
    auto sa_arr = suffix_array(sequence);
    cout << "Suffix Array:\n";
    for (auto ind : sa_arr)
            cout << ind << " ";
    cout << '\n';

    auto lcp = lcp_construction(sequence, sa_arr);
    cout << "LCP Array:\n";
    for (auto ind : lcp)
        cout << ind << " ";
    cout << '\n';
    return 0;
}