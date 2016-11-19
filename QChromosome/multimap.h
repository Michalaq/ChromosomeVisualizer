#ifndef MULTIMAP_H
#define MULTIMAP_H

// klatka kluczowa
struct Keyframe
{
    unsigned frame;
    double value;
};

// wierzchołek pełnego drzewa binarnego
class Node
{
public:
    Node(unsigned f, double v);
    Node(unsigned f, double v, Node* r, Node* l);
    ~Node();

    double minimum(unsigned lb, unsigned rb, Keyframe lm, Keyframe rm) const;
    double maximum(unsigned lb, unsigned rb, Keyframe lm, Keyframe rm) const;

private:
    // lewe i prawe poddrzewo
    Node *left, *right;

    // ramka i wartość funkcji
    Keyframe rightm;

    // końce reprezentowanego przedziału ramek
    unsigned lbound, rbound;

    // maksymalna i minimalna wartość w poddrzewie
    double minv, maxv;

    // skrajnie lewy wierzchołek w poddrzewie
    Keyframe leftm;

    // głębokość poddrzewa
    unsigned size;

friend class Nodes;
};

#include <QList>

class Nodes
{
public:
    Nodes();
    ~Nodes();

    void insert(unsigned frame, unsigned value);

    double minimum(unsigned lbound, unsigned rbound) const;
    double maximum(unsigned lbound, unsigned rbound) const;

    bool visible;

private:
    QList<Node*> nodes;
};

#include <QMap>

class Tree
{
public:
    Tree();

    void insert(std::string fname, unsigned frame, unsigned value);

    double minimum(unsigned lbound, unsigned rbound) const;
    double maximum(unsigned lbound, unsigned rbound) const;

    void setVisible(const std::string& fname, bool visible);

    bool empty() const;

    void clear();

private:
    QMap<std::string, Nodes> roots;
};

#endif // MULTIMAP_H
