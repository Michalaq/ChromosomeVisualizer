#include "multimap.h"
#include <algorithm>

Node::Node(unsigned f, double v) :
    left(nullptr),
    right(nullptr),
    frame(f),
    value(v),
    lbound(f),
    rbound(f),
    minv(v),
    maxv(v),
    leftm(v),
    rightm(v),
    lsize(0),
    rsize(0)
{

}

Node::Node(unsigned f, double v, Node* l) :
    left(l),
    right(nullptr),
    frame(f),
    value(v),
    lbound(l->lbound),
    rbound(f),
    minv(std::min(l->minv, v)),
    maxv(std::max(l->maxv, v)),
    leftm(l->leftm),
    rightm(v),
    lsize(2 * l->lsize + 1),
    rsize(0)
{

}

Node::~Node()
{
    delete left;
    delete right;
}

Node* Node::insert(unsigned f, double v, Node* node)
{
    if (!node)
        return new Node(f, v);

    if (node->rsize < node->lsize)
    {
        node->right = insert(f, v, node->right);

        node->rbound = f;

        node->minv = std::min(node->minv, v);
        node->maxv = std::max(node->maxv, v);

        node->rightm = v;

        node->rsize++;

        return node;
    }

    return new Node(f, v, node);
}

double Node::minimum(unsigned lb, unsigned rb, const Node* node)
{
    // drzewo jest puste
    if (!node)
        return 0.;

    // przedział zawiera całe poddrzewo
    if (lb <= node->lbound && node->rbound <= rb)
        return node->minv;

    // przedział znajduje się całkowicie na lewo - ektrapoluj skrajnie lewą wartością
    if (rb < node->lbound)
        return node->leftm;

    // przedział znajduje się całkowicie na prawo - ektrapoluj skrajnie prawą wartością
    if (node->rbound < lb)
        return node->rightm;

    // przedział przecina wyłącznie lewe poddrzewo (node->left != nullptr)
    if (rb < node->frame)
        return minimum(lb, rb, node->left);

    // przedział przecina wyłącznie prawe poddrzewo (node->right != nullptr)
    if (node->frame < lb)
        return minimum(lb, rb, node->right);

    return std::min(node->value, std::min(minimum(lb, rb, node->left), minimum(lb, rb, node->right)));
}

double Node::maximum(unsigned lb, unsigned rb, const Node* node)
{
    // drzewo jest puste
    if (!node)
        return 0.;

    // przedział zawiera całe poddrzewo
    if (lb <= node->lbound && node->rbound <= rb)
        return node->maxv;

    // przedział znajduje się całkowicie na lewo - ektrapoluj skrajnie lewą wartością
    if (rb < node->lbound)
        return node->leftm;

    // przedział znajduje się całkowicie na prawo - ektrapoluj skrajnie prawą wartością
    if (node->rbound < lb)
        return node->rightm;

    // przedział przecina wyłącznie lewe poddrzewo (node->left != nullptr)
    if (rb < node->frame)
        return maximum(lb, rb, node->left);

    // przedział przecina wyłącznie prawe poddrzewo (node->right != nullptr)
    if (node->frame < lb)
        return maximum(lb, rb, node->right);

    return std::max(node->value, std::max(maximum(lb, rb, node->left), maximum(lb, rb, node->right)));
}

Tree::Tree() : root(nullptr)
{

}

Tree::~Tree()
{
    delete root;
}

void Tree::insert(unsigned frame, unsigned value)
{
    root = Node::insert(frame, value, root);
}

double Tree::minimum(unsigned lbound, unsigned rbound) const
{
    return Node::minimum(lbound, rbound, root);
}

double Tree::maximum(unsigned lbound, unsigned rbound) const
{
    return Node::maximum(lbound, rbound, root);
}

void Tree::clear()
{
    delete root;
    root = nullptr;
}
