#include "moviemaker.h"

#include "rendersettings.h"

#include <QSettings>
#include <QProcess>

std::ostream& operator<<(std::ostream& out, const QVector3D & vec)
{
    return out << "<" << -vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

std::ostream& operator<<(std::ostream& out, const QColor & col)
{
    return out << "rgbt<" << col.redF() << ", " << col.greenF() << ", " << col.blueF() << ", " << 1. - col.alphaF() * col.alphaF() << ">";
}

void prepareINIFile(const QString& filename, const RenderSettings * renderSettings)
{
    std::ofstream outFile((filename + ".ini").toUtf8().constData());
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height()
            << "\nQuality=" << renderSettings->quality().toStdString();
    if (renderSettings->antiAliasing())
    {
        outFile << "\nAntialias=on"
                << "\nSampling_Method=" << renderSettings->aaSamplingMethod().toStdString()
                << "\nAntialias_Threshold=" << renderSettings->aaThreshold().toStdString()
                << "\nAntialias_Depth=" << renderSettings->aaDepth().toStdString();
        if (renderSettings->aaJitter())
        {
            outFile << "\nJitter=on"
                    << "\nJitter_Amount=" << renderSettings->aaJitterAmount().toStdString();
        }
        else
        {
            outFile << "\nJitter=off";
        }
    }
    else
    {
        outFile << "\nAntialias=off";
    }
}

void prepareINIFile1(const QString& filename, const RenderSettings * renderSettings, int fbeg, int fend)
{
    std::ofstream outFile((filename + ".ini").toUtf8().constData());
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height()
            << "\nQuality=" << renderSettings->quality().toStdString();
    if (renderSettings->antiAliasing())
    {
        outFile << "\nAntialias=on"
                << "\nSampling_Method=" << renderSettings->aaSamplingMethod().toStdString()
                << "\nAntialias_Threshold=" << renderSettings->aaThreshold().toStdString()
                << "\nAntialias_Depth=" << renderSettings->aaDepth().toStdString();
        if (renderSettings->aaJitter())
        {
            outFile << "\nJitter=on"
                    << "\nJitter_Amount=" << renderSettings->aaJitterAmount().toStdString();
        }
        else
        {
            outFile << "\nJitter=off";
        }
    }
    else
    {
        outFile << "\nAntialias=off";
    }
    outFile << "\nInitial_Frame=" << 0
            << "\nFinal_Frame=" << (fend - fbeg) * renderSettings->framerate()
            << "\nInitial_Clock=" << fbeg
            << "\nFinal_Clock=" << fend;
}

void createPOVFile(std::ofstream& outFile, std::string filename)
{
    outFile.open(filename + ".pov");
    outFile << "#version 3.7;\n"
            << "global_settings { assumed_gamma 1.0 }\n";
}

void setCamera(std::ofstream& outFile, const Camera* camera, QSize size)
{
    outFile << "camera{perspective\n"
            << "right x*" << size.width() << "/" << size.height() << "\n"
            << "look_at -z\n"
            << "angle " << camera->angle() << "\n"
            << "rotate " << -camera->EulerAngles() << "\n"
            << "translate " << camera->position() << "\n"
            << "}\n"
            << "\n";

    outFile << "light_source {\n"
            << QVector3D() << "," << QColor(Qt::white) << "\n"
            << "parallel\n"
            << "point_at " << -QVector3D(1., 1., 2.) << "\n"
            << "}\n";
}

void setCamera1(std::ofstream& outFile, const Camera* camera, QSize size, bool s)
{
    outFile << "camera{perspective\n"
            << "right x*" << size.width() << "/" << size.height() << "\n"
            << "look_at -z\n"
            << "angle " << camera->angle() << "\n";

    if (s)
        outFile << "rotate " << -camera->EulerAngles() << "\n"
            << "translate " << camera->position() << "\n";
    else
        outFile << "rotate -MySplineAng(clock)\n"
            << "translate MySplinePos(clock)\n";

    outFile << "}\n"
            << "\n";

    outFile << "light_source {\n"
            << QVector3D() << "," << QColor(Qt::white) << "\n"
            << "parallel\n"
            << "point_at " << -QVector3D(1., 1., 2.) << "\n"
            << "}\n";
}

void set360Camera(std::ofstream& outFile, const Camera* camera, QSize size)
{
    outFile << "camera{spherical \n"
            << "right x*" << size.width() << "/" << size.height() << "\n"
            << "location " << camera->position() << "\n"
            << "look_at " << camera->lookAt() << "\n"
            << "}\n"
            << "\n";

    outFile << "light_source {" << camera->position() << " " << "color " << QColor(Qt::white) << "}\n"
            << "\n";
}

void setBackgroundColor(std::ofstream& outFile, const QColor & color)
{
    outFile << "background{color " << color << "}\n";
}

void setFog(std::ofstream& outFile, const QColor & color, const float distance)
{
    outFile << "fog{color " << color << " distance " << distance << " }\n";
}

void addLabel(std::ofstream& outFile, const QString & label)
{

}

void MovieMaker::captureScene(int fbeg, int fend, const VizWidget* scene, const Camera* camera, const Interpolator& ip, QString suffix, int fr)
{
    QTemporaryDir dir;
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + '/' + renderSettings->saveFile();
    prepareINIFile1(filename, renderSettings, fbeg, fend);
    std::ofstream outFile;
    createPOVFile(outFile, filename.toStdString());

    if (!ip.keys().isEmpty())
        outFile << ip;

    if (renderSettings->cam360())
        set360Camera(outFile, camera, renderSettings->outputSize());
    else
        setCamera1(outFile, camera, renderSettings->outputSize(), ip.keys().isEmpty());
    setBackgroundColor(outFile, scene->backgroundColor());
    setFog(outFile, scene->backgroundColor(), 1.f / scene->fogDensity()); //TODO: dobre rownanie dla ostatniego argumentu

    scene->writePOVFrames(outFile, fbeg, fend);

    for (auto & key : scene->getLabels().keys())
        addLabel(outFile, "");

    outFile.flush();

    if (renderSettings->exportPOV())
    {
        QFile::copy(filename + ".ini", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".ini"));
        QFile::copy(filename + ".pov", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".pov"));
    }

