#ifndef MULTIMAP_H
#define MULTIMAP_H


class Node
{
public:
    Node(unsigned f, double v);
    Node(unsigned f, double v, Node* l);
    ~Node();

    static Node* insert(unsigned f, double v, Node* node);

    static double minimum(unsigned lb, unsigned rb, const Node* node, double lm, double rm);
    static double maximum(unsigned lb, unsigned rb, const Node* node, double lm, double rm);

private:
    // lewe i prawe poddrzewo
    Node *left, *right;

    // ramka i wartość funkcji
    unsigned frame;
    double value;

    // końce reprezentowanego przedziału ramek
    unsigned lbound, rbound;

    // maksymalna i minimalna wartość w poddrzewie
    double minv, maxv;

    // rozmiary lewego i prawego poddrzewa
    unsigned lsize, rsize;
};

#include <QMap>

class Tree
{
public:
    Tree();
    ~Tree();

    void insert(std::string fname, unsigned frame, unsigned value);

    double minimum(unsigned lbound, unsigned rbound) const;
    double maximum(unsigned lbound, unsigned rbound) const;

    void clear();

private:
    QMap<std::string, Node*> roots;
};

#endif // MULTIMAP_H
