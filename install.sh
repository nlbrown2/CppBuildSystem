make;
EXE="cppb"
if [ $# -ne 0 ]; then
    EXE=$1
fi
cp cppb /usr/local/bin/$EXE
