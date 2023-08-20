#!/bin/bash

# argv[1]: the number of keys
# argv[2]: the number of queries

green="\033[1;32m"
red="\033[1;31m"
default="\033[0m"

top_dir="../../"
build_dir="${top_dir}build/"
data="data/"
bin_dir="bin/"

test_generator="generator"
test_driver="driver"
ans_generator="ans_generator"

function build_from_sources
{
    local basic_options="-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-11"

    cmake ${top_dir} -B ${build_dir} ${basic_options}

    echo -en "\n"
    echo "Building test generator..."
    cmake --build ${build_dir} --target ${test_generator}
    echo -en "\n"

    echo "Building test driver..."
    cmake --build ${build_dir} --target ${test_driver}
    echo -en "\n"

    echo "Building generator of answers..."
    echo -en "\n"
    cmake --build ${build_dir} --target ${ans_generator}
    echo -en "\n"

    echo "Installing..."
    cmake --install ${build_dir}
    echo -en "\n"
}

function generate_test
{
    local n_keys=$1
    local n_queries=$2

    mkdir -p ${data}

    echo "Generating test..."
    ${bin_dir}${test_generator} ${n_keys} ${n_queries} > "${data}${n_keys}_${n_queries}.test"
    echo -en "\n"
}

function generate_answer
{
    local n_keys=$1
    local n_queries=$2

    echo "Generating answer..."
    ${bin_dir}${ans_generator} < "${data}${n_keys}_${n_queries}.test" \
                               > "${data}${n_keys}_${n_queries}.ans"
    echo -en "\n"
}

function run_test
{
    local n_keys=$1
    local n_queries=$2

    echo "Running test..."
    ${bin_dir}${test_driver} < "${data}${n_keys}_${n_queries}.test" \
                             > "${data}${n_keys}_${n_queries}.res"
    echo -en "\n"

    echo -en "Result: "
    if diff -Z "${data}${n_keys}_${n_queries}.ans" "${data}${n_keys}_${n_queries}.res" > /dev/null
    then
        echo -e "${green}passed${default}"
    else
        echo -e "${red}failed${default}"
    fi
}

if [ $# -ne 2 ]
then
    echo "Testing script requires exactly 2 arguments"
else
    n_keys=$1

    if [ $n_keys -le 0 ]
    then
        echo "The number of keys has to be a positive integer number"
    else
        n_queries=$2

        if [ $n_queries -le 0 ]
        then
            echo "The number of queries has to be a positive integer number"
        else
            build_from_sources
            generate_test $n_keys $n_queries
            generate_answer $n_keys $n_queries
            run_test $n_keys $n_queries
        fi
    fi
fi
