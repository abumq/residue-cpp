#!/bin/sh

VERSION=`curl https://api.github.com/repos/muflihun/residue-cpp/releases | grep tag_name | head -n1 | grep -o "[0-9]\.[0-9]\.[0-9]"`
DAR=`uname -a | grep 'Darwin' | wc -c | grep -o '[0-9]'`
TYPE=darwin
if [ "$DAR" = "0" ];then
    TYPE=linux
fi

## Headers
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-headers.tar.gz
tar -xf libresidue-$VERSION-headers.tar.gz
mkdir /usr/local/include/residue/
cp libresidue-$VERSION-headers/* /usr/local/include/residue/

## Dynamic lib
[[ $TYPE = "darwin" ]] && EXTENSION="dylib" || EXTENSION="so"
[[ $TYPE = "darwin" ]] && DEST_SUFFIX=".$VERSION.$EXTENSION" || DEST_SUFFIX=".$EXTENSION.$VERSION"
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-x86_64-$TYPE.tar.gz
tar -xf libresidue-$VERSION-x86_64-$TYPE.tar.gz
cp libresidue-$VERSION-x86_64-$TYPE/libresidue* /usr/local/lib/libresidue$DEST_SUFFIX
ln -s /usr/local/lib/libresidue$DEST_SUFFIX /usr/local/lib/libresidue.$EXTENSION

## Static lib
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
tar -xf libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
cp libresidue-$VERSION-static-x86_64-$TYPE/libresidue-static.$VERSION.a /usr/local/lib/
ln -s /usr/local/lib/libresidue-static.$VERSION.a /usr/local/lib/libresidue-static.a

