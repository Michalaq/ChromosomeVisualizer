Chromosome Visualizer
=====================

Requirements
------------

QChromosome 4D Studio requires some libraries and headers installed to work correctly under Ubuntu 16.04:  
1.  **build-essential libgl1-mesa-dev** -- dependencies required for Qt installation
2.  **Qt, version >= 5.9** -- application framework
3.  **OpenGL, version >= 4.1** -- required for displaying the editor
4.  **povray, version >= 3.7.1** -- required for rendering pictures and movies
5.  **ffmpeg** -- required for converting movies
6.  **zlib** -- required for .gz support


Installation
------------

To install Chromosome Visualizer:
1.  Install all dependencies listed in "Requirements".
2.  Provide Qt installation details for install.sh:
      * set QTDIR to path to the Qt installation directory (e.g. /home/username/Qt/5.9.3/gcc_64)
      * set DESTDIR to the desired location of binary files
3.  Run ./install.sh with root privileges. The executable should appear in /usr/local/bin.
