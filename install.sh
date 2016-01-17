QT_PATH=/home/bartek/Qt
PROJECT__PATH=.
COMPILER=gcc_64

mkdir build
cd build
$QT_PATH/5.5/$COMPILER/bin/qmake ../$PROJECT_PATH/QChromosome/QChromosome.pro
make

