#!/bin/bash

# argv[1]: the number of keys
# argv[2]: the number of queries

green="\033[1;32m"
red="\033[1;31m"
default="\033[0m"

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
top_dir="${script_dir}/../../"
build_dir="${top_dir}build/"
data="${script_dir}/data/"
bin_dir="${script_dir}/bin/"

test_generator="generator"
ans_generator="ans_generator"

function build_from_sources
{
    local tree=$1

    if [ $tree = "splay" ]
    then
        test_driver="driver"
    else
        test_driver="augmented_driver"
    fi

    local basic_options="-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-14"

    cmake ${top_dir} -B ${build_dir} ${basic_options}

    echo -en "\n"
    echo -e "${green}Building test generator...${default}"
    cmake --build ${build_dir} --target ${test_generator}
    echo -en "\n"

    echo -e "${green}Building test driver...${default}"
    cmake --build ${build_dir} --target ${test_driver}
    echo -en "\n"

    echo -e "${green}Building generator of answers...${default}"
    cmake --build ${build_dir} --target ${ans_generator}
    echo -en "\n"

    echo -e "${green}Installing...${default}"
    cmake --install ${build_dir}
    echo -en "\n"
}

function generate_test
{
    local n_keys=$1
    local n_queries=$2

    mkdir -p ${data}

    echo -e "${green}Generating test...${default}"
    ${bin_dir}${test_generator} --n-keys ${n_keys} --n-queries ${n_queries} > "${data}${n_keys}_${n_queries}.test"
    echo -en "\n"
}

function generate_answer
{
    local n_keys=$1
    local n_queries=$2

    echo -e "${green}Generating answer...${default}"
    ${bin_dir}${ans_generator} < "${data}${n_keys}_${n_queries}.test" \
                               > "${data}${n_keys}_${n_queries}.ans"
    echo -en "\n"
}

function run_test
{
    local n_keys=$1
    local n_queries=$2

    echo -e "${green}Running test...${default}"
    ${bin_dir}${test_driver} < "${data}${n_keys}_${n_queries}.test" \
                             > "${data}${n_keys}_${n_queries}.res"
    echo -en "\n"

    if diff -Z "${data}${n_keys}_${n_queries}.ans" "${data}${n_keys}_${n_queries}.res" > /dev/null
    then
        echo -e "${green}Test passed${default}"
    else
        echo -e "${red}Test failed${default}"
    fi
}

if [ $# -ne 3 ]
then
    echo -e "${red}Testing script requires exactly 3 arguments${default}"
else
    tree=$1

    if [ ! $tree = "splay" ] && [ ! $tree = "splay+" ]
    then
        echo -e "${red}There is no testing mode with name \"$tree\"${default}"
    else
        n_keys=$2
        number="^[0-9]+$"

        if ! [[ $n_keys =~ $number ]] || [ $n_keys -le 0 ]
        then
            echo -e "${red}The number of keys has to be a positive integer number${default}"
        else
            n_queries=$3

            if ! [[ $n_queries =~ $number ]] || [ $n_queries -le 0 ]
            then
                echo -e "${red}The number of queries has to be a positive integer number${default}"
            else
                build_from_sources $tree
                generate_test $n_keys $n_queries
                generate_answer $n_keys $n_queries
                run_test $n_keys $n_queries
            fi
        fi
    fi
fi
