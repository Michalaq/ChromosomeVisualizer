#include "commondata.h"

CommonData* CommonData::instance = new CommonData();

CommonData::CommonData()
{

}

CommonData* CommonData::getInstance()
{
    return instance;
}

QVariant CommonData::getValue(Record r) const
{
    return values[r];
}

void CommonData::setValue(Record r, const QVariant& value)
{
    values[r] = value;

    for (auto f : connections[r])
        f(value);
}

void CommonData::connect(Record r, const Functor& f)
{
    connections[r].append(f);
}
