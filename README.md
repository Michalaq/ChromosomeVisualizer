Chromosome Visualizer
=====================

Requirements
------------

Chromosome Visualizer is based on few components required for it to work correctly on Ubuntu 16.04:  
1.  **Qt 5.5** -- the framework used to develop the application  
2.  **build-essential libgl1-mesa-dev** -- these are required for OpenGL support  
3.  **povray povray-includes** -- required for picture capturing  
4.  **ffmpeg** -- required for movie rendering  
5.  **libprotobuf-dev protobuf-compiler** -- required for binary format support  
6.  **gcc, version >= 5.0** -- required for C++14 support


Installation
------------

To install Chromosome Visualizer:  
1.  Install all dependencies listed in "requirements".  
2.  Qt installation details:  
      * set "QT_PATH" in install.sh to path to the Qt installation folder.  
      * optionally, set "COMPILER" to the desired compiler used (it should be inside QT_PATH/5.5 folder)  
3.  Run ./install.sh.  
4.  The executable should appear in "build" directory.  
