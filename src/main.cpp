#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <unordered_map>
using namespace std;

vector<string> utf8_split(const string &str){
    vector<string> split;
    int len = str.length();
    int left = 0;
    int right = 1;

    for (int i = 0; i < len; i++){
        if (right >= len || ((str[right] & 0xc0) != 0x80)){
            string s = str.substr(left, right - left);
            split.push_back(s);
            // printf("%s %d %d\n", s.c_str(), left, right);
            left = right;
        }
        ++right;
    }
    return split;
}


// 最长公共子序列（不连续）
int lcs_sequence_length(const string &str1, const string &str2) {
    if (str1 == "" || str2 == "")
        return 0;
    vector<string> s1 = utf8_split(str1);
    vector<string> s2 = utf8_split(str2);
    int m = s1.size();
    int n = s2.size();
    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= m; ++i) {
        int prev = 0;
        for (int j = 1; j <= n ; ++j) {
            int temp = dp[j];
            if (s1[i - 1] == s2[j - 1]) {
                dp[j] = prev + 1;
            } else {
                dp[j] = max(dp[j], dp[j - 1]);
            }
            prev = temp;
        }
    }
    return dp[n];
}

// 我们用到的核心代码，我只优化这个，这一版内存是O(n)的
vector<int> lcs_sequence_idx(const string &str, const string &ref) {
    /***
     * Hunt-Szymanski Algorithm for LCS
     *
     * Complexity: O(R + N) log N
     * R = numbered of ordered pairs of positions where the two strings match (worst case, R = N^2)
     *
     * copied from https://github.com/sgtlaugh/algovault/blob/0ba0b3eb0d2e31e78de05da188f13d8d7d0365ae/code_library/hunt_szymanski.cpp
     ***/
    vector<string> A = utf8_split(str);
    vector<string> B = utf8_split(ref);
    
    unordered_map<string, vector<int>> adj; // 均摊O(B.size())，当然实际可能会大一点但是绝对不会达到A*B
    int i, j, n = A.size(), m = B.size();
    if (m == 0 || n == 0)
        return vector<int>(A.size(), -1);

    for (i = 0; i < m; i++)
        adj[B[i]].push_back(i);
    vector<int> ar{-1};
    ar.reserve(A.size());
    for (i = 0; i < n; i++)
    {
        for (j = (int)adj[A[i]].size() - 1; j >= 0; j--) // 对于英文字符来说，这种方法不一定很优，因为每个adj的桶里的内容都很多，但是对于中文字符来说这种做法很可能很快
        {
            int x = adj[A[i]][j];
            if (x > ar.back())
                ar.emplace_back(x);
            else
                ar[lower_bound(ar.begin(), ar.end(), x) - ar.begin()] = x; // 最坏情况出现在A串和B串都由同一个字符构造而来，这种情况是n*m*log(n)的时间，但是在我们的数据上不可能发生
        }
    }
    vector<int> idx(A.size(), -1);

    j = 1;
    for(i = 0; i < A.size(); ++i)
        if(A[i] == B[ar[j]])
            idx[i] = ar[j++];

    return idx;
}
/*
// 稍微省一点内存的版本
vector<int> lcs_sequence_idx(const string &str, const string &ref) {
    vector<string> s1 = utf8_split(str);
    vector<string> s2 = utf8_split(ref);
    int m = s1.size();
    int n = s2.size();
    vector<int> dp(n + 1, 0);
    vector<vector<bool>> direct_r(m, vector<bool>(n, 0));
    vector<vector<bool>> direct_m(m, vector<bool>(n, 0)); // for space efficiency
    vector<int> res(m, -1);
    if (m == 0 || n == 0)
        return res;

    int i, j;
    for (i = 1; i <= m; i++) {
        int prev = 0;
        for (j = 1; j <= n; j++) {
            int temp = dp[j];
            if (s1[i - 1] == s2[j - 1]) {
                dp[j] = prev + 1;
                direct_m[i - 1][j - 1] = 1; // match
            } else {
                if (dp[j] < dp[j - 1]) {
                    dp[j] = dp[j - 1];
                    direct_r[i - 1][j - 1] = 1; // str+1
                }
                    // direct_r = 1;     // ref+1
            }
            prev = temp;
        }
    }
    for (i = m, j = n; i > 0 && j > 0; ){
        if (direct_m[i - 1][j - 1]){
            res[i-1] = j-1;
            i--; j--;
        }
        else if (direct_r[i - 1][j - 1]) j--;
        else i--;
    }
    return res;
}*/


