
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int N;
vector<vector<char>> grid;

int countArea(int r, int c) {
   if (r < 0 || r >= N + 2 || c < 0 || c >= N + 2) return 0;
   if (grid[r][c] != 'w') return 0;

   grid[r][c] = 'v';

   return 1
       + countArea(r - 1, c)
       + countArea(r + 1, c)
       + countArea(r, c - 1)
       + countArea(r, c + 1);
}

bool readGrid() {
   cout << "Enter the grid row by row.\n";
   cout << "Use 'w' for white and 'b' for black. Each row must have exactly " << N << " characters.\n";

   for (int r = 1; r <= N; ++r) {
       string line;

       while (true) {
           cout << "Row " << r << ": ";
           if (!getline(cin, line)) return false;

           string stripped;
           for (char c : line)
               if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
                   stripped += c;

           if ((int)stripped.size() != N) {
               cout << "Expected " << N << " characters, got " << stripped.size() << ". Try again.\n";
               continue;
           }

           bool valid = true;
           for (char c : stripped) {
               if (c != 'w' && c != 'b') {
                   cout << "Invalid character '" << c << "'. Use only 'w' or 'b'. Try again.\n";
                   valid = false;
                   break;
               }
           }
           if (!valid) continue;

           for (int c = 1; c <= N; ++c)
               grid[r][c] = stripped[c - 1];

           break;
       }
   }
   return true;
}

int main() {
   while (true) {
       cout << "Enter grid size N: ";
       string s;
       getline(cin, s);

       try {
           size_t pos;
           N = stoi(s, &pos);
           if (pos == s.size() && N >= 1 && N <= 100) break;
       }
       catch (...) {}

       cout << "Please enter an integer between 1 and 100.\n";
   }

   grid.assign(N + 2, vector<char>(N + 2, 'b'));

   if (!readGrid()) {
       cerr << "Input error.\n";
       return 1;
   }

   vector<int> areaSizes;

   for (int r = 1; r <= N; ++r) {
       for (int c = 1; c <= N; ++c) {
           if (grid[r][c] == 'w') {
               int sz = countArea(r, c);
               areaSizes.push_back(sz);
           }
       }
   }

   cout << "\nNumber of white areas: " << areaSizes.size() << "\n";

   if (!areaSizes.empty()) {
       // Manual descending sort (no algorithm library)
       for (int i = 0; i < (int)areaSizes.size(); i++) {
           for (int j = i + 1; j < (int)areaSizes.size(); j++) {
               if (areaSizes[i] < areaSizes[j]) {
                   int temp = areaSizes[i];
                   areaSizes[i] = areaSizes[j];
                   areaSizes[j] = temp;
               }
           }
       }

       cout << "Sizes (largest first): ";
       for (size_t i = 0; i < areaSizes.size(); ++i) {
           if (i > 0) cout << ", ";
           cout << areaSizes[i];
       }
       cout << "\n";
   }

   return 0;
}