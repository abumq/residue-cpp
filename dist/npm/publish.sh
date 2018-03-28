CURR_DIR=${PWD##*/}

if [ "$CURR_DIR" != "npm" ];then
    echo "Run this script from 'dist/npm' directory"
    exit;
fi

MAJOR=`grep 'RESIDUE_MAJOR' ../../CMakeLists.txt | grep -o [0-9] | tr -d '\n'`
MINOR=`grep 'RESIDUE_MINOR' ../../CMakeLists.txt | grep -o [0-9] | tr -d '\n'`
PATCH=`grep 'RESIDUE_PATCH' ../../CMakeLists.txt | grep [0-9] | awk '{print $3}' | cut -d "\"" -f2`
VERSION="$MAJOR.$MINOR.$PATCH"

echo "Publish $VERSION to NPM. Continue (y/n)?"

read confirm

if [ "$confirm" = "y" ]; then
    rm -rf headers/*.h
    cp ../../include/* headers/
    echo "---- CONTENTS ------"
    ls -l headers/
    echo "---- package.json ------"
    grep name headers/package.json
    grep version headers/package.json
    echo "---- npm whoami ----- (please wait...)"
    npm whoami
    echo "Do above contents look good? Continue (y/n)?"
    read good
    if [ "$good" = "y" ];then
        cd headers/
        npm publish
        cd ../
        rm -rf headers/*.h
    fi
fi
