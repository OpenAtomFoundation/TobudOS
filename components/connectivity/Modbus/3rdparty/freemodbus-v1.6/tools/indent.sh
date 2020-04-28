#!/bin/sh

MYDIR="/cygdrive/c/GnuWin32/bin/"

$MYDIR/indent.exe --version
$MYDIR/indent.exe \
    --declaration-indentation16 \
    --procnames-start-lines \
    --blank-lines-after-procedures \
    --break-before-boolean-operator \
    --braces-after-if-line \
    --braces-after-struct-decl-line \
    --brace-indent0 \
    --case-indentation0 \
    --no-space-after-function-call-names \
    --no-space-after-for \
    --no-space-after-if \
    --no-space-after-while \
    --no-space-after-casts \
    --space-after-parentheses \
    --dont-format-comments \
    --indent-level4 \
    --ignore-newlines \
    --no-tabs \
    --line-length120 \
    $@
   
unix2dos $@
