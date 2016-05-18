QT_PATH=/home/bartek/Qt
PROJECT__PATH=.
COMPILER=gcc_64

protoc --proto_path=QtChromosomeViz_v2/bartekm_code/message_format --cpp_out=QtChromosomeViz_v2/bartekm_code/message_format QtChromosomeViz_v2/bartekm_code/message_format/message.proto
mkdir -p build
cd build
$QT_PATH/5.5/$COMPILER/bin/qmake ../$PROJECT_PATH/QChromosome/QChromosome.pro
make

