#include <string>
#include <iostream>
#include <vector>
#include "sa_radix.h"

using namespace std;

int main(int argc, char** argv) {
    // sample usage;
    vector<int> sequence = {1, 3, 2, 2, 1, 3, 4, 0};
    auto sa_arr = suffix_array_smooth(sequence);
    cout << "Suffix Array:\n";
    for (auto ind : sa_arr)
            cout << ind << " ";
    cout << '\n';

    vector<int> query = {1, 3};
    auto answ = answ_query(sequence, sa_arr, query);
    for (int i = answ.first; i < answ.second; i++)
        cout << sa_arr[i] << " ";
    cout << "\n";
    return 0;
}