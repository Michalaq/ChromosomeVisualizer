# Building [QChromosome 4D Studio](http://regulomics.mimuw.edu.pl/wp/qcv/) on Windows

This distribution contains complete source code for QChromosome 4D Studio.

## Dependencies

QChromosome 4D Studio is developed with cross-platform software framework **Qt 5.9 LTS**. Before installation verify that your graphic driver supports **OpenGL 4.0**.

You can download Qt SDK from official website https://www.qt.io/download. Run the installation package and follow instructions. On *Windows 10* we recommend you have the following components pre-installed:
```shell
MinGW 5.3.0 32 bit
Qt Charts
```

To enable rendering also pre-install **POV-Ray**. You can download it from official website https://www.povray.org/download/. Rendering Omni-Directional Strereo (ODS) requires user defined camera capability. It was added to pre-release **POV-Ray 3.8** shared at https://github.com/POV-Ray/povray/releases/.

To enable converting a set of images into a video also pre-install **FFmpeg**. You can download it from official website https://www.ffmpeg.org/download.html.

## Building the code

In order to use **Qt**, the `PATH` environment variable needs to be extended. This is done by adding

```shell
C:\Qt\Tools\mingw530_32\bin
C:\Qt\5.9\mingw53_32\bin
```

to the `PATH` variable through the *Control Panel | System | Advanced | Environment variables* menu.

Launch the *Command Prompt*. Press *Windows + X* or right-click the *Start* button to open *Power User* menu and select *Command Prompt*. Create a build directory and from inside it run

```shell
qmake ..
mingw32-make
```

## Installing

Launch the *Command Prompt* as Administrator. Press *Windows + X* or right-click the *Start* button to open *Power User* menu and select *Command Prompt (Admin)*. From inside a build directory run

```shell
mingw32-make install
```
