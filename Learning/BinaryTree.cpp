#include <iostream>
#include <stack>
#include <queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};



void pre_order_recursive(TreeNode* root) {
    if (root != NULL) {
        cout << root->val << ' ';
        pre_order_recursive(root->left);
        pre_order_recursive(root->right);
    }
}

void pre_order_non_recursive(TreeNode* root) {
    stack<TreeNode*> s;
    TreeNode* p = root;
    
    while (p != NULL || !s.empty()) {
        while (p != NULL) {
            cout << p->val << ' ';
            s.push(p);
            p = p->left;
        }
        if (!s.empty()) {
            p = s.top()->right;
            s.pop();
            // p = p->right;
        }
    }
}

void in_order_recursive(TreeNode* root) {
    if (root != NULL) {
        in_order_recursive(root->left);
        cout << root->val << ' ';
        in_order_recursive(root->right);
    }
}

void in_order_non_recursive(TreeNode* root) {
    stack<TreeNode*> s;
    TreeNode* p = root;

    while (p != NULL || !s.empty()) {
        while (p != NULL) {
            s.push(p);
            p = p->left;
        }
    
        if (!s.empty()) {
            p = s.top();
            cout << p->val << ' ';
            s.pop();
            p = p->right;
        }
    }
}

void post_order_recursive(TreeNode* root) {
    if (root != NULL) {
        post_order_recursive(root->left);
        post_order_recursive(root->right);
        cout << root->val << ' ';
    }
}

void post_order_non_recursive(TreeNode* root) {
    stack<TreeNode*> s;
    TreeNode* cur = NULL;
    TreeNode* pre = NULL;
    s.push(root);

    while (!s.empty()) {
        cur = s.top();
        if ((cur->left == NULL && cur->right == NULL) ||
            (pre != NULL && (pre == cur->left || pre == cur->right))) {
            cout << cur->val << ' ';
            s.pop();
            pre = cur;
        }
        else {
            if (cur->right != NULL)
                s.push(cur->right);
            if (cur->left != NULL)
                s.push(cur->left);
        }
    }

}

void level_order(TreeNode* root) {
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

        cout << p->val << ' ';
        q.pop();
    }
}

int main() {

    // build Tree
    TreeNode* root = new TreeNode(4);
    root->left = new TreeNode(2);
    root->right = new TreeNode(6);
    TreeNode* cur = root->left;
    cur->left = new TreeNode(1);
    cur->right = new TreeNode(3);
    cur = root->right;
    cur->left = new TreeNode(5);
    cur->right = new TreeNode(7);

    // pre order
    pre_order_recursive(root);
    cout << endl;
    pre_order_non_recursive(root);
    cout << endl;

    // in order
    in_order_recursive(root);
    cout << endl;
    in_order_non_recursive(root);
    cout << endl;

    // post order
    post_order_recursive(root);
    cout << endl;
    post_order_non_recursive(root);
    cout << endl;

    // level order
    level_order(root);
    cout << endl;

    return 0;
}