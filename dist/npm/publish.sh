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
if [ "$confirm" = "y" ]; then
    if [ ! -f ../../build/libresidue-$VERSION-$TYPE-x86_64.tar.gz ];then
        echo "Could not find libresidue-$VERSION-$TYPE-x86_64.tar.gz in build directory"
        exit 1;
    fi
    tar -xf ../../build/libresidue-$VERSION-$TYPE-x86_64.tar.gz -C $TYPE/
    rm -rf $TYPE/libresidue.so.*
    rm -rf $TYPE/libresidue.*.dylib
    mv $TYPE/libresidue-$VERSION-$TYPE-x86_64/libresidue* $TYPE/
    cp ../../include/* $TYPE/
    rm -rf $TYPE/libresidue-$VERSION-$TYPE-x86_64
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