#ifdef __linux__
    if (renderSettings->render())
    {
        QProcess p;
        p.setWorkingDirectory(dir.path());

        QStringList argv;
        argv << renderSettings->saveFile() + ".ini"
             << "-D"
             << "+V"
             << renderSettings->saveFile() + ".pov";

        p.start("povray", argv);
        p.waitForFinished(-1);

        int total = (fend - fbeg) * renderSettings->framerate();
        int fw1 = QString::number(total).length();
        int fw2 = QString::number(fend).length();

        QImage img;

        QFont f;
        f.setFamily("RobotoMono");
        f.setPixelSize(15);

        QTransform t;
        t.translate(renderSettings->outputSize().width(), 0);
        t.scale(qreal(renderSettings->outputSize().height()) / 240, qreal(renderSettings->outputSize().height()) / 240);

        for (int i = 0; i <= total; i++)
        {
            QFile file(QString("%1%2.png").arg(filename).arg(i, fw1, 10, QChar('0')));

            file.open(QIODevice::ReadOnly);
            img.load(&file, "PNG");
            file.close();

            QPainter p(&img);
            p.setRenderHint(QPainter::Antialiasing);
            p.setPen(Qt::white);
            p.setFont(f);
            p.setTransform(t);

            p.drawText(QRect(-320, 0, 320, 240), QString("Frame %1/%2\nTime %3").arg(i, fw1, 10, QChar('0')).arg(total).arg(fbeg + i / renderSettings->framerate(), fw2, 10, QChar('0')), Qt::AlignRight | Qt::AlignTop);

            file.open(QIODevice::WriteOnly);
            img.save(&file, "PNG");
            file.close();
        }

        fr *= renderSettings->framerate();

        argv.clear();
        argv << "-y"
             << "-framerate" << QString::number(fr)
             << "-i" << renderSettings->saveFile() + "%0" + QString::number(fw1) + "d.png"
             << "-c:v" << "libx264"
             << "-r" << QString::number(fr)
             << "-pix_fmt" << "yuv420p"
             << "file:" + QDir::current().filePath(renderSettings->saveFile() + suffix + ".mp4");

        p.start("ffmpeg", argv);
        p.waitForFinished(-1);
    }
