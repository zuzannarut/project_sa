#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "sketch_minimizer.h"
#include "sa_radix.h"
using namespace std;
using namespace std::chrono;


int main(int argc, char** argv) {
    // change filePath to desired FASTA file
    // string filePath = "./test_data/felis.fasta";
    // string filePath = "./test_data/megavirus.fasta";
    // string filePath = "./test_data/pichia.fasta";
    string filePath = "./test_data/falco.fasta";
    // string filePath = "./test_data/salmonella.fa";

    cout << "Attempting to open file: " << filePath << endl;

    // open file
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file." << endl;
        return 1;
    }

    cout << "File opened successfully." << endl;

    // read header
    string firstLine;
    getline(file, firstLine);
    cout << "Header: " << firstLine << endl;

    // read sequence
    stringstream sequenceBuffer;
    string line;
    while (getline(file, line)) {
        sequenceBuffer << line;
    }

    string sequence = sequenceBuffer.str();
    file.close();

    cout << "File read successfully!" << endl;
    cout << "Sequence Length: " << sequence.length() << " characters" << endl;

    // optimal parameters for minimizer sketch
    size_t k = 11; 
    size_t w = 100; 

    // running approach with minimizer sketch and tracking runtime
    cout << "Starting to compute minimizer then build suffix array with minimizer!" << endl;
    auto start = high_resolution_clock::now();
    vector<pair<string, size_t>> minimizers = compute_minimizers(sequence, k, w);
    vector<int> sa_min = min_sketch_suffix_array(minimizers);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "Done computing minizer and building suffix array with minimizer!" << endl;
    cout << duration << endl;

    // running standard suffix array construction approach and tracking runtime
    cout << "Starting to build suffix array!" << endl;
    start = high_resolution_clock::now();
    vector<int> sa = standard_suffix_array(sequence);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "Done building suffix array!" << endl;
    cout << duration << endl;

    return 0;
}