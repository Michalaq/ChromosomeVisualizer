#include "multimap.h"
#include <algorithm>

Node::Node(unsigned f, double v) :
    left(nullptr),
    right(nullptr),
    data({f, v}),
    lbound(f),
    rbound(f),
    minv(v),
    maxv(v),
    leftm({f, v}),
    size(0)
{

}

Node::Node(unsigned f, double v, Node* r, Node* l) :
    left(l),
    right(r),
    data({f, v}),
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

double Node::minimum(unsigned lb, unsigned rb, Keyframe lm, Keyframe rm) const
{
    // przedział znajduje się całkowicie na lewo
    if (rb < lbound)
        return std::min(lm.value, leftm.value);

    // przedział znajduje się całkowicie na prawo
    if (rbound < lb)
        return std::min(data.value, rm.value);

    // przedział jest lewostronnie dłuższy
    if (lb < lbound)
        return std::min(lm.value, minimum(lbound, rb, lm, rm));

    // przedział jest prawostronnie dłuższy
    if (rbound < rb)
        return std::min(minimum(lb, rbound, lm, rm), rm.value);

    // przedział jest równy zakresowi poddrzewa
    if (lb == lbound && rbound == rb)
        return minv;

    Q_ASSERT(left != nullptr && right != nullptr);

    // przedział jest rozłączny z prawym poddrzewem
    if (rb < right->lbound)
        return left->minimum(lb, rb, lm, right->leftm);

    // przedział jest rozłączny z lewym poddrzewem
    if (left->rbound < lb)
        return right->minimum(lb, rb, left->data, data);

    // przedział ma część wspólną z każdym poddrzewem
    return std::min(left->minimum(lb, rb, lm, right->leftm), right->minimum(lb, rb, left->data, data));
}

double Node::maximum(unsigned lb, unsigned rb, Keyframe lm, Keyframe rm) const
{
    // przedział znajduje się całkowicie na lewo
    if (rb < lbound)
        return std::max(lm.value, leftm.value);

    // przedział znajduje się całkowicie na prawo
    if (rbound < lb)
        return std::max(data.value, rm.value);

    // przedział jest lewostronnie dłuższy
    if (lb < lbound)
        return std::max(lm.value, maximum(lbound, rb, lm, rm));

    // przedział jest prawostronnie dłuższy
    if (rbound < rb)
        return std::max(maximum(lb, rbound, lm, rm), rm.value);

    // przedział jest równy zakresowi poddrzewa
    if (lb == lbound && rbound == rb)
        return maxv;

    Q_ASSERT(left != nullptr && right != nullptr);

    // przedział jest rozłączny z prawym poddrzewem
    if (rb < right->lbound)
        return left->maximum(lb, rb, lm, right->leftm);

    // przedział jest rozłączny z lewym poddrzewem
    if (left->rbound < lb)
        return right->maximum(lb, rb, left->data, data);

    // przedział ma część wspólną z każdym poddrzewem
    return std::max(left->maximum(lb, rb, lm, right->leftm), right->maximum(lb, rb, left->data, data));
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
        nodes.prepend(new Node(frame, value, nodes.takeFirst(), nodes.takeFirst()));
    else
        nodes.prepend(new Node(frame, value));
}

double Nodes::maximum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::min();

    Keyframe leftm = nodes.last()->leftm;
    Keyframe rightm = nodes.first()->data;

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
        return std::max((*i)->maximum(lbound, rbound, {0, NAN}, rightm), ans);
    else
        return std::max(leftm.value, ans);
}

double Nodes::minimum(unsigned lbound, unsigned rbound) const
{
    double ans = std::numeric_limits<double>::max();

    Keyframe leftm = nodes.last()->leftm;
    Keyframe rightm = nodes.first()->data;

    auto i = nodes.cbegin();

    while (i != nodes.cend() && (*i)->lbound >= rbound)
    {
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to najdalszy przedział, którego lewy koniec jest przed prawym końcem zakresu

    while (i != nodes.cend() && (*i)->lbound >= lbound)
    {
        ans = std::min((*i)->minimum(lbound, rbound, (*i)->leftm, rightm), ans);
        rightm = (*i)->leftm;
        i++;
    }

    // teraz i to pierwszy przedział, którego lewy koniec jest mniejszy od lewego końca zakresu

    if (i != nodes.cend())
        return std::min((*i)->minimum(lbound, rbound, {0, NAN}, rightm), ans);
    else
        return std::min(leftm.value, ans);
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
