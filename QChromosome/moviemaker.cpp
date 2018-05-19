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

#include <QFile>

void prepareINIFile(const QString& filename)
{
    QFile out(filename + ".ini");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    auto renderSettings = RenderSettings::getInstance();
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height() * (renderSettings->cam360() ? 2 : 1)
            << "\nQuality=" << renderSettings->quality();
    if (renderSettings->antiAliasing())
    {
        outFile << "\nAntialias=on"
                << "\nSampling_Method=" << renderSettings->aaSamplingMethod()
                << "\nAntialias_Threshold=" << renderSettings->aaThreshold()
                << "\nAntialias_Depth=" << renderSettings->aaDepth();
        if (renderSettings->aaJitter())
        {
            outFile << "\nJitter=on"
                    << "\nJitter_Amount=" << renderSettings->aaJitterAmount();
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
    QFile out(filename + ".ini");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    auto renderSettings = RenderSettings::getInstance();
    QSize size = renderSettings->outputSize();
    outFile << "Width=" << size.width() << "\nHeight=" << size.height() * (renderSettings->cam360() ? 2 : 1)
            << "\nQuality=" << renderSettings->quality();
    if (renderSettings->antiAliasing())
    {
        outFile << "\nAntialias=on"
                << "\nSampling_Method=" << renderSettings->aaSamplingMethod()
                << "\nAntialias_Threshold=" << renderSettings->aaThreshold()
                << "\nAntialias_Depth=" << renderSettings->aaDepth();
        if (renderSettings->aaJitter())
        {
            outFile << "\nJitter=on"
                    << "\nJitter_Amount=" << renderSettings->aaJitterAmount();
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

void createPOVFile(QTextStream& outFile)
{
    outFile << "#version 3.7;\n"
            << "global_settings { assumed_gamma 1.0 }\n";
}

void setCamera(QTextStream& outFile, const Camera* camera, bool s)
{
    camera->writePOVCamera(outFile, s);

    outFile << "light_source {\n"
            << QVector3D() << "," << QColor(Qt::white) << "\n"
            << "parallel\n"
            << "point_at " << -QVector3D(1., 1., 2.) << "\n"
            << "}\n";
}

void set360Camera(QTextStream& outFile, const Camera* camera, bool s)
{
    outFile << "#declare odsIPD = 0.065;\n";

    if (s)
        outFile << "#declare odsLocation = MySplinePos(clock);\n"
                << "#declare odsAngles = MySplineAng(clock);\n";
    else
        outFile << "#declare odsLocation = " << camera->getPosition() << ";\n"
                << "#declare odsAngles = " << camera->getRotation() << ";\n";

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

void MovieMaker::captureScene(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix, int fr)
{
    if (isRunning())
    {
        QMessageBox::information(0, "QChromosome 4D Studio", "The external renderer is calculating an image."/*" Do you want to stop it?"*/);
        return;
    }

    snapshot = false; fbeg_ = fbeg; fend_ = fend; simulation_ = simulation; camera_ = camera; suffix_ = suffix; fr_ = fr;
    start();
}

void MovieMaker::captureScene_(int fbeg, int fend, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix, int fr)
{
    QTemporaryDir dir;
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + '/' + renderSettings->saveFile();
    prepareINIFile1(filename, fbeg, fend);
    QFile out(filename + ".pov");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    createPOVFile(outFile);

    if (renderSettings->cam360())
        set360Camera(outFile, camera, camera->count() > 1);
    else
        setCamera(outFile, camera, camera->count() > 1);

    auto& buffer = Viewport::getBuffer();
    setBackgroundColor(outFile, buffer.ucBackgroundColor);
    if (buffer.ubEnableFog) setFog(outFile, buffer.ucFogColor, 1. - buffer.ufFogStrength, buffer.ufFogDistance);

    Material::writePOVMaterials(outFile);

    simulation->writePOVFrames(outFile, fbeg, fend);

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

        QRegularExpression re1("Rendering frame (\\d+) of (\\d+) \\((#\\d+)\\)");
        QRegularExpression re("Rendered (\\d+) of (\\d+) pixels \\((\\d+)%\\)");
        QByteArray buffer;

        int cf, tf;

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

        emit progressChanged(101);

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

        if (renderSettings->openFile())
            QProcess::execute("xdg-open", {QDir::current().filePath(renderSettings->saveFile() + suffix + ".mp4")});
    }
#endif
}

void MovieMaker::captureScene1(int fn, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix)
{
    if (isRunning())
    {
        QMessageBox::information(0, "QChromosome 4D Studio", "The external renderer is calculating an image."/*" Do you want to stop it?"*/);
        return;
    }

    snapshot = true; fn_ = fn; simulation_ = simulation; camera_ = camera; suffix_ = suffix;
    start();
}

void MovieMaker::captureScene1_(int fn, const std::shared_ptr<Simulation> simulation, const Camera* camera, QString suffix)
{
    QTemporaryDir dir;
    auto renderSettings = RenderSettings::getInstance();
    QString filename = dir.path() + "/" + renderSettings->saveFile();
    prepareINIFile(filename);
    QFile out(filename + ".pov");
    out.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream outFile(&out);
    createPOVFile(outFile);

    if (renderSettings->cam360())
        set360Camera(outFile, camera, false);
    else
        setCamera(outFile, camera, false);

    auto& buffer = Viewport::getBuffer();
    setBackgroundColor(outFile, buffer.ucBackgroundColor);
    if (buffer.ubEnableFog) setFog(outFile, buffer.ucFogColor, 1. - buffer.ufFogStrength, buffer.ufFogDistance);

    Material::writePOVMaterials(outFile);

    simulation->writePOVFrame(outFile, fn);

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

        QRegularExpression re("Rendered (\\d+) of (\\d+) pixels \\((\\d+)%\\)");
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
            QProcess::execute("xdg-open", {QDir::current().filePath(renderSettings->saveFile() + suffix + ".png")});
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
        captureScene1_(fn_, simulation_, camera_, suffix_);
    else
        captureScene_(fbeg_, fend_, simulation_, camera_, suffix_, fr_);
}
