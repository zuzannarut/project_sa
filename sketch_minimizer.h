#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// using algorithm from https://www.liebertpub.com/doi/full/10.1089/cmb.2023.0094
// purpose is to compare the runtime of standard suffix array construction and minimizer-space suffix array

// compute hash value for given k-mer (rolling polynomial hash)
size_t mini_sketch_hash_kmer(const string& kmer) {
    size_t hash = 0;
    // use 31 b/c prime and reduces chance of collisions
    size_t p_val = 31;
    size_t p_power = 1;
    for (char c : kmer) {
        hash += (c - 'A' + 1) * p_power;
        p_power *= p_val;
    }
    return hash;
}

// compute sketch minimizers
vector<pair<string, size_t>> compute_minimizers(const string& sequence, size_t k, size_t w) {
    vector<pair<string, size_t>> minimizers;
    if (sequence.length() < k) {
        return minimizers;
    }

    // sliding window
    for (size_t i = 0; i <= sequence.length() - w; ++i) {
        size_t curr = numeric_limits<size_t>::max();
        string min_kmer;
        size_t min = 0;

        for (size_t j = 0; j <= w - k; ++j) {
            string kmer = sequence.substr(i + j, k);
            size_t kmer_hash = mini_sketch_hash_kmer(kmer);

            if (kmer_hash < curr) {
                curr = kmer_hash;
                min_kmer = kmer;
                min = i + j;
            }
        }
        // store k-mer and index
        minimizers.emplace_back(min_kmer, min);
    }

    return minimizers;
}

// constructing suffix array using minimizer sketch
vector<int> min_sketch_suffix_array(const vector<pair<string, size_t>>& minimizers) {
    int n = minimizers.size();
    vector<int> sa(n);
    
    // suffix array with minimizer positions
    for (int i = 0; i < n; ++i) {
        sa[i] = minimizers[i].second;
    }
    
    // sort suffix array using minimizer array
    sort(sa.begin(), sa.end(), 
         [&](int a, int b) { return minimizers[a].first < minimizers[b].first; });

    return sa;
}

// standard approach of constructing suffix array
vector<int> standard_suffix_array(const string& sequence) {
    int n = sequence.size();
    vector<int> sa(n);
    vector<int> ranks(n);
    vector<int> temp(n);
    
    // ranking
    for (int i = 0; i < n; ++i) {
        sa[i] = i;
        ranks[i] = sequence[i];
    }
    
    for (int length = 1; length < n; length *= 2) {
        auto compare = [&](int a, int b) {
            if (ranks[a] != ranks[b]) return ranks[a] < ranks[b];
            int ra = (a + length < n) ? ranks[a + length] : -1;
            int rb = (b + length < n) ? ranks[b + length] : -1;
            return ra < rb;
        };
        
        // sort
        sort(sa.begin(), sa.end(), compare);
        
        temp[sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            temp[sa[i]] = temp[sa[i - 1]] + compare(sa[i - 1], sa[i]);
        }
        ranks = temp;
    }
    
    return sa;
}
