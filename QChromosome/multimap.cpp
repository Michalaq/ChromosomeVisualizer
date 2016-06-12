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
    size(0)
{

}

Node::Node(unsigned f, double v, Node* l, Node* r) :
    left(l),
    right(r),
    frame(f),
    value(v),
    lbound(l->lbound),
    rbound(f),
    minv(std::min({l->minv, r->minv, v})),
    maxv(std::max({l->maxv, r->maxv, v})),
    leftm(l->leftm),
    size(l->size + 1)
{
    Q_ASSERT(l->size == r->size);
}

Node::~Node()
{
    delete left;
    delete right;
}

double Node::minimum(unsigned lb, unsigned rb, double lm, double rm) const
{
    // przedział znajduje się całkowicie na lewo
    if (rb < lbound)
        return std::min(lm, leftm);

    // przedział znajduje się całkowicie na prawo
    if (rbound < lb)
        return std::min(value, rm);

    // przedział wystaje na lewo
    if (lb < lbound)
        return std::min(lm, minimum(lbound, rb, lm, rm));

    // przedział wystaje na prawo
    if (rbound < rb)
        return std::min(minimum(lb, rbound, lm, rm), rm);

    // przedział jest równy poddrzewu
    if (lb == lbound && rbound == rb)
        return minv;

    Q_ASSERT(left != nullptr && right != nullptr);

    // nie zahacza o prawe
    if (rb < right->lbound)
        return left->minimum(lb, rb, lm, right->leftm);

    // nie zahacza o lewe
    if (left->rbound < lb)
        return right->minimum(lb, rb, left->value, value);

    // zahacza o oba poddrzewa
    return std::min(left->minimum(lb, rb, lm, right->leftm), right->minimum(lb, rb, left->value, value));
}

double Node::maximum(unsigned lb, unsigned rb, double lm, double rm) const
{
    // przedział znajduje się całkowicie na lewo
    if (rb < lbound)
        return std::max(lm, leftm);

    // przedział znajduje się całkowicie na prawo
    if (rbound < lb)
        return std::max(value, rm);

    // przedział wystaje na lewo
    if (lb < lbound)
        return std::max(lm, maximum(lbound, rb, lm, rm));

    // przedział wystaje na prawo
    if (rbound < rb)
        return std::max(maximum(lb, rbound, lm, rm), rm);

    // przedział jest równy poddrzewu
    if (lb == lbound && rbound == rb)
        return maxv;

    Q_ASSERT(left != nullptr && right != nullptr);

    // nie zahacza o prawe
    if (rb < right->lbound)
        return left->maximum(lb, rb, lm, right->leftm);

    // nie zahacza o lewe
    if (left->rbound < lb)
        return right->maximum(lb, rb, left->value, value);

    // zahacza o oba poddrzewa
    return std::max(left->maximum(lb, rb, lm, right->leftm), right->maximum(lb, rb, left->value, value));
}

Nodes::Nodes() : visible(true)
{

}

Nodes::~Nodes()
{
    qDeleteAll(nodes);
}

void Nodes::insert(unsigned frame, unsigned value)
{
    if (nodes.count() > 1 && nodes.at(0)->size == nodes.at(1)->size)
    {
        Node *right = nodes.takeFirst();
        Node *left = nodes.takeFirst();

        nodes.prepend(new Node(frame, value, left, right));
    }
    else
        nodes.prepend(new Node(frame, value));
}

double Nodes::maximum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::min();

    double leftm = nodes.last()->leftm;
    double rightm = nodes.first()->value;

    auto i = nodes.cbegin();

    while (i != nodes.cend() && (*i)->lbound >= rbound)
    {
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to najdalszy przedział, którego lewy koniec jest przed prawym końcem zakresu

    while (i != nodes.cend() && (*i)->lbound >= lbound)
    {
        ans = std::max((*i)->maximum(lbound, rbound, (*i)->leftm, rightm), ans);
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to pierwszy przedział, którego lewy koniec jest mniejszy od lewego końca zakresu

    if (i != nodes.cend())
        return std::max((*i)->maximum(lbound, rbound, NAN, rightm), ans);
    else
        return std::max(leftm, ans);
}

double Nodes::minimum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::max();

    double leftm = nodes.last()->leftm;
    double rightm = nodes.first()->value;

    auto i = nodes.cbegin();

    while (i != nodes.cend() && (*i)->lbound >= rbound)
    {
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to najdalszy przedział, którego lewy koniec jest przed prawym końcem zakresu

    while (i!=nodes.cend()&& (*i)->lbound >= lbound)
    {
        ans = std::min((*i)->minimum(lbound, rbound, (*i)->leftm, rightm), ans);
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to pierwszy przedział, którego lewy koniec jest mniejszy od lewego końca zakresu

    if (i != nodes.cend())
        return std::min((*i)->minimum(lbound, rbound, NAN, rightm), ans);
    else
        return std::min(leftm, ans);
}

Tree::Tree()
{

}

void Tree::insert(std::string fname, unsigned frame, unsigned value)
{
    roots[fname].insert(frame, value);
}

double Tree::minimum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::max();

    for (auto& i : roots)
        if (i.visible)
            ans = std::min(i.minimum(lbound, rbound), ans);

    return ans;
}

double Tree::maximum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::min();

    for (auto& i : roots)
        if (i.visible)
            ans = std::max(i.maximum(lbound, rbound), ans);

    return ans;
}

void Tree::clear()
{
    roots.clear();
}

bool Tree::empty() const
{
    for (auto& i : roots)
        if (i.visible) return false;

    return true;
}

void Tree::setVisible(const std::string &fname, bool visible)
{
    roots[fname].visible = visible;
}
