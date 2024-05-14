#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

const int MIN_SIZE = 5;

//assumes that values of triples are in the range [-4, max(triples.size() + 3, ASCII)]
//performing radix sort on the set of triples
vector<int> renumber(const vector<vector<int>>& r) {
    int OFFSET = 4; // to avoid negative values
    int MMAX = 0; // max value found in triples
    for (int i = 0; i < (int)r.size(); i++)
        MMAX = max(MMAX, max(r[i][0], r[i][1]));

    // radix sort <- triples are given new numbers
    vector<vector<int>> radix_arr[3];
    for (int i = 0; i < 3; i++)
        radix_arr[i].resize(MMAX + OFFSET + 3);

    for (int i = 0; i < (int)r.size(); i++) // 3rd letter
        radix_arr[0][r[i][2] + OFFSET].push_back(i);

    for (int i = 0; i < MMAX + OFFSET + 3; i++) // 2nd letter
        for (auto ind : radix_arr[0][i])
            radix_arr[1][r[ind][1] + OFFSET].push_back(ind);

    for (int i = 0; i < MMAX + OFFSET + 3; i++) // 1st letter
        for (auto ind : radix_arr[1][i])
            radix_arr[2][r[ind][0] + OFFSET].push_back(ind);

    // renumbering
    int rank = -1;
    vector<int> renumbered((int)r.size(), 0);
    for (int i = 0; i < MMAX + OFFSET + 3; i++) {
        for (int j = 0; j < (int)radix_arr[2][i].size(); j++) {
            if (j != 0 && r[radix_arr[2][i][j]] == r[radix_arr[2][i][j - 1]])
                renumbered[radix_arr[2][i][j]] = rank;
            else
                renumbered[radix_arr[2][i][j]] = ++rank;
        }
    }
    return renumbered;
}

vector<int> renumber_pairs(vector<pair<int,int>> doubles) {
    int OFFSET = 4;
    int MMAX = 0;
    for (int i = 0; i < (int)doubles.size(); i++)
        MMAX = max(MMAX, max(doubles[i].first, doubles[i].second));

    vector<vector<int>> radix_arr[2];
    for (int i = 0; i < 2; i++)
        radix_arr[i].resize(MMAX + OFFSET + 3);

    for (int i = 0; i < (int)doubles.size(); i++)
        radix_arr[0][doubles[i].second + OFFSET].push_back(i);

    for (int i = 0; i < (int)radix_arr[0].size(); i++)
        for (auto ind : radix_arr[0][i])
            radix_arr[1][doubles[ind].first + OFFSET].push_back(ind);

    int rank = 0;
    vector<int> renumbered((int)doubles.size(), 0);
    for (int i = 0; i < (int)radix_arr[1].size(); i++)
        for (auto ind : radix_arr[1][i])
            renumbered[ind] = rank++;
    return renumbered;
}

vector<pair<int,int>> create_divisible_pairs(const vector<int>& s, const vector<int>& rank_s) {
    vector<pair<int,int>> divisible_pair;
    for (int i = 0; i < (int)s.size(); i += 3)
        divisible_pair.push_back({s[i], rank_s[i + 1]});

    return divisible_pair;
}

