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

void prepareINIFile(const QString& filename)
{
    std::ofstream outFile((filename + ".ini").toUtf8().constData());
    auto renderSettings = RenderSettings::getInstance();
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height() * (renderSettings->cam360() ? 2 : 1)
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

void prepareINIFile1(const QString& filename, int fbeg, int fend)
{
    std::ofstream outFile((filename + ".ini").toUtf8().constData());
    auto renderSettings = RenderSettings::getInstance();
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height() * (renderSettings->cam360() ? 2 : 1)
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

void setCamera(std::ofstream& outFile, const Camera* camera, bool s)
{
    camera->writePOVCamera(outFile, s);

    outFile << "light_source {\n"
            << QVector3D() << "," << QColor(Qt::white) << "\n"
            << "parallel\n"
            << "point_at " << -QVector3D(1., 1., 2.) << "\n"
            << "}\n";
}

void set360Camera(std::ofstream& outFile, const Camera* camera, bool s)
{
    outFile << "#declare odsIPD = 0.065;\n";

    if (s)
        outFile << "#declare odsLocation = MySplinePos(clock);\n"
                << "#declare odsAngles = MySplineAng(clock);\n";
    else
        outFile << "#declare odsLocation = " << camera->position() << ";\n"
                << "#declare odsAngles = " << camera->EulerAngles() << ";\n";

    outFile << "#declare odsX = <1, 0, 0>;\n"
            << "#declare odsY = <0, 1, 0>;\n"
            << "#declare odsZ = <0, 0, 1>;\n"
            << "#declare odsX = vaxis_rotate(odsX, odsY, odsAngles.y);\n"
            << "#declare odsZ = vaxis_rotate(odsZ, odsY, odsAngles.y);\n"
            << "#declare odsY = vaxis_rotate(odsY, odsX, -odsAngles.x);\n"
            << "#declare odsZ = vaxis_rotate(odsZ, odsX, -odsAngles.x);\n"
            << "#declare odsX = vaxis_rotate(odsX, odsZ, odsAngles.z);\n"
            << "#declare odsY = vaxis_rotate(odsY, odsZ, odsAngles.z);\n";

    outFile << "#declare odsLocationX = -odsLocation.x;\n"
            << "#declare odsLocationY = odsLocation.y;\n"
            << "#declare odsLocationZ = odsLocation.z;\n"
            << "#declare odsXX = odsX.x;\n"
            << "#declare odsXY = odsX.y;\n"
            << "#declare odsXZ = odsX.z;\n"
            << "#declare odsYX = odsY.x;\n"
            << "#declare odsYY = odsY.y;\n"
            << "#declare odsYZ = odsY.z;\n"
            << "#declare odsZX = -odsZ.x;\n"
            << "#declare odsZY = -odsZ.y;\n"
            << "#declare odsZZ = -odsZ.z;\n";

    outFile << "camera {\n"
            << "user_defined\n"
            << "location {\n"
            << "function { -(odsLocationX + cos(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsXX + sin(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsZX) }\n"
            << "function {   odsLocationY + cos(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsXY + sin(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsZY  }\n"
            << "function {   odsLocationZ + cos(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsXZ + sin(((x+0.5)) * 2 * pi - pi)*odsIPD/2*select(-y,-1,+1) * odsZZ  }\n"
            << "}\n"
            << "direction {\n"
            << "function { -((sin(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsXX + (sin(pi / 2 - select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsYX + (-cos(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi) * -1) * odsZX) }\n"
            << "function {   (sin(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsXY + (sin(pi / 2 - select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsYY + (-cos(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi) * -1) * odsZY  }\n"
            << "function {   (sin(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsXZ + (sin(pi / 2 - select(y, 1-2*(y+0.5), 1-2*y) * pi)) * odsYZ + (-cos(((x+0.5)) * 2 * pi - pi) * cos(pi / 2 -select(y, 1-2*(y+0.5), 1-2*y) * pi) * -1) * odsZZ  }\n"
            << "}\n"
            << "}\n";

    outFile << "light_source {\n"
            << QVector3D() << "," << QColor(Qt::white) << "\n"
            << "parallel\n"
            << "point_at " << -QVector3D(1., 1., 2.) << "\n"
            << "}\n";
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

void MovieMaker::captureScene(int fbeg, int fend, const VizWidget* scene, const Camera* camera, QString suffix, int fr)
{
    QTemporaryDir dir;
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + '/' + renderSettings->saveFile();
    prepareINIFile1(filename, fbeg, fend);
    std::ofstream outFile;
    createPOVFile(outFile, filename.toStdString());

    if (renderSettings->cam360())
        set360Camera(outFile, camera, camera->count() > 1);
    else
        setCamera(outFile, camera, camera->count() > 1);
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

        if (renderSettings->overlays())
        {
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
    prepareINIFile(filename);
    std::ofstream outFile;
    createPOVFile(outFile, filename.toStdString());

    if (renderSettings->cam360())
        set360Camera(outFile, camera, false);
    else
        setCamera(outFile, camera, false);
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
