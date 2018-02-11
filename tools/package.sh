# Usage: package.sh darwin

CURR_DIR=${PWD##*/}

if [ "$CURR_DIR" != "build" ];then
	echo "Run this script from 'build' directory"
	exit;
fi

STRIP=strip
TYPE=$1
VERSION=$2

SPECIAL_EDITION_VERSION=$3
if [ "$SPECIAL_EDITION_VERSION" = "" ];then
    SPECIAL_EDITION_VERSION="OFF"
fi

if [ "$SHASUM" = "" ];then
    export SHASUM="shasum"
fi

if [ "$TYPE" = "" ] || [ "$VERSION" = "" ];then
	echo "Usage: $0 <type> <version> <special_edition = OFF>"
	echo "  example: $0 darwin 1.0.0 OFF"
	exit;
fi

if [ `grep -o ' -O0 ' ../CMakeLists.txt -c` != "0" ];then
    echo "Error: Optimization not reset"
    exit;
fi

PACK=libresidue-$VERSION-x86_64-$TYPE
PACK_STATIC=libresidue-$VERSION-static-x86_64-$TYPE

if [ -d "$PACK" ];then
	echo "$PACK already exist. Remove $PACK first"
	exit;
fi

if [ -d "$PACK_STATIC" ];then
	echo "$PACK_STATIC already exist. Remove $PACK_STATIC first"
	exit;
fi


cmake -DCMAKE_BUILD_TYPE=Release -Dproduction=ON -Dprofiling=OFF -Dtest=OFF -Dspecial_edition=$SPECIAL_EDITION_VERSION ..
make

if [ [ "$STATIC_CRYPTOPP_LIB" = "" ]; then
    echo "Please specify STATIC_CRYPTOPP_LIB"
    exit;
fi

echo "Creating $PACK.tar.gz a dn $PACK_STATIC..."
mkdir $PACK
mkdir $PACK_STATIC

echo "Creating static full"
cd $PACK_STATIC
cp $STATIC_CRYPTOPP_LIB .
cp ../libresidue-static.a .
ar -x libcrypto*.a
ar -x libresidue-static.a
rm -rf *.a
ar -qc libresidue-static-full.a *.o
rm *.o
cd ..
mv $PACK_STATIC/libresidue-static-full.a .
rm -rf $PACK_STATIC/*
mv libresidue-static-full.a $PACK_STATIC/libresidue-static.a

cp libresidue.so $PACK/libresidue.so
cp libresidue.dylib $PACK/libresidue.dylib

ls -lh $PACK
ls -lh $PACK_STATIC

tar cfz $PACK.tar.gz $PACK
tar cfz $PACK_STATIC.tar.gz $PACK_STATIC
rm -rf $PACK
rm -rf $PACK_STATIC
$SHASUM $PACK.tar.gz
$SHASUM $PACK_STATIC.tar.gz
echo `pwd`/$PACK.tar.gz
echo `pwd`/$PACK_STATIC.tar.gz
