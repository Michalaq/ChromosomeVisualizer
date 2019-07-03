#include "moviemaker.h"

MovieMaker* MovieMaker::instance = nullptr;

const QRegularExpression re1 = QRegularExpression("Rendering frame (\\d+) of (\\d+)");
const QRegularExpression re2 = QRegularExpression("Rendered (\\d+) of (\\d+) pixels");
const QRegularExpression re3 = QRegularExpression("^File:\\s+(.*?)\\s+Line:\\s+(\\d+)\n$");

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
#include "messagehandler.h"

void MovieMaker::captureScene(Session* session)
{
    if (!stopRenderer())
        return;

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

    povStream << "#version " << (session->currentCamera->getMode() == Camera::CM_Mono ? "3.7" : "3.8") << ";\n"
              << "global_settings { assumed_gamma 1.0 }\n";

    session->currentCamera->writePOVCamera(povStream, interpolate);

    const auto& viewport = session->viewportUniformBuffer[0];

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
            if (!ini.dir().mkpath(".") || !iniFile.copy(ini.filePath()))
                qcCritical("Files cannot be written - please check output paths!", ini.filePath(), -1, -1);

        QFileInfo pov(session->renderSettings->getTranslatorDir().filePath(tname + ".pov"));

        if (!pov.exists() || (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you really want to overwrite file\n%1 ?").arg(pov.filePath()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes && QFile::remove(pov.filePath())))
            if (!pov.dir().mkpath(".") || !povFile.copy(pov.filePath()))
                qcCritical("Files cannot be written - please check output paths!", pov.filePath(), -1, -1);
    }

    // tracing
    if (session->renderSettings->render())
    {
        p.waitForFinished(-1);
        p.setWorkingDirectory(dir->path());

        QStringList argv;
        argv << "-D"
             << "+V"
             << "-GA"
             << "Fatal_File=true"
             << "povray.ini"
             << "scene.pov";

        buffer.clear();

        p.connect(&p, &QProcess::readyReadStandardError, [this] {
            buffer += p.readAllStandardError();

            int offset = 0;
            auto match1 = re1.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match1.hasMatch())
            {
                emit frameChanged(match1.captured(1).toInt(), match1.captured(2).toInt());
                match1 = re1.match(buffer, offset = match1.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            auto match = re2.match(buffer, offset, QRegularExpression::PartialPreferFirstMatch);

            while (match.hasMatch())
            {
                emit progressChanged(100 * match.captured(1).toInt() / match.captured(2).toInt());
                match = re2.match(buffer, offset = match.capturedEnd(), QRegularExpression::PartialPreferFirstMatch);
            }

            if (match.hasPartialMatch())
                buffer = buffer.right(match.capturedLength());
            else
                buffer.clear();
        });

        p.connect(&p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, interpolate, dir, session, oname] {
            emit finished();

            p.disconnect();

            if (p.exitCode())
            {
                QFile fatal(dir->filePath("fatal.out"));
                fatal.open(QIODevice::ReadOnly);

                auto match = re3.globalMatch(fatal.readLine()).next();
                fatal.readLine();
                fatal.readLine();

                qcCritical(fatal.readAll().simplified(), match.captured(1), match.captured(2).toInt(), -1);

                fatal.close();
                return;
            }

            if (interpolate)
            {
                QStringList argv;
                session->renderSettings->getFFmpegArgs(argv);

                argv << "-loglevel" << "error";

                p.start("ffmpeg", argv);
                p.waitForFinished(-1);

                if (p.exitCode())
                {
                    auto messages = p.readAllStandardError().split('\n');
                    messages.removeLast();

                    for (const auto& message : messages)
                        qcCritical(message, "", -1, -1);

                    return;
                }
            }

            if (session->renderSettings->saveOutput() && !oname.isEmpty())
            {
                QFileInfo out(session->renderSettings->getOutputDir().filePath(oname + "." + (interpolate ? "avi" : session->renderSettings->getExtension())));

                if (!out.exists() || (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you really want to overwrite file\n%1 ?").arg(out.filePath()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes && QFile::remove(out.filePath())))
                    if (!out.dir().mkpath(".") || !QFile::copy(dir->filePath(QString("scene.") + (interpolate ? "avi" : session->renderSettings->getExtension())), out.filePath()))
                        qcCritical("Files cannot be written - please check output paths!", out.filePath(), -1, -1);
            }

            if (session->renderSettings->openFile())
                QDesktopServices::openUrl(QUrl::fromLocalFile(dir->filePath(QString("scene.") + (interpolate ? "avi" : session->renderSettings->getExtension()))));
        });

        connect(&p, &QProcess::errorOccurred, [this](QProcess::ProcessError) {
            qcCritical(p.errorString(), "", -1, -1);

            emit finished();

            p.disconnect();
        });

#if defined(Q_OS_WIN)
        p.start("pvengine", argv);
#elif defined(Q_OS_UNIX)
        p.start("povray", argv);
#endif
    }
}

bool MovieMaker::stopRenderer()
{
    if (p.state() == QProcess::NotRunning)
        return true;

    if (QMessageBox::question(Q_NULLPTR, "QChromosome 4D Studio", "The external renderer is calculating an image. Do you want to stop it?") == QMessageBox::Yes)
    {
        emit finished();

        p.disconnect();
        p.kill();

        return true;
    }

    return false;
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
