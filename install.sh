QTDIR=/home/laurenthil/Qt/5.9.3/gcc_64
#QTDIR=/Users/irina/Praca/5.9.3/clang_64
DESTDIR=$(pwd)/build-QChromosome-Release

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

mkdir -p .tmp
cd .tmp
cmake ..
cd ..
rm -rf .tmp

CWD=$(pwd)

mkdir -p $DESTDIR
cd $DESTDIR
$QTDIR/bin/qmake $CWD/QChromosome/QChromosome.pro -spec $SPEC
/usr/bin/make
/usr/bin/make install
