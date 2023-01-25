#include "Tree.h"
#include "iostream"

int main(int argc, char** argv){
    kTree::BST<int> tree;
    tree.Insert(100);
    tree.Insert(89);
    tree.Insert(94);
    tree.Insert(72);
    tree.Insert(70);
    tree.Insert(73);
    tree.Insert(75);
    tree.Insert(74);

    tree.Remove(89);
    kTree::AVL<int> avlTree;
    avlTree.InsertAndBalance(10);
    avlTree.InsertAndBalance(11);
    avlTree.InsertAndBalance(4);
    avlTree.InsertAndBalance(6);
    avlTree.InsertAndBalance(2);
    avlTree.InsertAndBalance(1);
    avlTree.InsertAndBalance(3);

    auto node = avlTree.Search(4);
    avlTree.TestRotateRight(node);
}