#endif
}

void MovieMaker::captureScene1(int fn, const VizWidget* scene, const Camera* camera, QString suffix)
{
    QTemporaryDir dir;
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + "/" + renderSettings->saveFile();
    prepareINIFile(filename, renderSettings);
    std::ofstream outFile;
    createPOVFile(outFile, filename.toStdString());

    if (renderSettings->cam360())
        set360Camera(outFile, camera, renderSettings->outputSize());
    else
        setCamera(outFile, camera, renderSettings->outputSize());
    setBackgroundColor(outFile, scene->backgroundColor());
    setFog(outFile, scene->backgroundColor(), 1.f / scene->fogDensity()); //TODO: dobre rownanie dla ostatniego argumentu

    scene->writePOVFrame(outFile, fn);

    for (auto & key : scene->getLabels().keys())
        addLabel(outFile, "");

    outFile.flush();

    if (renderSettings->exportPOV())
    {
        QFile::copy(filename + ".ini", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".ini"));
        QFile::copy(filename + ".pov", QDir::current().filePath(renderSettings->POVfileName() + suffix + ".pov"));
    }

#ifdef __linux__
    if (renderSettings->render())
    {
        QProcess p;
        p.setWorkingDirectory(dir.path());

        QStringList argv;
        argv << renderSettings->saveFile() + ".ini"
             << "-D"
             << "+V"
             << "+O" + QDir::current().filePath(renderSettings->saveFile() + suffix + ".png")
             << renderSettings->saveFile() + ".pov";

        p.start("povray", argv);
        p.waitForFinished(-1);
    }
#endif
}

void MovieMaker::addSphere(std::ostream& outFile, const QVector3D & position, float radius, const Material *color)
{
    outFile << "sphere {"
            << position << ", "
            << radius << " "
            << "material { " << color << " }"
            << "}\n";
}

void MovieMaker::addCylinder(std::ostream& outFile, const QVector3D & positionA, const QVector3D & positionB, float radiusA, float radiusB, const Material *colorA, const Material *colorB)
{
    QVector3D direction = positionB - positionA;

    outFile << "cone{"
            << " " << positionA << ", " << radiusA
            << " " << positionB << ", " << radiusB
            << " texture{gradient" << direction << " texture_map{[0.0 " << colorA << "tex][1.0 " << colorB << "tex]}"
            << " scale " << direction.length()
            << " translate " << positionA
            << "}}\n";
}

void MovieMaker::addSphere1(std::ostream& outFile, int id, float radius, const Material *color)
{
    outFile << "sphere {"
            << "Atom" << id << "Pos(clock), "
            << radius << " "
            << "material { " << color << " }"
            << "}\n";
}

void MovieMaker::addCylinder1(std::ostream& outFile, int idA, int idB, float radiusA, float radiusB, const Material *colorA, const Material *colorB)
{
    outFile << "cone{"
            << " Atom" << idA << "Pos(clock), " << radiusA
            << " Atom" << idB << "Pos(clock), " << radiusB
            << " texture{gradient (Atom" << idB << "Pos(clock)-Atom" << idA << "Pos(clock)) texture_map{[0.0 " << colorA << "tex][1.0 " << colorB << "tex]}"
            << " scale vlength(Atom" << idB << "Pos(clock)-Atom" << idA << "Pos(clock))"
            << " translate Atom" << idA << "Pos(clock)"
            << "}}\n";
}
