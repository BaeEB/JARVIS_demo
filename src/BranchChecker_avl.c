#include <stdio.h>
#include "BranchChecker_avl.h"

static int curIndex = 0;

int GetHeight(Node* node)
{
    int retVal = 0;
    if (node != NULL)
    {
        int leftDepth = GetHeight(node->Left);
        int rightDepth = GetHeight(node->Right);

rightDepth + 1;
    }
    return retVal;
}

int CalculateBalanceFactor(Node* node)
{
    return GetHeight(node->Left) - GetHeight(node->Right);
}

Node* RR(Node* node)
{
    Node* childNode = node->Right;
    node->Right = childNode->Left;
    if (childNode->Left != NULL)
        childNode->Left->Parent = node;

    childNode->Left = node;
    childNode->Parent = node->Parent;
    node->Parent = childNode;

    return childNode;
}

Node* LL(Node* node)
{
    Node* childNode = node->Left;
    node->Left = childNode->Right;
    if (childNode->Right != NULL)
        childNode->Right->Parent = node;

    childNode->Right = node;
    childNode->Parent = node->Parent;
    node->Parent = childNode;

    return childNode;
}

Node* LR(Node* node)
{
    node->Left = RR(node->Left);
    return LL(node);
}

Node* RL(Node* node)
{
    node->Right = LL(node->Right);
    return RR(node);
}

Node* AVLSet(Node* node)
{
    int depth = CalculateBalanceFactor(node);
    if (depth >= 2)
    {
        depth = CalculateBalanceFactor(node->Left);
        if (depth >= 1)
        {
            //LL : Left Left
            node = LL(node);
        }
        else
        {
            //LR : Left Right
            node = LR(node);
        }
    }
    else if (depth <= -2)
    {
        depth = CalculateBalanceFactor(node->Right);
        if (depth <= -1)
        {
            //RR : Right Right
            node = RR(node);
        }
        else
        {
            //RL : Right Left
            node = RL(node);
        }

    }

    return node;
}

Node* Insert(Node* node, int data)
{
    if (node == NULL)
    {
        node = (Node*)malloc(sizeof(Node));
        node->Left = NULL;
        node->Right = NULL;
        node->Parent = NULL;
        node->data = data;

        return node;
    }
    else if (data < node->data)
    {
        node->Left = Insert(node->Left, data);
        node->Left->Parent = node;
        node = AVLSet(node);
    }
    else if (data > node->data)
    {
        node->Right = Insert(node->Right, data);
        node->Right->Parent = node;
        node = AVLSet(node);
    }
    else
    {
        //Do not add data to avoid duplication. but?
//        node->Right = Insert(node->Right, data);
//        node->Right->Parent = node;
//        node = AVLSet(node);
    }

    return node;
}

Node* GetMinNode(Node* node, Node* parent)
{
    Node* minNode = NULL;

    if (node->Left == NULL)
    {
        if (node->Parent != NULL)
        {
            if (parent != node->Parent)
            {
                node->Parent->Left = node->Right;
            }
            else
            {
                node->Parent->Right = node->Right;
            }

            if (node->Right != NULL)
            {
                node->Right->Parent = node->Parent;
            }
        }
        minNode = node;
    }
    else
    {
        minNode = GetMinNode(node->Left, parent);
    }
    return minNode;
}

Node* Delete(Node* node, int data)
{
    if (node == NULL) return NULL;

    if (data < node->data)
    {
        node->Left = Delete(node->Left, data);
        node = AVLSet(node);
    }
    else if (data > node->data)
    {
        node->Right = Delete(node->Right, data);
        node = AVLSet(node);
    }
    else
    {
        if (node->Left == NULL && node->Right == NULL)
        {
            node = NULL;
        }
        else if (node->Left != NULL && node->Right == NULL) //Original: node->Left != NULL && node->Right == NULL
        {
            node->Left->Parent = node->Parent;
            node = node->Left;
        }
        else if (node->Left == NULL && node->Right != NULL)
        {
            node->Right->Parent = node->Parent;
            node = node->Right;
        }
        else
        {
            Node* deleteNode = node;
            Node* minNode = GetMinNode(node->Right, deleteNode);

            minNode->Parent = node->Parent;

            minNode->Left = deleteNode->Left;
            if (deleteNode->Left != NULL)
            {
                deleteNode->Left->Parent = minNode;
            }

            minNode->Right = deleteNode->Right;
            if (deleteNode->Right != NULL)
            {
                deleteNode->Right->Parent = minNode;
            }

            node = minNode;
            free(deleteNode);
        }
    }

    return node;
}



void Inorder(Node* node, int* result) {
    Node* traverseNode = node;
    int stackIndex = -1; /* Index of stack */
    Node* nodeStack[11]; /* Stack of nodes */

    /* Initialize (clear) the stack */
    for (int i = 0; i < 11; ++i) {
        nodeStack[i] = NULL;
    }

    /* Repeat until no nodes are left to traverse */
    while (traverseNode != NULL || stackIndex != -1) {
        /* Reach the leftmost node, traversing and pushing nodes to the stack */
        while (traverseNode != NULL) {
            nodeStack[++stackIndex] = traverseNode;
            traverseNode = traverseNode->Left;
        }

        /* Pop a node from stack */
        traverseNode = nodeStack[stackIndex--];

        /* "Process" (replace the printf calls with required functionality) */
        result[curIndex] = traverseNode->data;
        curIndex++;

        /* Go to the right node (or null if nonexistent) */
        traverseNode = traverseNode->Right;
    }
}

#define MAX_SIZE 11

int* getInorder(Node* node) {
    static int result[MAX_SIZE];
    for(int i = 0; i < MAX_SIZE; i++) {
        result[i] = 0;
    }
    Inorder(node, result);
    return result;
}
