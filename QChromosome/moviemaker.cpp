#include "moviemaker.h"

MovieMaker* MovieMaker::instance = nullptr;

#include "rendersettings.h"

#include <QSettings>
#include <QProcess>

QTextStream& operator<<(QTextStream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

QTextStream& operator<<(QTextStream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() * col.alphaF() << ">";
}

MovieMaker::MovieMaker(QObject *parent) : QThread(parent)
{

}

MovieMaker *MovieMaker::getInstance()
{
    return instance ? instance : instance = new MovieMaker;
}

void createPOVFile(QTextStream& outFile)
{
    outFile << "#version 3.7;\n"
            << "global_settings { assumed_gamma 1.0 }\n";
}

void setBackgroundColor(QTextStream& outFile, const QColor & color)
{
    outFile << "background{color " << color << "}\n";
}

void setFog(QTextStream& outFile, const QColor & color, float transmittance, const float distance)
{
    outFile << "fog{color rgbt<" << color.redF() << ", " << color.greenF() << ", " << color.blueF() << ", " << transmittance << "> distance " << distance << " }\n";
}

#include <QMessageBox>
#include <QTemporaryDir>
#include "viewport.h"
#include <QRegularExpression>
#include <QPainter>

void MovieMaker::captureScene(int fbeg, int fend, Session* session, QString suffix)
{
    if (isRunning())
    {
        QMessageBox::information(0, "QChromosome 4D Studio", "The external renderer is calculating an image."/*" Do you want to stop it?"*/);
        return;
    }

    snapshot = false; fbeg_ = fbeg; fend_ = fend; suffix_ = suffix; session_ = session;
    start();
}

#include <QDesktopServices>
#include <QUrl>

void MovieMaker::captureScene_(int fbeg, int fend, QString suffix, Session* session)
{
    QTemporaryDir dir;

    QFile ini(dir.filePath("povray.ini"));

    ini.open(QIODevice::WriteOnly);
    session->renderSettings->writeINIFile(&ini);
    ini.close();

    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + '/' + renderSettings->saveFile();
    QFile out(filename + ".pov");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    createPOVFile(outFile);

    session->currentCamera->writePOVCamera(outFile, true);

    auto& buffer = Viewport::getBuffer();
    setBackgroundColor(outFile, buffer.ucBackgroundColor);
    if (buffer.ubEnableFog) setFog(outFile, buffer.ucFogColor, 1. - buffer.ufFogStrength, buffer.ufFogDistance);

    Material::writePOVMaterials(outFile);

    session->simulation->writePOVFrames(outFile, fbeg, fend);

    outFile.flush();

    if (renderSettings->exportPOV())
    {
        ini.copy(QDir::current().filePath(renderSettings->POVfileName() + suffix + ".pov"));
        QFile::copy(filename + ".pov", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".pov"));
    }

#ifdef Q_OS_UNIX
    if (renderSettings->render())
    {
        QProcess p;
        p.setWorkingDirectory(dir.path());

        QStringList argv;
        argv << "-D"
             << "+V"
             << "+I" + renderSettings->saveFile();

        QRegularExpression re1("Rendering frame (\\d+) of (\\d+)");
        QRegularExpression re("Rendered (\\d+) of (\\d+) pixels");
        QByteArray buffer;

        int cf = 1, tf = 1;

        p.connect(&p, &QProcess::readyReadStandardError, [&] {
            buffer += p.readAllStandardError();

            int offset = 0;
            auto match1 = re1.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match1.hasMatch())
            {
                cf = match1.captured(1).toInt();
                tf = match1.captured(2).toInt();

                match1 = re1.match(buffer, offset = match1.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            auto match = re.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match.hasMatch())
            {
                int a = match.captured(1).toInt();
                int b = match.captured(2).toInt();

                emit progressChanged(100 * ((cf - 1) * b + a) / (tf * b));

                match = re.match(buffer, offset = match.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            if (match.hasPartialMatch())
                buffer = buffer.right(match.capturedLength());
            else
                buffer.clear();
        });

        p.start("povray", argv);
        p.waitForFinished(-1);
        p.disconnect();

        emit progressChanged(101);

        if (renderSettings->openFile())
            QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::current().filePath(renderSettings->saveFile() + ".avi")));
    }
#endif
}