// 最长公共子串（连续）
int lcs_string_length(const string &str1, const string &str2) {
    if (str1 == "" || str2 == "")
        return 0;
    vector<string> s1 = utf8_split(str1);
    vector<string> s2 = utf8_split(str2);
    int m = s1.size();
    int n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    int i, j;
    int max = 0;

    for (i = 0; i <= m; i++) {
        dp[i][0] = 0;
    }
    for (j = 0; j <= n; j++) {
        dp[0][j] = 0;
    }
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > max){
                    max = dp[i][j];
                }
            }
            else {
                dp[i][j] = 0;
            }
        }
    }
    return max;
}


vector<int> lcs_string_idx(const string &str, const string &ref) {
    vector<string> s1 = utf8_split(str);
    vector<string> s2 = utf8_split(ref);
    int m = s1.size();
    int n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    vector<int> res(m, -1);
    if (m == 0 || n == 0)
        return res;

    int i, j;
    int max_i = 0, max_j = 0;
    for (i = 0; i <= m; i++) {
        dp[i][0] = 0;
    }
    for (j = 0; j <= n; j++) {
        dp[0][j] = 0;
    }
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > dp[max_i][max_j]){
                    max_i = i; max_j = j;
                }
            }
            else {
                dp[i][j] = 0;
            }
        }
    }
    for (i = 0; i < dp[max_i][max_j]; i++) {
        res[max_i-i-1] = max_j-i-1;
    }
    return res;
}


vector<int> lcs_sequence_of_list(const string &str1, vector<string> &str_list){
    int size = str_list.size();
    vector<int> ls(size);
    for (int i = 0; i < size; i++){
        int l = lcs_sequence_length(str1, str_list[i]);
        ls[i] = l;
    }
    return ls;
}


vector<int> lcs_string_of_list(const string &str1, vector<string> &str_list){
    int size = str_list.size();
    vector<int> ls(size);
    for (int i = 0; i < size; i++){
        int l = lcs_string_length(str1, str_list[i]);
        ls[i] = l;
    }
    return ls;
}


// 编辑距离
int levenshtein_distance(const string &str1, const string &str2) {
    vector<string> s1 = utf8_split(str1);
    vector<string> s2 = utf8_split(str2);
    int m = s1.size();
    int n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    int i, j;

    for (i = 0; i <= m; i++) {
        dp[i][0] = i;
    }
    for (j = 0; j <= n; j++) {
        dp[0][j] = j;
    }
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min({dp[i][j - 1], dp[i - 1][j], dp[i - 1][j - 1]});
            }
        }
    }
    return dp[m][n];
}


bool inline __contains(map<string, map<string, float>> m, string k1, string k2) {
    return m.find(k1) != m.end() && m[k1].find(k2) != m[k1].end();
}

float levenshtein_distance_weighted(const string &str1, const string &str2, map<string, map<string, float>> &weight) {
    vector<string> s1 = utf8_split(str1);
    vector<string> s2 = utf8_split(str2);
    string __("");
    int m = s1.size();
    int n = s2.size();
    vector<vector<float>> dp(m + 1, vector<float>(n + 1));
    int i, j;
    float si, sj, sij;

    dp[0][0] = 0;
    for (i = 1; i <= m; i++) {
        dp[i][0] = dp[i-1][0] + (__contains(weight, s1[i-1], __) ? weight[s1[i-1]][__] : 1);
    }
    for (j = 1; j <= n; j++) {
        dp[0][j] = dp[0][j-1] + (__contains(weight, __, s2[j-1]) ? weight[__][s2[j-1]] : 1);
    }
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            si = __contains(weight, s1[i-1], __) ? weight[s1[i-1]][__] : 1;
            sj = __contains(weight, __, s2[j-1]) ? weight[__][s2[j-1]] : 1;
            sij = __contains(weight, s1[i-1], s2[j-1]) ? weight[s1[i-1]][s2[j-1]] : (s1[i-1] == s2[j-1] ? 0 : 1);
            dp[i][j] = min({dp[i-1][j] + si, dp[i][j-1] + sj, dp[i-1][j-1] + sij});
        }
    }
    return dp[m][n];
}


