#ifndef SA_RADIX_H
#define SA_RADIX_H

#include <string>
#include <vector>



void sort_interval(std::vector<int>& p, std::vector<int>& c, int n, int h, int a, int b) {
    if (b - a == 1)
        return;
    std::vector<int> cn(b - a);
    cn[0] = c[p[a]];
    int poz = c[0];
    //std::cout << a << " " << b << "\n";
    sort(p.begin() + a, p.begin() + b,
         [&] (const int key1, const int key2) {return c[(key1 + (1 << h)) % n] < c[(key2 + (1 << h)) % n];});
    for (int i = a + 1; i < b; i++) {
        poz++;
        std::pair<int, int> p1 = {c[p[i]], c[(p[i] + (1<<h)) % n]};
        std::pair<int, int> p2 = {c[p[i - 1]], c[(p[i - 1] + (1<<h)) % n]};
        if (p1 != p2)
            cn[i - a] = poz;
        else
            cn[i - a] = cn[i - 1 - a];
    }
    for (int i = a; i < b; i++)
        c[p[i]] = cn[i - a];
}

std::vector<int> suffix_array_smooth(const std::vector<int>& s) {
    int n = s.size();

    // Important: it assumes that values in s are in the range [0, s.size() - 1]
    int alphabet_size = std::max(n, (int)256);

    std::vector<int> p(n), c(n, 0);
    std::vector<int> pn(n), cn(n), counts(alphabet_size, 0);
    // counting sort on substrings of length 1
    for (auto el : s)
        counts[el]++;
    for (int i = 1; i < alphabet_size; i++)
        counts[i] += counts[i - 1];

    // creating the array p which is a sorted order of suffices sorted based on substrings of length 1
    for (int i = 0; i < n; i++) {
        counts[s[i]]--;
        p[counts[s[i]]] = i;
    }

    // assigning classes to elements of s
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i - 1]])
            c[p[i]] = c[p[i - 1]] + 1;
        else
            c[p[i]] = c[p[i - 1]];
    }
    int diff = 0;
    // doubling the length by which the subtrings are sorted
    for (int h = 0; (1<<h) < n; h++) {
        // radix sorting substrings of length 2^h

        if (10 * diff >  9 * n) {
            int prev = 0;
            for (int j = 1; j < n; j++) {
                if (c[p[j]] != c[p[j - 1]]) {
                    sort_interval(p, c, n, h, prev, j);
                    prev = j;
                }
            }
        }
        else {
            diff = 0;

            for (int i = 0; i < n; i++)
                if (p[i] - (1 << h) < 0)
                    pn[i] = p[i] - (1 << h) + n;
                else
                    pn[i] = p[i] - (1 << h);

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
            int poz = 0;
            for (int i = 1; i < n; i++) {
                poz++;
                std::pair<int, int> p1 = {cn[p[i]], cn[(p[i] + (1 << h)) % n]};
                std::pair<int, int> p2 = {cn[p[i - 1]], cn[(p[i - 1] + (1 << h)) % n]};
                if (p1 != p2) {
                    c[p[i]] = poz;
                    diff++;
                } else
                    c[p[i]] = c[p[i - 1]];
            }
        }
        // if the number of classes is n, then all suffices are already different based on substrings of length 2^h
        // and we can terminate the algorithm
        std::cout << diff << "\n";
        std::cout << c[p[n - 1]] << "\n";
        //if (c[p[n - 1]] == n - 1)
        //    break;

    }
    return p;
}

// builds a suffix array of a sequence of ints, the last element of s must be the smallest
// the algorithms is based on the folklore doubling approach but contains some improvements
// comment: for building a suffix array of a string, cast the string to a sequence of ints first
// algorithm description: https://cp-algorithms.com/string/suffix-array.html

std::vector<int> suffix_array(const std::vector<int>& s) {
    int n = s.size();

    // Important: it assumes that values in s are in the range [0, s.size() - 1]
    int alphabet_size = std::max(n, (int)256);

    std::vector<int> p(n), c(n, 0);
    std::vector<int> pn(n), cn(n), counts(alphabet_size, 0);
    // counting sort on substrings of length 1
    for (auto el : s)
        counts[el]++;
    for (int i = 1; i < alphabet_size; i++)
        counts[i] += counts[i - 1];

    // creating the array p which is a sorted order of suffices sorted based on substrings of length 1
    for (int i = 0; i < n; i++) {
        counts[s[i]]--;
        p[counts[s[i]]] = i;
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
        std::cout << c[p[n - 1]] << "\n";
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