void MovieMaker::captureScene1(Session *session, QString suffix)
{
    if (isRunning())
    {
        QMessageBox::information(nullptr, "QChromosome 4D Studio", "The external renderer is calculating an image."/*" Do you want to stop it?"*/);
        return;
    }

    snapshot = true; suffix_ = suffix; session_ = session;
    start();
}

void MovieMaker::captureScene1_(QString suffix, Session* session)
{
    QTemporaryDir dir;

    QFile ini(dir.filePath("povray.ini"));

    ini.open(QIODevice::WriteOnly);
    session->renderSettings->writeINIFile(&ini);
    ini.close();

    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + "/" + renderSettings->saveFile();
    QFile out(filename + ".pov");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    createPOVFile(outFile);

    session->currentCamera->writePOVCamera(outFile, false);

    auto& buffer = Viewport::getBuffer();
    setBackgroundColor(outFile, buffer.ucBackgroundColor);
    if (buffer.ubEnableFog) setFog(outFile, buffer.ucFogColor, 1. - buffer.ufFogStrength, buffer.ufFogDistance);

    Material::writePOVMaterials(outFile);

    session->simulation->writePOVFrame(outFile, session->projectSettings->getDocumentTime());

    outFile.flush();

    if (renderSettings->exportPOV())
    {
        ini.copy(QDir::current().filePath(renderSettings->POVfileName() + suffix + ".ini"));
        QFile::copy(filename + ".pov", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".pov"));
    }

#ifdef Q_OS_UNIX
    if (renderSettings->render())
    {
        QProcess p;
        p.setWorkingDirectory(dir.path());

        QStringList argv;
        argv << "-D"
             << "+V"
             << "+O" + QDir::current().filePath(renderSettings->saveFile() + suffix + ".png")
             << "+I" + renderSettings->saveFile() + ".pov";

        QRegularExpression re("Rendered (\\d+) of (\\d+) pixels");
        QByteArray buffer;

        p.connect(&p, &QProcess::readyReadStandardError, [&] {
            buffer += p.readAllStandardError();

            int offset = 0;
            auto match = re.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match.hasMatch())
            {
                int a = match.captured(1).toInt();
                int b = match.captured(2).toInt();

                emit progressChanged(100 * a / b);

                match = re.match(buffer, offset = match.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            if (match.hasPartialMatch())
                buffer = buffer.right(match.capturedLength());
            else
                buffer.clear();
        });

        p.start("povray", argv);
        p.waitForFinished(-1);

        emit progressChanged(101);

        if (renderSettings->openFile())
            QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::current().filePath(renderSettings->saveFile() + suffix + ".png")));
    }
#endif
}

void MovieMaker::addSphere(QTextStream& outFile, const QVector3D & position, float radius, int color)
{
    outFile << "sphere {"
            << position << ", "
            << radius << " "
            << "material { " << Material::fetch(color) << " }"
            << "}\n";
}

void MovieMaker::addCylinder(QTextStream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, int colorA, int colorB)
{
    QVector3D direction = positionB - positionA;

    outFile << "cone{"
            << " " << positionA << ", " << radiusA
            << " " << positionB << ", " << radiusB
            << " texture{gradient" << direction << " texture_map{[0.0 " << Material::fetch(colorA) << "tex][1.0 " << Material::fetch(colorB) << "tex]}"
            << " scale " << direction.length()
            << " translate " << positionA
            << "}}\n";
}

void MovieMaker::addSphere1(QTextStream& outFile, int id, float radius, int color)
{
    outFile << "sphere {"
            << "Atom" << id << "Pos(clock), "
            << radius << " "
            << "material { " << Material::fetch(color) << " }"
            << "}\n";
}

void MovieMaker::addCylinder1(QTextStream& outFile, int idA, int idB, float radiusA, float radiusB, int colorA, int colorB)
{
    outFile << "cone{"
            << " Atom" << idA << "Pos(clock), " << radiusA
            << " Atom" << idB << "Pos(clock), " << radiusB
            << " texture{gradient (Atom" << idB << "Pos(clock)-Atom" << idA << "Pos(clock)) texture_map{[0.0 " << Material::fetch(colorA) << "tex][1.0 " << Material::fetch(colorB) << "tex]}"
            << " scale vlength(Atom" << idB << "Pos(clock)-Atom" << idA << "Pos(clock))"
            << " translate Atom" << idA << "Pos(clock)"
            << "}}\n";
}

void MovieMaker::run()
{
    if (snapshot)
        captureScene1_(suffix_, session_);
    else
        captureScene_(fbeg_, fend_, suffix_, session_);
}
