#!/bin/bash

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
top_dir="${script_dir}/../.."
build_dir="${top_dir}/build"
data="${script_dir}/data"
bin_dir="${build_dir}/test/end_to_end"

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

    local basic_options="-DCMAKE_BUILD_TYPE=Release"

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
}

function generate_test
{
    local n_keys=$1
    local n_queries=$2

    mkdir -p ${data}

    echo -e "${green}Generating test...${default}"
    "${bin_dir}/${test_generator}" --n-keys ${n_keys} --n-queries ${n_queries} > "${data}/${n_keys}_${n_queries}.test"
    echo -en "\n"
}

function generate_answer
{
    local n_keys=$1
    local n_queries=$2

    echo -e "${green}Generating answer...${default}"
    "${bin_dir}/${ans_generator}" < "${data}/${n_keys}_${n_queries}.test" \
                                  > "${data}/${n_keys}_${n_queries}.ans"
    echo -en "\n"
}

function run_test
{
    local n_keys=$1
    local n_queries=$2

    echo -e "${green}Running test...${default}"
    "${bin_dir}/${test_driver}" < "${data}/${n_keys}_${n_queries}.test" \
                                > "${data}/${n_keys}_${n_queries}.res"
    echo -en "\n"

    if diff -Z "${data}/${n_keys}_${n_queries}.ans" "${data}/${n_keys}_${n_queries}.res" > /dev/null
    then
        echo -e "${green}Test passed${default}"
    else
        echo -e "${red}Test failed${default}"
    fi
}

source $script_dir/opts.sh
build_from_sources $tree
generate_test $n_keys $n_queries
generate_answer $n_keys $n_queries
run_test $n_keys $n_queries
