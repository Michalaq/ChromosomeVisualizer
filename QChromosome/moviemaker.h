#ifndef MOVIEMAKER_H
#define MOVIEMAKER_H

#include <cstdlib>
#include <fstream>

#include "../QtChromosomeViz_v2/VizWidget.hpp"


class MovieMaker
{
public:

    static void inline captureScene(const std::string& filename, QVector<VizBallInstance> & vizBalls, const int connectionCount, QVector3D & cameraLocation, QVector3D & CameraLookAt)
    {
        prepareINIFile(1920, 1080, true);
        std::ofstream outFile = createPOVFile(filename);
        setCamera(outFile, cameraLocation.x, cameraLocation.y, cameraLocation.z, cameraLookAt.x, cameraLookAt.y, cameraLookAt.z, 1920, 1080);
        for (unsigned int i = 0; i < vizBalls.length(); i++)
        {
            addSphere(outFile, vizBalls[i].position.x, vizBalls[i].position.y, vizBalls[i].position.z, vizBalls[i].size,
                      ((vizBalls[i].color >> 16) & 0xFF) / 255.f, ((vizBalls[i].color >> 8) & 0xFF) / 255.f,
                      (vizBalls[i].color & 0xFF) / 255.f, 1.f - (vizBalls[i].color >> 24) / 255.f);
        }

        for (unsigned int i = 0; i < connectionCount; i++)
        {
            addCylinder(outFile, vizBalls[i].position.x, vizBalls[i].position.y, vizBalls[i].position.z,
                        vizBalls[i+1].position.x, vizBalls[i+1].position.y, vizBalls[i+1].position.z, vizBalls[i].size / 3.f,
                        ((vizBalls[i].color >> 16) & 0xFF) / 255.f, ((vizBalls[i].color >> 8) & 0xFF) / 255.f,
                        (vizBalls[i].color & 0xFF) / 255.f, ((vizBalls[i+1].color >> 16) & 0xFF) / 255.f,
                        ((vizBalls[i+1].color >> 8) & 0xFF) / 255.f, (vizBalls[i+1].color & 0xFF) / 255.f);
        }

//TODO: ponizej do ogarniecia
#ifdef _WIN32
        system(R"~(""C:\Program Files\POV-Ray\v3.7\bin\pvengine64.exe" povray.ini -D /RENDER )~" + filename + ".pov \/EXIT");
#else
        system("povray povray.ini " + filename + ".pov");
#endif

    }

private:

    static void inline prepareINIFile(int width, int height, bool aa)
    {
        std::ofstream outFile("povray.ini");
        outFile << "Width=" << width << "\nHeight=" << height << "\nAntialias=" << (aa ? "On" : "Off") << std::endl;
    }

    static std::ofstream inline createPOVFile(std::string filename)
    {
        std::ofstream outFile(filename + ".pov");

        outFile << "#include \"colors.inc\"\n#include \"stones.inc\"" << std::endl;

        return outFile;
    }

    void inline setCamera(std::ofstream& outFile, float x, float y, float z, float lookx, float looky, float lookz, float reswidth, float resheight)
    {
        outFile << "camera{right x*" << reswidth << "/" << resheight << "\nlocation<" << x << ", " << y << ", " << z << ">look_at<" << lookx << ", " << looky << ", " << lookz << ">}\n" <<
            "light_source {<" << x << ", " << y << ", " << z << "> color White}"<< std::endl;
    }

    static void inline addSphere(std::ofstream& outFile, float x, float y, float z, float size, float r, float g, float b, float t)
    {
        outFile << "sphere{<" << x << ", " << y << ", " << z << ">, " << size << " texture{pigment{rgbt<" << r << ", " << g << ", " << b << ", " << t << ">}}}" << std::endl;
    }

    static void inline addCylinder(std::ofstream& outFile, float ax, float ay, float az, float bx, float by, float bz, float radius,
        float ar, float ag, float ab, float br, float bg, float bb)
    {
        outFile << "cylinder{<" << ax << ", " << ay << ", " << az << ">, <" << bx << ", " << by << ", " << bz << ">," << radius <<
            " texture{pigment{gradient<" << ax-bx << ", " << ay-by << ", " << az-bz << "> color_map{[0.0 color rgb<" << ar << ", " << ag << ", " << ab << ">][1.0 color rgb<"
            << br << ", " << bg << ", " << bb << ">]}}}}" << std::endl;
    }
};

#endif // MOVIEMAKER_H
