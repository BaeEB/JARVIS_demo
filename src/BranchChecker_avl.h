#include <stdio.h>

typedef struct Node
{
    int data;
    struct Node* Parent, * Left, * Right;
}Node;

int GetHeight(Node* node);
int CalculateBalanceFactor(Node* node);
Node* RR(Node* node);
Node* LL(Node* node);
Node* LR(Node* node);
Node* RL(Node* node);
Node* AVLSet(Node* node);
Node* Insert(Node* node, int data);
Node* GetMinNode(Node* node, Node* parent);
Node* Delete(Node* node, int data);
void Inorder(Node* node, int* result);
int* getInorder(Node* node);

