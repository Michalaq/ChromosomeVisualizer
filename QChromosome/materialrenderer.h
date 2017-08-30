#ifndef MATERIALRENDERER_H
#define MATERIALRENDERER_H

#include <QPainter>

class Material;

class MaterialRenderer
{
public:
    static MaterialRenderer* getInstance();
    ~MaterialRenderer();

    void paint(QPainter *painter, QRect bounds, const Material* material);

signals:

public slots:

private:
    explicit MaterialRenderer();

    static MaterialRenderer* instance;
};

#endif // MATERIALRENDERER_H
