#include "xvgsimulationseries.h"
#include "messagehandler.h"

XVGSimulationSeries::XVGSimulationSeries(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationSeries(name, s, f, l, t)
{
    buffer.resize(1024);

    if (cacheHeaders(b ? 0 : INT_MAX) < 0)
        qcCritical("No data was found.", file->fileName(), -1, -1);
}

XVGSimulationSeries::~XVGSimulationSeries()
{

}

#include <QRegularExpression>
#include "session.h"

void XVGSimulationSeries::skipHeader()
{
    static const QRegularExpression re("^@\\s*s(\\d+)\\b.*?\\blegend\\s*\"(.*?)\"");

    while (true)
    {
        if (!file->readLine(buffer.data(), buffer.size()))
        {
            atEnd = true;
            return;
        }

        line++;

        if (buffer[0] == '#')
            continue;

        if (buffer[0] == '@')
        {
            auto i = re.match(buffer);

            if (i.hasMatch())
            {
                auto s = new QtCharts::QLineSeries;
                s->setName(i.captured(2));

                session->chart->addSeries(s);
                legend.insert(i.captured(1).toInt(), s);
            }

            continue;
        }

        if (buffer[0] == '&')
            continue;

        atEnd = false;
        return;
    }
}

int XVGSimulationSeries::cacheHeaders(int time)
{
    static const QRegularExpression re("\\S+");

    if (atEnd)
        return j - 1;

    while (i <= first)
    {
        skipHeader();
        i++;

        if (atEnd)
            return j - 1;
    }

    while (j <= time)
    {
        auto g = re.globalMatch(buffer);
        g.next();

        for (int k = 0; g.hasNext(); k++)
        {
            auto match = g.next();

            if (legend.contains(k))
            {
                bool ok;
                qreal value = match.captured().toDouble(&ok);

                if (ok)
                    legend[k]->append(j, value);
                else
                    qcWarning("Real value is malformed.", file->fileName(), line, match.capturedStart() + 1);
            }
            else
                qcWarning("Series is undefined.", file->fileName(), line, match.capturedStart() + 1);
        }

        j++;

        for (int k = 0; k < stride; k++)
        {
            skipHeader();
            i++;

            if (i > last)
                atEnd = true;

            if (atEnd)
                return j - 1;
        }
    }

    return j - 1;
}

int XVGSimulationSeries::lastEntry() const
{
    return j - 1;
}
