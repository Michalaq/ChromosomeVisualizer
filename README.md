Chromosome Visualizer
=====================

Requirements
------------

Chromosome Visualizer is based on few components required for it to work correctly on Ubuntu 16.04:  
1.  **Qt, version >= 5.9** -- the framework used to develop the application
2.  **OpenGL, version >= 3.3** -- required for displaying the editor
3.  **build-essential libgl1-mesa-dev** -- these are required for OpenGL support
4.  **povray** -- required for picture capturing
5.  **ffmpeg** -- required for movie rendering
6.  **libprotobuf-dev protobuf-compiler** -- required for binary format support
7.  **gcc, version >= 5.0** -- required for C++14 support


Installation
------------

To install Chromosome Visualizer:
1.  Install all dependencies listed in "requirements".
2.  Provide Qt installation details for install.sh:
      * set QTDIR to path to the Qt installation directory (e.g. /home/username/Qt/5.9.3/gcc_64)
      * set DESTDIR to the desired location of binary files
3.  Run ./install.sh. The executable should appear in DESTDIR directory.
