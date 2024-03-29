#!/bin/sh

VERSION=$1

if [ "$VERSION" = "" ];then
    VERSION=`curl https://api.github.com/repos/abumq/residue-cpp/releases | grep tag_name | head -n1 | grep -o "[0-9]\.[0-9]\.[0-9]"`
fi

TYPE=`uname | tr '[:upper:]' '[:lower:]'`

if [ "$LINUX_SPECIAL_EDITION" != "" ];then
    TYPE=linux-se
fi

if [ "$VERSION" = "" ];then
    echo "Could not determine latest version."
    if [ "$RESIDUE_VERSION" != "" ];then
        echo "Setting to $RESIDUE_VERSION form RESIDUE_VERSION env variable"
    else
        echo "Please enter manual e.g, 2.1.3"
        read RESIDUE_VERSION
    fi
    VERSION=$RESIDUE_VERSION
fi

PKG=libresidue-$VERSION-x86_64-$TYPE

echo "-----------"
echo " INSTALLING $PKG"
echo "-----------"

## Headers
wget https://github.com/abumq/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-headers.tar.gz
tar -xf libresidue-$VERSION-headers.tar.gz
mkdir -p /usr/local/include/residue/
cp libresidue-$VERSION-headers/* /usr/local/include/residue/

## Dynamic lib
[[ $TYPE = "darwin" ]] && EXTENSION="dylib" || EXTENSION="so"
[[ $TYPE = "darwin" ]] && DEST_SUFFIX=".$VERSION.$EXTENSION" || DEST_SUFFIX=".$EXTENSION.$VERSION"
wget https://github.com/abumq/residue-cpp/releases/download/v$VERSION/$PKG.tar.gz
tar -xf $PKG.tar.gz
cp $PKG/libresidue* /usr/local/lib/libresidue$DEST_SUFFIX
rm /usr/local/lib/libresidue.$EXTENSION
ln -s /usr/local/lib/libresidue$DEST_SUFFIX /usr/local/lib/libresidue.$EXTENSION

## Static lib
wget https://github.com/abumq/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
tar -xf libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
cp libresidue-$VERSION-static-x86_64-$TYPE/libresidue-static.$VERSION.a /usr/local/lib/
rm /usr/local/lib/libresidue-static.a
ln -s /usr/local/lib/libresidue-static.$VERSION.a /usr/local/lib/libresidue-static.a
