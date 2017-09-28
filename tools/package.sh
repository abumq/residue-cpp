# Usage: package.sh darwin

CURR_DIR=${PWD##*/}

if [ "$CURR_DIR" != "build" ];then
	echo "Run this script from 'build' directory"
	exit;
fi

TYPE=$1
VERSION=$2

if [ "$TYPE" = "" ] || [ "$VERSION" = "" ];then
	echo "Usage: $0 <type> version>"
	echo "  example: $0 darwin 1.0.0"
	exit;
fi

PACK=libresidue-$VERSION-x86_64-$TYPE

if [ -d "$PACK" ];then
	echo "$PACK already exist. Remove $PACK first"
	exit;
fi


cmake -DCMAKE_BUILD_TYPE=Release -Dprofiling=OFF -Dtest=OFF ..
make

echo "Creating $PACK.tar.gz ..."
mkdir $PACK
cp libresidue.so $PACK/libresidue.so
cp libresidue.dylib $PACK/libresidue.dylib

ls -lh $PACK

tar cfz $PACK.tar.gz $PACK
rm -rf $PACK
shasum $PACK.tar.gz
echo `pwd`/$PACK.tar.gz
