# Building [QChromosome 4D Studio](http://regulomics.mimuw.edu.pl/wp/qcv/) on Linux

This distribution contains complete source code for QChromosome 4D Studio.

## Dependencies

QChromosome 4D Studio is developed with cross-platform software framework **Qt 5.9 LTS**. Before installation verify that your graphic driver supports **OpenGL 4.0**.

By default aplication will attempt to build using system libraries. On *Ubuntu 18.04.1 LTS (Bionic Beaver)* we recommend you have the following packages pre-installed:

```shell
build-essential
qt5-default
libqt5svg5-dev
libqt5charts5-dev
zlib1g-dev
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

## :warning: Running

In order to use **POV-Ray** or **FFmpeg**, the `PATH` environment variable may need to be extended.
