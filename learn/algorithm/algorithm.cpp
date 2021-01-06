#include <iostream>
#include <string>
#include <vector>

using namespace std;

int LongestCommonSubstring(string A, string B) {
    int m = A.size(), n = B.size();

    if (m == 0 || n == 0)
        return 0;
    
    int maxLen = 0;
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));

    for (int i = 1; i < m + 1; i++) {
        for (int j = 1; j < n + 1; j++) {
            if (A[i-1] == B[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
                maxLen = max(maxLen, dp[i][j]);
            }
        }
    }
    
    return maxLen;
}

int main() {
    cout << LongestCommonSubstring("1AB2345CD", "12345EF") << endl;
}