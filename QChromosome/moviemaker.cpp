#include "moviemaker.h"

MovieMaker* MovieMaker::instance = nullptr;

const QRegularExpression MovieMaker::re1 = QRegularExpression("Rendering frame (\\d+) of (\\d+)");
const QRegularExpression MovieMaker::re2 = QRegularExpression("Rendered (\\d+) of (\\d+) pixels");

QTextStream& operator<<(QTextStream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

QTextStream& operator<<(QTextStream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() * col.alphaF() << ">";
}

MovieMaker::MovieMaker(QObject *parent) : QObject(parent)
{

}

MovieMaker::~MovieMaker()
{
    qDeleteAll(history);
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
#include "rendersettings.h"
#include "viewport.h"
#include <QDesktopServices>
#include <QUrl>

void MovieMaker::captureScene(Session* session)
{
    if (p.state() != QProcess::NotRunning)
    {
        if (QMessageBox::question(Q_NULLPTR, "QChromosome 4D Studio", "The external renderer is calculating an image. Do you want to stop it?") == QMessageBox::Yes)
            p.kill();
        else
            return;
    }

    auto tname = session->renderSettings->getTranslatorName();

    if (session->renderSettings->saveTraslator() && tname.isEmpty() && QMessageBox::question(Q_NULLPTR, "QChromosome 4D Studio", "There is no file name specified for the rendering scene. Do you want to continue without saving?") == QMessageBox::No)
        return;

    auto range = session->renderSettings->frameRange();
    bool interpolate = range.first != range.second;

    auto dir = new QTemporaryDir();
    history.append(dir);

    // write INI file
    QFile iniFile(dir->filePath("povray.ini"));
    iniFile.open(QIODevice::WriteOnly);

    QTextStream iniStream(&iniFile);
    *session->renderSettings << iniStream;

    iniFile.close();

    // write POV file
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir->path() + '/' + renderSettings->saveFile();
    QFile povFile(filename + ".pov");
    povFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream povStream(&povFile);
    createPOVFile(povStream);

    session->currentCamera->writePOVCamera(povStream, interpolate);

    auto& buffer_ = Viewport::getBuffer();
    setBackgroundColor(povStream, buffer_.ucBackgroundColor);
    if (buffer_.ubEnableFog) setFog(povStream, buffer_.ucFogColor, 1. - buffer_.ufFogStrength, buffer_.ufFogDistance);

    Material::writePOVMaterials(povStream);

    session->simulation->writePOVFrames(povStream, range.first, range.second);

    povStream.flush();
    povFile.close();

    // translator
    if (session->renderSettings->saveTraslator() && !tname.isEmpty())
    {
        iniFile.copy(session->renderSettings->getTranslatorDir().filePath(tname + ".ini"));
        povFile.copy(session->renderSettings->getTranslatorDir().filePath(tname + ".pov"));
    }

#ifdef Q_OS_UNIX
    // tracing
    if (renderSettings->render())
    {
        p.setWorkingDirectory(dir->path());

        QStringList argv;
        argv << "-D"
             << "+V"
             << "+I" + renderSettings->saveFile();

        buffer.clear();
        cf = 1; tf = 1;

        p.connect(&p, &QProcess::readyReadStandardError, [this] {
            buffer += p.readAllStandardError();

            int offset = 0;
            auto match1 = re1.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match1.hasMatch())
            {
                cf = match1.captured(1).toInt();
                tf = match1.captured(2).toInt();

                match1 = re1.match(buffer, offset = match1.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            auto match = re2.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match.hasMatch())
            {
                int a = match.captured(1).toInt();
                int b = match.captured(2).toInt();

                emit progressChanged(100 * ((cf - 1) * b + a) / (tf * b));

                match = re2.match(buffer, offset = match.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            if (match.hasPartialMatch())
                buffer = buffer.right(match.capturedLength());
            else
                buffer.clear();
        });

        p.connect(&p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this,interpolate,renderSettings,dir] {
            emit progressChanged(101);

            if (renderSettings->openFile())
                QDesktopServices::openUrl(QUrl::fromLocalFile(dir->filePath(renderSettings->saveFile() + (interpolate ? ".avi" : ".png"))));

            p.disconnect();
        });

        p.start("povray", argv);
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
