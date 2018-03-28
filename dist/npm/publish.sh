CURR_DIR=${PWD##*/}

if [ "$CURR_DIR" != "npm" ];then
    echo "Run this script from 'dist/npm' directory"
    exit;
fi

TYPE=`uname | tr '[:upper:]' '[:lower:]'`
if [ "$1" != "" ];then
    TYPE="$1"
fi

MAJOR=`grep 'RESIDUE_MAJOR' ../../CMakeLists.txt | grep -o [0-9] | tr -d '\n'`
MINOR=`grep 'RESIDUE_MINOR' ../../CMakeLists.txt | grep -o [0-9] | tr -d '\n'`
PATCH=`grep 'RESIDUE_PATCH' ../../CMakeLists.txt | grep [0-9] | awk '{print $3}' | cut -d "\"" -f2`
VERSION="$MAJOR.$MINOR.$PATCH"

echo "Publish $VERSION on $TYPE to NPM. Continue (y/n)?"

read confirm

PKG=libresidue-$VERSION-x86_64-$TYPE

if [ "$confirm" = "y" ]; then
    if [ ! -f ../../build/$PKG.tar.gz ];then
        echo "Could not find $PKG.tar.gz in build directory"
        exit 1;
    fi
    tar -xf ../../build/$PKG.tar.gz -C $TYPE/
    rm -rf $TYPE/libresidue.so.*
    rm -rf $TYPE/libresidue.*.dylib
    mv $TYPE/$PKG/libresidue* $TYPE/
    cp ../../include/* $TYPE/
    rm -rf $TYPE/$PKG
    echo "---- CONTENTS ------"
    ls -l $TYPE/
    echo "---- package.json ------"
    grep name $TYPE/package.json
    grep version $TYPE/package.json
    echo "---- npm whoami ----- (please wait...)"
    npm whoami
    echo "Do above contents look good? Continue (y/n)?"
    read good
    if [ "$good" = "y" ];then
        cd $TYPE/
        npm publish
        cd ../
        rm -rf $TYPE/*.h $TYPE/libresidue*
    fi
fi