vector<int> levenshtein_distance_idx_weighted(const string &str, const string &ref, map<string, map<string, float>> &weight) {
    vector<string> s1 = utf8_split(str);
    vector<string> s2 = utf8_split(ref);
    string __("");
    int m = s1.size();
    int n = s2.size();
    vector<vector<float>> dp(m + 1, vector<float>(n + 1));
    vector<vector<char>> direct(m + 1, vector<char>(n + 1));
    vector<int> res(m, -1);
    int i, j;
    float si, sj, sij;

    dp[0][0] = 0;
    for (i = 1; i <= m; i++) {
        dp[i][0] = dp[i-1][0] + (__contains(weight, s1[i-1], __) ? weight[s1[i-1]][__] : 1);
        direct[i][0] = 's';
    }
    for (j = 1; j <= n; j++) {
        dp[0][j] = dp[0][j-1] + (__contains(weight, __, s2[j-1]) ? weight[__][s2[j-1]] : 1);
        direct[0][j] = 'r';
    }
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            si = __contains(weight, s1[i-1], __) ? weight[s1[i-1]][__] : 1;
            sj = __contains(weight, __, s2[j-1]) ? weight[__][s2[j-1]] : 1;
            sij = __contains(weight, s1[i-1], s2[j-1]) ? weight[s1[i-1]][s2[j-1]] : (s1[i-1] == s2[j-1] ? 0 : 1);
            if (dp[i-1][j-1] + sij <= dp[i-1][j] + si && dp[i-1][j-1] + sij <= dp[i][j-1] + sj) {
                dp[i][j] = dp[i-1][j-1] + sij;
                direct[i][j] = 'm';
            }
            else if (dp[i-1][j] + si <= dp[i][j-1] + sj) {
                dp[i][j] = dp[i-1][j] + si;
                direct[i][j] = 's';
            }
            else {
                dp[i][j] = dp[i][j-1] + sj;
                direct[i][j] = 'r';
            }
        }
    }

    for (i = m, j = n; i > 0 && j > 0; ){
        if (direct[i][j] == 'm') {
            res[i-1] = j-1;
            i--; j--;
        }
        else if (direct[i][j] == 's') i--;
        else if (direct[i][j] == 'r') j--;
    }
    return res;
}


vector<int> levenshtein_distance_of_list(const string &str1, vector<string> &str_list){
    int size = str_list.size();
    vector<int> ls(size);
    for (int i = 0; i < size; i++){
        int l = levenshtein_distance(str1, str_list[i]);
        ls[i] = l;
    }
    return ls;
}


namespace py = pybind11;


PYBIND11_MODULE(pylcs, m) {
    m.def("lcs", &lcs_sequence_length, R"pbdoc(Longest common subsequence)pbdoc");
    m.def("lcs_sequence_length", &lcs_sequence_length, R"pbdoc(Longest common subsequence)pbdoc");
    m.def("lcs_sequence_idx", &lcs_sequence_idx, R"pbdoc(Longest common subsequence indices mapping from str to ref)pbdoc",
        py::arg("s"), py::arg("ref"));
    m.def("lcs_of_list", &lcs_sequence_of_list, R"pbdoc(Longest common subsequence of list)pbdoc");
    m.def("lcs_sequence_of_list", &lcs_sequence_of_list, R"pbdoc(Longest common subsequence of list)pbdoc");

    m.def("lcs2", &lcs_string_length, R"pbdoc(Longest common substring)pbdoc");
    m.def("lcs_string_length", &lcs_string_length, R"pbdoc(Longest common substring)pbdoc");
    m.def("lcs_string_idx", &lcs_string_idx, R"pbdoc(Longest common substring indices mapping from str to ref)pbdoc",
        py::arg("s"), py::arg("ref"));
    m.def("lcs2_of_list", &lcs_string_of_list, R"pbdoc(Longest common substring of list)pbdoc");
    m.def("lcs_string_of_list", &lcs_string_of_list, R"pbdoc(Longest common substring of list)pbdoc");

    m.def("levenshtein_distance", &levenshtein_distance, R"pbdoc(Levenshtein Distance of Two Strings)pbdoc");
    m.def("levenshtein_distance", &levenshtein_distance_weighted, R"pbdoc(Levenshtein Distance of Two Strings. A weight dict<str, dict<str, float>> can be used.)pbdoc",
        py::arg("str1"), py::arg("str2"), py::arg("weight"));
    m.def("levenshtein_distance_idx", &levenshtein_distance_idx_weighted, R"pbdoc(Levenshtein Distance indices mapping from str to ref)pbdoc",
        py::arg("s"), py::arg("ref"), py::arg("weight")=map<string, map<string, float>>());
    m.def("edit_distance", &levenshtein_distance, R"pbdoc(Same As levenshtein_distance(): Levenshtein Distance of Two Strings)pbdoc");
    m.def("edit_distance", &levenshtein_distance_weighted, R"pbdoc(Same As levenshtein_distance(): Levenshtein Distance of Two Strings)pbdoc",
        py::arg("str1"), py::arg("str2"), py::arg("weight"));
    m.def("edit_distance_idx", &levenshtein_distance_idx_weighted, R"pbdoc(Edit Distance indices mapping from str to ref)pbdoc",
        py::arg("s"), py::arg("ref"), py::arg("weight")=map<string, map<string, float>>());
    m.def("levenshtein_distance_of_list", &levenshtein_distance_of_list, R"pbdoc(Levenshtein Distance of one string to a list of strings)pbdoc");
    m.def("edit_distance_of_list", &levenshtein_distance_of_list, R"pbdoc(Levenshtein Distance of one string to a list of strings)pbdoc");
}
