#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "../sa_radix.h"
#include <time.h>

using namespace std;

// This C++ code takes as an input a path to a file in FASTA format and executes
// the suffix array algorithm from sa_radix.h on the sequence from the file

int main(int argc, char** argv) {
    ifstream input_file(argv[1]);
    string current_line, genome;
    getline(input_file, current_line);
    while (getline(input_file, current_line))
        genome += current_line;
    genome += '$';

    vector<int> sequence(genome.size());
    for (int i = 0; i < genome.size(); i++)
        sequence[i] = (int)genome[i];

    clock_t start, finish;
    start = clock();
    auto sa_arr = suffix_array_smooth(sequence);
    finish = clock();
    fprintf(stderr, "%.6f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC);

    return 0;
}