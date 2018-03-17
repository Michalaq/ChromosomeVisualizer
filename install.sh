QTDIR=/home/laurenthil/Qt/5.9.3/gcc_64
DESTDIR=build-QChromosome-Desktop_Qt_5_9_3_GCC_64bit-Debug

protoc --proto_path=QtChromosomeViz_v2/bartekm_code/message_format --cpp_out=QtChromosomeViz_v2/bartekm_code/message_format QtChromosomeViz_v2/bartekm_code/message_format/message.proto

mkdir tmp
cd tmp
cmake ..
cd ..
rm -rf tmp

mkdir -p $DESTDIR
cd $DESTDIR
$QTDIR/bin/qmake ../QChromosome/QChromosome.pro CONFIG+=debug CONFIG+=qml_debug
/usr/bin/make
