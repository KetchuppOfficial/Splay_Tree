#!/bin/bash

green="\033[1;32m"
red="\033[1;31m"
default="\033[0m"

function usage()
{
    local message=$1
    echo -e "${red}Error:${default} $message\n"
    echo "Usage: /path/to/checker.sh [OPTIONS]"
    echo ""
    echo "-m <testing-mode>         Set the tree to test on: <splay> or <splay+>"
    echo "-k <number-of-keys>       Set the number of keys for the test"
    echo "-q <number-of-queries>    Set the number of queries for the test"
    exit 1
}

N_ARGS=$#
if [ $N_ARGS -ne 6 ]; then # each option takes 2 command line arguments
    usage "Script requires 3 options"
fi

NUM="^[0-9]+$"

while getopts :m:k:q: OPT; do
    case $OPT in
    m) # -m <testing-mode>: splay/splay+
        tree=$OPTARG
        [[ $tree != "splay" || $tree != "splay+" ]] || usage "the is no testing mode with name ${N_PROC}"
	    ;;
	k) # -k <number-of-keys>
	    n_keys=$OPTARG
        [[ $n_keys =~ $NUM ]] || usage "-k is followed not by a number"
        [[ $n_keys -gt 0 ]] || usage "-k is followed by a non-positive number"
	    ;;
    q) # -q <number-of-queries>
        n_queries=$OPTARG
        [[ $n_queries =~ $NUM ]] || usage "-q is followed not by a number"
        [[ $n_queries -gt 0 ]] || usage "-q is followed by a non-positive number"
	    ;;
	*)
	    usage "invalid command line argument $OPTARG"
	    ;;
  esac
done
