#ifndef COMMONDATA_H
#define COMMONDATA_H

#include <QtCore>

enum Record
{
    Camera_X,
    Camera_Y,
    Camera_Z,
    Camera_H,
    Camera_P,
    Camera_B,
    Camera_Focal_Length,
    Camera_Aperture_Width,
    Camera_Field_Of_View
};

class CommonData
{
public:
    typedef std::function<void(QVariant)> Functor;

    static CommonData* getInstance();

    QVariant getValue(Record r) const;
    void setValue(Record r, const QVariant& value);

    void connect(Record r, const Functor& f);

private:
    explicit CommonData();

    static CommonData* instance;

    QMap<Record, QVariant> values;
    QMap<Record, QList<Functor>> connections;
};

#endif // COMMONDATA_H
