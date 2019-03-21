# Building [QChromosome 4D Studio](http://regulomics.mimuw.edu.pl/wp/qcv/) on Mac OS

This distribution contains complete source code for QChromosome 4D Studio.

## Dependencies

QChromosome 4D Studio is developed with cross-platform software framework **Qt 5.9**. Before installation verify that your graphic driver supports **OpenGL 4.1**.

By default aplication will attempt to build using system libraries. On *macOS 10.14 Mojave* we recommend you have the following packages pre-installed:

```shell
qt5
```

If you do not have these packages installed, you can install them by executing the following commands:

```shell
brew install qt5
brew link qt5 --force
```

To enable rendering also pre-install:

```shell
povray
```

Rendering Omni-Directional Strereo (ODS) requires user defined camera capability. It was added to pre-release **POV-Ray 3.8** shared at https://github.com/POV-Ray/povray/releases/.

To enable converting a set of images into a video also pre-install:

```shell
ffmpeg
```

## Building the code

Create a build directory and from inside it run

```shell
qmake ..
make
```

## Installing

From inside a build directory run

```shell
su
make install
```
