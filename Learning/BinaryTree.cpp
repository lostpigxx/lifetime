/* 
 *   Basic operations of BST (binary search tree) including:
 *      pre order traversal
 *      in order traversal
 *      post order traversal
 *      level order traversal
 *      reverse tree
 */


#include <iostream>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};



void pre_order_recursive(TreeNode* root, vector<int>& ans) {
    if (root != NULL) {
        ans.push_back(root->val);
        pre_order_recursive(root->left, ans);
        pre_order_recursive(root->right, ans);
    }
}

void pre_order_iteration(TreeNode* root, vector<int>& ans) {
    if (root == NULL)
        return;
    
    TreeNode *p;
    stack<TreeNode*> s;
    s.push(root);
    while (!s.empty()) {
        p = s.top();
        s.pop();
        ans.push_back(p->val);
        if (p->right)
            s.push(p->right);
        if (p->left)
            s.push(p->left);
    }
}


void in_order_recursive(TreeNode* root, vector<int>& ans) {
    if (root != NULL) {
        in_order_recursive(root->left, ans);
        ans.push_back(root->val);
        in_order_recursive(root->right, ans);
    }
}

void in_order_iteration(TreeNode* root, vector<int>& ans) {
    stack<TreeNode*> s;
    TreeNode* p = root;

    while (p != NULL || !s.empty()) {
        while (p != NULL) {
            s.push(p);
            p = p->left;
        }
    
        if (!s.empty()) {
            p = s.top();
            ans.push_back(p->val);
            s.pop();
            p = p->right;
        }
    }
}

void post_order_recursive(TreeNode* root, vector<int>& ans) {
    if (root != NULL) {
        post_order_recursive(root->left, ans);
        post_order_recursive(root->right, ans);
        ans.push_back(root->val);
    }
}

void post_order_iteration(TreeNode* root, vector<int>& ans) {
    if (root == NULL)
        return;

    stack<TreeNode*> s;
    TreeNode* cur = NULL;
    TreeNode* pre = NULL;
    s.push(root);

    while (!s.empty()) {
        cur = s.top();
        if ((cur->left == NULL && cur->right == NULL) ||
            (pre != NULL && (pre == cur->left || pre == cur->right))) {
            ans.push_back(cur->val);
            s.pop();
            pre = cur;
        }
        else {
            if (cur->right)
                s.push(cur->right);
            if (cur->left)
                s.push(cur->left);
        }
    }

}

void post_order_iteration_reverse(TreeNode* root, vector<int>& ans) {
    if (root == NULL)
        return;

    TreeNode *p;
    stack<TreeNode*> s;
    vector<int> t;
    s.push(root);

    while (!s.empty()) {
        p = s.top();
        s.pop();
        if (p->left) 
            s.push(p->left);
        if (p->right)
            s.push(p->right);
        t.push_back(p->val);
    }

    for (int i = t.size() - 1; i >= 0; i--)
        ans.push_back(t[i]);
}


void level_order(TreeNode* root, vector<int>& ans) {
    if (root == NULL)
        return;

    queue<TreeNode*> q;
    TreeNode* p = NULL;
    q.push(root);

    while (!q.empty()) {
        p = q.front();
        if (p->left != NULL)
            q.push(p->left);
        if (p->right != NULL)
            q.push(p->right);
        ans.push_back(p->val);
        q.pop();
    }
}

void reverse_recursive(TreeNode* root) {
    if (root == NULL)
        return;
    if (root->left == NULL && root->right == NULL)
        return;
    
    TreeNode* tmp = NULL;
    tmp = root->left;
    root->left = root->right;
    root->right = tmp;

    if (root->left)
        reverse_recursive(root->left);
    if (root->right)
        reverse_recursive(root->right);

}

void reverse_iteration(TreeNode* root) {
    if (root == NULL)
        return;
    
    queue<TreeNode*> q;
    TreeNode* p;
    q.push(root);

    while(!q.empty()) {
        p = q.front();
        if (p->left)
            q.push(p->left);
        if (p->right)
            q.push(p->right);

        TreeNode* tmp = p->left;
        p->left = p->right;
        p->right = tmp;

        q.pop();
    }
    
}

void printVector(const vector<int> &v) {
    cout << '[';
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << ", ";
    }
    cout << ']' << endl;
}

int main() {

    // build Tree
    /*
                4
              /   \
             2     6
            / \   / \
           1   3 5   7
    */
    TreeNode* root = new TreeNode(4);
    root->left = new TreeNode(2);
    root->right = new TreeNode(6);
    TreeNode* cur = root->left;
    cur->left = new TreeNode(1);
    cur->right = new TreeNode(3);
    cur = root->right;
    cur->left = new TreeNode(5);
    cur->right = new TreeNode(7);

    vector<int> ans;

    // pre order
    ans.clear();
    pre_order_recursive(root, ans);
    printVector(ans);

    ans.clear();
    pre_order_iteration(root, ans);
    printVector(ans);



    // // in order
    ans.clear();
    in_order_recursive(root, ans);
    printVector(ans);

    ans.clear();
    in_order_iteration(root, ans);
    printVector(ans);


    // // post order
    ans.clear();
    post_order_recursive(root, ans);
    printVector(ans);

    ans.clear();
    post_order_iteration(root, ans);
    printVector(ans);

    ans.clear();
    post_order_iteration_reverse(root, ans);
    printVector(ans);

    // level order
    ans.clear();
    level_order(root, ans);
    printVector(ans);


    // reverse
    ans.clear();
    // reverse_recursive(root);
    reverse_iteration(root);
    level_order(root, ans);
    printVector(ans);

    return 0;
}