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
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() << ">";
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

#include <QMessageBox>
#include <QTemporaryDir>
#include "viewport.h"
#include <QDesktopServices>
#include <QUrl>

void MovieMaker::captureScene(Session* session)
{
    if (p.state() != QProcess::NotRunning)
    {
        if (QMessageBox::question(Q_NULLPTR, "QChromosome 4D Studio", "The external renderer is calculating an image. Do you want to stop it?") == QMessageBox::Yes)
        {
            p.disconnect();
            p.kill();
            p.waitForFinished(-1);
        }
        else
            return;
    }

    auto range = session->renderSettings->frameRange();
    bool interpolate = range.first != range.second;

    auto oname = session->renderSettings->getOutputName();
    auto tname = session->renderSettings->getTranslatorName();

    if (((oname.isEmpty() && session->renderSettings->saveOutput() && interpolate) || (tname.isEmpty() && session->renderSettings->saveTraslator())) && QMessageBox::question(Q_NULLPTR, "QChromosome 4D Studio", "There is no file name specified for the rendered image. Do you want to continue without saving?") == QMessageBox::No)
        return;

    auto dir = new QTemporaryDir();
    history.append(dir);

    // write INI file
    QFile iniFile(dir->filePath("povray.ini"));
    iniFile.open(QIODevice::WriteOnly);

    QTextStream iniStream(&iniFile);
    *session->renderSettings << iniStream;

    iniFile.close();

    // write POV file
    QFile povFile(dir->filePath("scene.pov"));
    povFile.open(QIODevice::WriteOnly);

    QTextStream povStream(&povFile);

    povStream << "#version 3.7;\n"
              << "global_settings { assumed_gamma 1.0 }\n";

    session->currentCamera->writePOVCamera(povStream, interpolate);

    const auto& viewport = *session->viewportUniformBuffer.constBegin();

    povStream << "background{color " << QColor(viewport.ucBackgroundColor) << "}\n";

    if (viewport.ubEnableFog)
    {
        QColor color = viewport.ucFogColor;
        color.setAlphaF(viewport.ufFogStrength);

        povStream << "fog{color " << color << " distance " << viewport.ufFogDistance << " }\n";
    }

    Material::writePOVMaterials(povStream);

    session->simulation->writePOVFrames(povStream, range.first, range.second);

    povFile.close();

    // translator
    if (session->renderSettings->saveTraslator() && !tname.isEmpty())
    {
        QFileInfo ini(session->renderSettings->getTranslatorDir().filePath(tname + ".ini"));

        if (!ini.exists() || (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you really want to overwrite file\n%1 ?").arg(ini.filePath()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes && QFile::remove(ini.filePath())))
            if (!iniFile.copy(ini.filePath()))
                QMessageBox::critical(Q_NULLPTR, "QChromosome 4D Studio", "Files cannot be written - please check output paths!");

        QFileInfo pov(session->renderSettings->getTranslatorDir().filePath(tname + ".pov"));

        if (!pov.exists() || (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you really want to overwrite file\n%1 ?").arg(pov.filePath()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes && QFile::remove(pov.filePath())))
            if (!povFile.copy(pov.filePath()))
                QMessageBox::critical(Q_NULLPTR, "QChromosome 4D Studio", "Files cannot be written - please check output paths!");
    }

#ifdef Q_OS_UNIX
    // tracing
    if (session->renderSettings->render())
    {
        p.setWorkingDirectory(dir->path());

        QStringList argv;
        argv << "-D"
             << "+V"
             << "+Iscene";

        buffer.clear();
        cf = 1; tf = 1;

        p.connect(&p, &QProcess::readyReadStandardError, [this] {
            buffer += p.readAllStandardError();

            int offset = 0;
            auto match1 = re1.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match1.hasMatch())
            {
                cf = match1.captured(1).toUInt();
                tf = match1.captured(2).toUInt();

                match1 = re1.match(buffer, offset = match1.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            auto match = re2.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match.hasMatch())
            {
                unsigned a = match.captured(1).toUInt();
                unsigned b = match.captured(2).toUInt();

                emit progressChanged(100 * ((cf - 1) * b + a) / (tf * b));

                match = re2.match(buffer, offset = match.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            if (match.hasPartialMatch())
                buffer = buffer.right(match.capturedLength());
            else
                buffer.clear();
        });

        p.connect(&p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this,interpolate,dir,session,oname] {
            p.disconnect();

            QStringList argv;
            session->renderSettings->getFFmpegArgs(argv);

            p.start("ffmpeg", argv);
            p.waitForFinished(-1);

            emit progressChanged(101);

            if (session->renderSettings->saveOutput() && !oname.isEmpty())
            {
                QFileInfo out(session->renderSettings->getOutputDir().filePath(oname + "." + (interpolate ? "avi" : session->renderSettings->getExtension())));

                if (!out.exists() || (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you really want to overwrite file\n%1 ?").arg(out.filePath()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes && QFile::remove(out.filePath())))
                    if (!QFile::copy(dir->filePath(QString("scene.") + (interpolate ? "avi" : session->renderSettings->getExtension())), out.filePath()))
                        QMessageBox::critical(Q_NULLPTR, "QChromosome 4D Studio", "Files cannot be written - please check output paths!");
            }

            if (session->renderSettings->openFile())
                QDesktopServices::openUrl(QUrl::fromLocalFile(dir->filePath(QString("scene.") + (interpolate ? "avi" : session->renderSettings->getExtension()))));
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
