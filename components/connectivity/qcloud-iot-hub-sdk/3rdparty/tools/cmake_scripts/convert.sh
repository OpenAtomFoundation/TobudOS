#!/bin/bash

if [ $1 == "linux" ]; then
   find $2 -type f -name "*.h" -print -o -name "*.c" -print | xargs -i sed -i '1 s/^\xef\xbb\xbf//' {}
   echo "Convert source files to Unix format!!!"
elif [ $1 == "windows" ]; then
   find $2 -type f -name "*.h" -print -o  -name "*.c" -print | xargs -i sed -i '1 s/^/\xef\xbb\xbf&/' {}
   echo "Convert source files to Windows format!!!" 
else 
   echo "Invaild argument!"
   echo "Please choose windows or linux !!!"
fi	
