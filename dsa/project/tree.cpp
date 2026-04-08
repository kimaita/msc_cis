#include "types.h"
#include <algorithm>
#include <functional>

static int height(AVLNode *n) { return n ? n->height : 0; }
static int balance_factor(AVLNode *n) {
  return n ? height(n->left) - height(n->right) : 0;
}

static void update_height(AVLNode *n) {
  if (n)
    n->height = 1 + std::max(height(n->left), height(n->right));
}

static AVLNode *rotate_right(AVLNode *y) {
  AVLNode *x = y->left;
  AVLNode *t2 = x->right;
  x->right = y;
  y->left = t2;
  update_height(y);
  update_height(x);
  return x;
}

static AVLNode *rotate_left(AVLNode *x) {
  AVLNode *y = x->right;
  AVLNode *t2 = y->left;
  y->left = x;
  x->right = t2;
  update_height(x);
  update_height(y);
  return y;
}

static AVLNode *rebalance(AVLNode *n) {
  update_height(n);
  int bf = balance_factor(n);

  if (bf > 1 && balance_factor(n->left) >= 0)
    return rotate_right(n);
  if (bf > 1 && balance_factor(n->left) < 0) {
    n->left = rotate_left(n->left);
    return rotate_right(n);
  }
  if (bf < -1 && balance_factor(n->right) <= 0)
    return rotate_left(n);
  if (bf < -1 && balance_factor(n->right) > 0) {
    n->right = rotate_right(n->right);
    return rotate_left(n);
  }
  return n;
}

AVLNode *avl_insert(AVLNode *root, const Patient &p) {
  if (!root) {
    auto *n = new AVLNode{};
    n->data = p;
    return n;
  }
  if (p.id < root->data.id)
    root->left = avl_insert(root->left, p);
  else if (p.id > root->data.id)
    root->right = avl_insert(root->right, p);
  else
    root->data = p; // update existing
  return rebalance(root);
}

static AVLNode *min_node(AVLNode *n) {
  while (n->left)
    n = n->left;
  return n;
}

AVLNode *avl_remove(AVLNode *root, int id) {
  if (!root)
    return nullptr;
  if (id < root->data.id)
    root->left = avl_remove(root->left, id);
  else if (id > root->data.id)
    root->right = avl_remove(root->right, id);
  else {
    if (!root->left || !root->right) {
      AVLNode *child = root->left ? root->left : root->right;
      delete root;
      return child;
    }
    AVLNode *successor = min_node(root->right);
    root->data = successor->data;
    root->right = avl_remove(root->right, successor->data.id);
  }
  return rebalance(root);
}

AVLNode *avl_search(AVLNode *root, int id) {
  if (!root || root->data.id == id)
    return root;
  return id < root->data.id ? avl_search(root->left, id)
                            : avl_search(root->right, id);
}

void avl_inorder(AVLNode *root,
                 const std::function<void(const Patient &)> &visitor) {
  if (!root)
    return;
  avl_inorder(root->left, visitor);
  visitor(root->data);
  avl_inorder(root->right, visitor);
}

void avl_free(AVLNode *root) {
  if (!root)
    return;
  avl_free(root->left);
  avl_free(root->right);
  delete root;
}
