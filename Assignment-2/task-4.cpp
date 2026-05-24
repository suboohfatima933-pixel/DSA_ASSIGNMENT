
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Node {
   int key;
   Node* left;
   Node* right;
   Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

class BST {
   Node* root;

   Node* insertRec(Node* node, int key) {
       if (!node) return new Node(key);
       if (key < node->key) node->left = insertRec(node->left, key);
       else if (key > node->key) node->right = insertRec(node->right, key);
       return node;
   }

   int inorderRec(Node* node) const {
       if (!node) return 0;
       return inorderRec(node->left) + node->key + inorderRec(node->right);
   }

   Node* minNode(Node* node) {
       while (node->left) node = node->left;
       return node;
   }

   Node* deleteRec(Node* node, int key) {
       if (!node) return nullptr;

       if (key < node->key)
           node->left = deleteRec(node->left, key);
       else if (key > node->key)
           node->right = deleteRec(node->right, key);
       else {
           if (!node->left) {
               Node* r = node->right;
               delete node;
               return r;
           }
           if (!node->right) {
               Node* l = node->left;
               delete node;
               return l;
           }

           Node* successor = minNode(node->right);
           node->key = successor->key;
           node->right = deleteRec(node->right, successor->key);
       }
       return node;
   }

   void destroyRec(Node* node) {
       if (!node) return;
       destroyRec(node->left);
       destroyRec(node->right);
       delete node;
   }

public:
   BST() : root(nullptr) {}
   ~BST() { destroyRec(root); }

   void insert(int key) { root = insertRec(root, key); }
   int inorder() const { return inorderRec(root); }
   void remove(int key) { root = deleteRec(root, key); }
};

double durationNs(high_resolution_clock::time_point start,
   high_resolution_clock::time_point end) {
   return duration_cast<nanoseconds>(end - start).count();
}

int main() {
   const int SETS = 5;
   const int COUNT = 100;

   mt19937 rng(42);
   uniform_int_distribution<int> dist(1, 10000);

   double totalInsert = 0.0, totalTraversal = 0.0;

   cout << left
       << setw(10) << "Set #"
       << setw(30) << "Time for Insertions (ns)"
       << "Time for Inorder Traversal (ns)" << "\n";

   cout << string(70, '-') << "\n";

   for (int s = 1; s <= SETS; ++s) {
       vector<int> nums(COUNT);
       for (int& n : nums) n = dist(rng);

       BST bst;

       auto t0 = high_resolution_clock::now();
       for (int n : nums) bst.insert(n);
       auto t1 = high_resolution_clock::now();
       double insertTime = durationNs(t0, t1);

       auto t2 = high_resolution_clock::now();
       int dummy = bst.inorder();
       auto t3 = high_resolution_clock::now();
       double traversalTime = durationNs(t2, t3);
       (void)dummy;

       totalInsert += insertTime;
       totalTraversal += traversalTime;

       cout << left
           << setw(10) << s
           << setw(30) << fixed << setprecision(0) << insertTime
           << traversalTime << "\n";
   }

   cout << string(70, '-') << "\n";
   cout << left
       << setw(10) << "Average"
       << setw(30) << fixed << setprecision(0)
       << (totalInsert / SETS)
       << (totalTraversal / SETS) << "\n";

   return 0;
}