#!/bin/sh

usage="Usage: $0 [OPTION]...

  --help      display this help and exit
  --version   output version information and exit"

case $# in
  1 )
    case "z${1}" in 
      z--help )
	 echo "$usage"; exit 0 ;;
      z--version )
	 echo "false - @VERSION@"; exit 0 ;;
      * ) ;;
    esac
    ;;
  * ) ;;
esac

exit 1
