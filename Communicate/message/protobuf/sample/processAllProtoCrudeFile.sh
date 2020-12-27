#!/usr/bin/env bash
#delete all old proto.pb.h and proto.pb.cc
rm ./*.pb.h
rm ./*.pb.cc

#generate all proto.pb.h and proto.pb.cc files from proto file
echo "***********************************************************************************"
echo "start compiling proto file..."
export PATH=$PATH:../../../../3rdparty/x86/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../../../3rdparty/x86/lib
for i in `ls ./*.proto`
	do protoc -I=. --cpp_out=. $i
	echo "$i has been compiled"
done
#move source file to src