// merges sorted suffixed started at position = 0 % 3 and sorted suffixed that = 1 v 2 % 3
vector<int> merge(const vector<int>& s, const vector<int>& renumbered_divisible, const vector<int>& rank_s,
                  const vector<pair<int,int>>& divisible)
{
    vector<int> sa_r(2 * (int)s.size() / 3);
    for (int i = 0; i < (int)rank_s.size(); i++)
        if (rank_s[i] >= 0)
            sa_r[rank_s[i]] = 2 * (i / 3) + (i % 3) - 1;

    vector<int> sorted_divisible((int)renumbered_divisible.size(), 0);
    for (int i = 0; i < (int)renumbered_divisible.size(); i++)
        sorted_divisible[renumbered_divisible[i]] = i;

    vector<int> sa;
    int i = 0, j = 0;
    while (i < (int)sorted_divisible.size() && j < (int)sa_r.size()) {
        int j_pos = sa_r[j] + sa_r[j] / 2 + 1;
        int i_pos = sorted_divisible[i] * 3;
        if ((j_pos % 3) == 1) { //comparing i with position j == 1 % 3
            pair<int, int> j_token = {s[j_pos], rank_s[j_pos + 1]};
            if (divisible[sorted_divisible[i]] < j_token) {
                sa.push_back(i_pos);
                i++;
            }
            else {
                sa.push_back(j_pos);
                j++;
            }
        }
        else {
            if (j_pos == (int)s.size() - 1) {
                sa.push_back(j_pos);
                j++;
            }
            else {
                vector<int> i_token = {s[i_pos], s[i_pos + 1], rank_s[i_pos + 2]};
                vector<int> j_token = {s[j_pos], s[j_pos + 1], rank_s[j_pos + 2]};
                if (i_token < j_token) {
                    sa.push_back(i_pos);
                    i++;
                }
                else {
                    sa.push_back(j_pos);
                    j++;
                }
            }
        }
    }
    while (i < (int)sorted_divisible.size()) {
        int i_pos = sorted_divisible[i] * 3;
        sa.push_back(i_pos);
        i++;
    }
    while (j < (int)sa_r.size()) {
        int j_pos = sa_r[j] + sa_r[j] / 2 + 1;
        sa.push_back(j_pos);
        j++;
    }
    return sa;
}

bool compare_vectors(const vector<int>& s1, const vector<int>& s2) {
    for (int i = 1; i < (int)s1.size(); i++)
        if (s1[i] < s2[i])
            return true;
        else if (s2[i] < s1[i])
            return false;
    return true;
}

vector<int> sort_short(const vector<int>& s) {
    vector<vector<int>> suffixes(s.size());
    for (int i = 0; i < (int)s.size(); i++) {
        suffixes[i].push_back(i);
        for (int j = i; j < (int)s.size(); j++)
            suffixes[i].push_back(s[j]);
    }

    sort(suffixes.begin(), suffixes.end(), compare_vectors);
    vector<int> ans;
    for (int i = 0; i < (int)suffixes.size(); i++)
        ans.push_back(suffixes[i][0]);
    return ans;
}

//assumes that s has only positive entries, assumes that the last letter of s is smaller than any other letter
vector<int> build_sa_from_ks(vector<int>& s) {
    if (s.size() <= MIN_SIZE) {
        return sort_short(s);
    }
    // first padding
    int i = -1;
    while ((s.size() % 3) != 0)
        s.push_back(i--);

    //creating r1 and r2
    vector<vector<int>> r1, r2;
    s.push_back(-3); s.push_back(-4); //second padding to avoid vector overflow
    for (int i = 1; i < (int)s.size() - 2; i += 3) {
        r1.push_back({s[i], s[i + 1], s[i + 2]});
        r2.push_back({s[i + 1], s[i + 2], s[i + 3]});
    }
    s.pop_back(); s.pop_back(); //removing second padding

    vector<vector<int>> r(r1);
    for (int i = 0; i < (int)r2.size(); i++)
        r.push_back(r2[i]);
    vector<int> pos_of_r;
    for (int i = 1; i < (int)s.size(); i += 3)
        pos_of_r.push_back(i);
    for (int i = 2; i < (int)s.size(); i += 3)
        pos_of_r.push_back(i);

    vector<int> r_prime = renumber(r);
    vector<int> sa_r_prime = build_sa_from_ks(r_prime);
    vector<int> rank_s((int)s.size(), -1);
    for (int i = 0; i < (int)sa_r_prime.size(); i++)
        rank_s[pos_of_r[sa_r_prime[i]]] = i;

    vector<pair<int,int>> sa_divisible = create_divisible_pairs(s, rank_s);
    vector<int> sa_divisible_renumbered = renumber_pairs(sa_divisible);
    vector<int> sa_t = merge(s, sa_divisible_renumbered, rank_s, sa_divisible);

    // removing padding characters
    reverse(sa_t.begin(), sa_t.end());
    while (i != -1) {
        sa_t.pop_back();
        i++;
    }
    reverse(sa_t.begin(), sa_t.end());
    return sa_t;
}

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
    auto sa_arr = build_sa_from_ks(sequence);
    finish = clock();
    fprintf(stderr, "%.6f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC);

    return 0;
}