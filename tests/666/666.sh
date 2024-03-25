#!/bin/sh

set -eu;umask 0022;export LC_ALL=C
if _p="$(command -p getconf PATH 2>/dev/null)";then PATH="$_p${PATH+:}${PATH:-}";fi
export UNIX_STD=2003 POSIXLY_CORRECT=1;IFS=' 	
'

($CMD_DIS_INTERPRETER $TEST_PROG;echo)|awk -v failed=1 'NR==1&&$0=="666"{failed=0;}failNR==2{exit failed=1};END{exit failed}'
