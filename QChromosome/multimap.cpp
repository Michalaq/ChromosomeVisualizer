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

        node->rsize++;

        return node;
    }

    return new Node(f, v, node);
}

double Node::minimum(unsigned lb, unsigned rb, const Node* node, double lm, double rm)
{
    // przedział znajduje się całkowicie na lewo
    if (rb < node->lbound)
        return lm;

    // przedział znajduje się całkowicie na prawo
    if (node->rbound < lb)
        return node->value;

    // przedział jest lewostronnie dłuższy
    if (lb < node->lbound)
        return std::min(lm, minimum(node->lbound, rb, node, lm, rm));

    // przedział jest prawostronnie dłuższy
    if (node->rbound < rb)
        return std::min(rm, minimum(lb, node->rbound, node, lm, rm));

    // przedział zawiera się w poddrzewie

    // przedział jest równy poddrzewu
    if (lb == node->lbound && node->rbound == rb)
        return node->minv;

    // przedział przecina wyłącznie lewe poddrzewo (node->left != nullptr)
    if (rb < node->frame)
        return minimum(lb, rb, node->left, lm, node->value);

    // przedział przecina wyłącznie prawe poddrzewo (node->right != nullptr)
    if (node->frame < lb)
        return minimum(lb, rb, node->right, node->value, rm);

    return std::min(minimum(lb, rb, node->left, lm, node->value), minimum(lb, rb, node->right, node->value, rm));
}

double Node::maximum(unsigned lb, unsigned rb, const Node* node, double lm, double rm)
{
    // przedział znajduje się całkowicie na lewo
    if (rb < node->lbound)
        return lm;

    // przedział znajduje się całkowicie na prawo
    if (node->rbound < lb)
        return node->value;

    // przedział jest lewostronnie dłuższy
    if (lb < node->lbound)
        return std::max(lm, maximum(node->lbound, rb, node, lm, rm));

    // przedział jest prawostronnie dłuższy
    if (node->rbound < rb)
        return std::max(rm, maximum(lb, node->rbound, node, lm, rm));

    // przedział zawiera się w poddrzewie

    // przedział jest równy poddrzewu
    if (lb == node->lbound && node->rbound == rb)
        return node->maxv;

    // przedział przecina wyłącznie lewe poddrzewo (node->left != nullptr)
    if (rb < node->frame)
        return maximum(lb, rb, node->left, lm, node->value);

    // przedział przecina wyłącznie prawe poddrzewo (node->right != nullptr)
    if (node->frame < lb)
        return maximum(lb, rb, node->right, node->value, rm);

    return std::max(maximum(lb, rb, node->left, lm, node->value), maximum(lb, rb, node->right, node->value, rm));
}

Tree::Tree()
{

}

Tree::~Tree()
{
    qDeleteAll(roots);
}

void Tree::insert(std::string fname, unsigned frame, unsigned value)
{
    auto i = roots.find(fname);

    if (i != roots.end())
        i.value() = Node::insert(frame, value, i.value());
    else
        roots.insert(fname, Node::insert(frame, value, nullptr));
}

double Tree::minimum(unsigned lbound, unsigned rbound) const
{
    double ans = INFINITY;

    for (auto node : roots)
        ans = std::min(Node::minimum(lbound, rbound, node, INFINITY, INFINITY), ans);

    return ans;
}

double Tree::maximum(unsigned lbound, unsigned rbound) const
{
    double ans = -INFINITY;

    for (auto node : roots)
        ans = std::max(Node::maximum(lbound, rbound, node, -INFINITY, -INFINITY), ans);

    return ans;
}

void Tree::clear()
{
    qDeleteAll(roots);
    roots.clear();
}
