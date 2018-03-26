QTDIR=/home/laurenthil/Qt/5.9.3/gcc_64
#QTDIR=/Users/irina/Praca/5.9.3/clang_64
DESTDIR=$(pwd)/build-QChromosome-Desktop_Qt_5_9_3-Debug

case $(uname) in
    "Linux")
        SPEC=linux-g++
        ;;
    "Darwin")
        SPEC=macx-clang
        ;;
    *)
        echo "System not supported."
        exit 1
        ;;
esac

protoc --proto_path=QtChromosomeViz_v2/bartekm_code/message_format --cpp_out=QtChromosomeViz_v2/bartekm_code/message_format QtChromosomeViz_v2/bartekm_code/message_format/message.proto

mkdir -p .tmp
cd .tmp
cmake ..
cd ..
rm -rf .tmp

CWD=$(pwd)

mkdir -p $DESTDIR
cd $DESTDIR
$QTDIR/bin/qmake $CWD/QChromosome/QChromosome.pro -spec $SPEC CONFIG+=debug CONFIG+=qml_debug
/usr/bin/make
cd $CWD
