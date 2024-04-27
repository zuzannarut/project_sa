#ifndef SA_RADIX_H
#define SA_RADIX_H

#include <string>
#include <vector>

// builds a suffix array of a sequence of ints, the last element of s must be the smallest
// the algorithms is based on the folklore doubling approach but contains some improvements
// comment: for building a suffix array of a string, cast the string to a sequence of ints first
// algorithm description: https://cp-algorithms.com/string/suffix-array.html

std::vector<int> suffix_array(const std::vector<int>& s) {
    int n = s.size();

    // Important: it assumes that values in s are in the range [0, s.size() - 1]
    int alphabet_size = n;

    std::vector<int> p(n), c(n), pn(n), cn(n), counts(n, 0);
    // counting sort on substrings of length 1
    for (auto el : s)
        counts[el]++;
    for (int i = 0; i < alphabet_size; i++)
        counts[i] += counts[i - 1];

    // creating the array p which is a sorted order of suffices sorted based on substrings of length 1
    for (int i = 0; i < n; i++) {
        counts[(int)s[i]]--;
        p[counts[(int)s[i]]] = i;
    }

    // assigning classes to elements of s
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i - 1]])
            c[p[i]] = c[p[i - 1]] + 1;
        else
            c[p[i]] = c[p[i - 1]];
    }

    // doubling the length by which the subtrings are sorted
    for (int h = 0; (1<<h) < n; h++) {
        // radix sorting substrings of length 2^h
        for (int i = 0; i < n; i++)
            if (p[i] - (1<<h) < 0)
                pn[i] = p[i] - (1<<h) + n;
            else
                pn[i] = p[i] - (1<<h);

        for (int i = 0; i < n; i++)
            counts[i] = 0;

        for (int i = 0; i < n; i++)
            counts[c[pn[i]]]++;

        for (int i = 1; i < n; i++)
            counts[i] += counts[i - 1];

        for (int i = n - 1; i >= 0; i--) {
            counts[c[pn[i]]]--;
            p[counts[c[pn[i]]]] = pn[i];
        }

        // creating classes for suffixes based on their sorted order up to length 2^h
        cn.swap(c);
        for (int i = 0; i < n; i++)
            c[i] = 0;

        for (int i = 1; i < n; i++) {
            std::pair<int, int> p1 = {cn[p[i]], cn[(p[i] + (1<<h)) % n]};
            std::pair<int, int> p2 = {cn[p[i - 1]], cn[(p[i - 1] + (1<<h)) % n]};
            if (p1 != p2)
                c[p[i]] = c[p[i - 1]] + 1;
            else
                c[p[i]] = c[p[i - 1]];
        }

        // if the number of classes is n, then all suffices are already different based on substrings of length 2^h
        // and we can terminate the algorithm
        if (c[p[n - 1]] == n - 1)
            break;
    }
    return p;
}

// code based on https://cp-algorithms.com/string/suffix-array.html
// s - the initial sequence on from which the suffix array has been constructed
// sa - the suffix array of s
std::vector<int> lcp_construction(const std::vector<int>& s, const std::vector<int>& sa) {
    int n = s.size();
    std::vector<int> rank(n, 0);
    for (int i = 0; i < n; i++)
        rank[sa[i]] = i;

    int k = 0;
    std::vector<int> lcp(n-1, 0);
    for (int i = 0; i < n; i++) {
        if (rank[i] == n - 1) {
            k = 0;
            continue;
        }
        int j = sa[rank[i] + 1];
        while (i + k < n && j + k < n && s[i+k] == s[j+k])
            k++;
        lcp[rank[i]] = k;
        if (k)
            k--;
    }
    return lcp;
}

#endif //SA_RADIX_H
