QTDIR=/home/laurenthil/Qt/5.9.6/gcc_64
#QTDIR=/Users/irina/Praca/5.9.3/clang_64
DESTDIR=$(pwd)/build-QChromosome4DStudio-Release

case $(uname) in
    "Linux")
        SPEC=linux-g++
        ;;
    "Darwin")
        SPEC=macx-clang
        ;;
    *)
        echo "Platform not supported."
        exit 1
        ;;
esac

CWD=$(pwd)

mkdir -p $DESTDIR
cd $DESTDIR
$QTDIR/bin/qmake $CWD/QChromosome/QChromosome.pro -spec $SPEC
make
make install
