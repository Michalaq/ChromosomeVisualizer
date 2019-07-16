#include "xvgsimulationseries.h"
#include "messagehandler.h"
#include "session.h"

XVGSimulationSeries::XVGSimulationSeries(const QString& name, Session* s, int f, int l, int t, bool b) :
    SimulationSeries(name, s, f, l, t)
{
    buffer.resize(1024);

    if (cacheHeaders(b ? 0 : INT_MAX) < 0)
        throw MessageLog({QtCriticalMsg, "No data was found.", file->fileName(), nullptr, -1, -1});

    if (!b)
    {
        int maxTime = s->previewRange ? s->projectSettings->getPreviewMaxTime() : s->projectSettings->getMaximumTime();

        if (maxTime == s->projectSettings->getPreviewMaxTime() && maxTime < j)
            session->setLastFrame(j);
    }
}

XVGSimulationSeries::~XVGSimulationSeries()
{

}

void XVGSimulationSeries::readEntry(int time, char*, std::size_t, std::size_t)
{
    cacheHeaders(time);
}

#include <QRegularExpression>
#include "session.h"

bool XVGSimulationSeries::skipHeader()
{
    static const QRegularExpression re("^@\\s*s(\\d+)\\b.*?\\blegend\\s*\"(.*?)\"");

    if (i > last)
        goto finished;

    while (true)
    {
        if (!file->readLine(buffer.data(), buffer.size()))
            goto finished;

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

                new SeriesItem(s, model);

                session->chart->addSeries(s);
                legend.insert(i.captured(1).toInt(), s);
            }

            continue;
        }

        if (buffer[0] == '&')
            continue;

        i++;

        atEnd = false;
        return true;
    }

    finished:
    session->finished(file->fileName(), j);

    atEnd = true;
    return false;
}

int XVGSimulationSeries::cacheHeaders(int time)
{
    static const QRegularExpression re("\\S+");

    if (atEnd)
        return j;

    while (i < first)
        if (!skipHeader())
            return j;

    while (j < time)
        if (skipHeader())
        {
            j++;

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

            for (int k = 1; k < stride; k++)
                if (!skipHeader())
                    return j;
        }
        else
            return j;

    return j;
}

int XVGSimulationSeries::lastEntry() const
{
    return j;
}

void XVGSimulationSeries::remove()
{
    session->simulation->removeOne(this);

    for (auto series : legend)
    {
        session->chart->removeSeries(series);
        session->series.remove(series);
    }

    session->setLastFrame(session->simulation->lastEntry());

    session->plot->updateSimulation();
}
