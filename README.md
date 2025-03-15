# HWT. Level 2: threaded splay tree

This project is a task on course "Uses and applications of C++ language" by
[K.I.Vladimirov](https://github.com/tilir). The task was to implement a class representing splay
tree.

During code review of an intermediate version of this project it was mentioned that `operator++`
and `operator--` of the iterator over my tree have *O(log(n))* time complexity that contradicts
standard requirements. Thus, I implemented a threaded splay tree instead of a regular one.

## Requirements

The following applications and libraries have to be installed:

- CMake of version 3.20 (or higher)
- python3
- python3-venv
- pip

There is a [Dockerfile](/Dockerfile) provided in case you wish to isolate the build.

## How to install

```bash
git clone git@github.com:KetchuppOfficial/Splay_Tree.git
cd Splay_Tree
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Splay_Tree/)

### 1) Installing conan and other python modules this project depends on

```bash
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt
```

### 2) Build the project

```bash
conan install . --output-folder=third_party --build=missing -s compiler.cppstd=23
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./third_party/conan_toolchain.cmake
cmake --build build [--target <tgt>]
```

**tgt** can be

- **driver**: a program that takes a test as an input, runs it on a tree and produces the answer;
- **generator**: a program that generates a test;
- **unit_tests**: self-explanatory.

If --target option is omitted, all targets will be built.

## How to run unit tests

```bash
ctest --test-dir build
```

## How to run end-to-end tests

If you want to run some tests on my tree, look at [checker.py](/test/end_to_end/checker.py).

```bash
checker.py --help
# usage: checker.py [-h] --install-dir PATH --tree {splay,splay+} -k N -q N
#
# This script is a utility for easy end-to-end testing of splay tree
#
# options:
#   -h, --help            show this help message and exit
#   --install-dir PATH    path to the directory containing all installed executables produced by the build system
#   --tree {splay,splay+}
#                         the tree to use in tests
#   -k N, --keys N        the number of random keys in test
#   -q N, --queries N     the number of random queries in test
```

The script generates a test with given number of keys (**N**) and queries (**M**). The test is saved
in **N_M.test**. After that this script runs **driver** on `std::set`, gets answers that are
supposed to be correct and saves them in file **N_M.ans**. Then, **driver** on the splay tree or
augmented splay tree or does the same. The results are saved in **N_M.res**. Finally, files
**N_M.ans** and **N_M.res** are compared. If they differ, then this test is considered "failed". It
is considered "passed" otherwise.

**tree** argument has to be of value **splay** for testing splay tree or **splay+** for testing
augmented splay tree.

All above mentioned files locate in *test/end_to_end/data* directory.

Before running the test **driver** and **generator** should be placed in a single directory. It can
be achieved by running the following command:

```bash
cmake --install build --prefix .
```

If you want to run tests manually:

```bash
./bin/driver --help
# Program that runs a benchmark from stdin on a search tree
# Usage: ./bin/driver [OPTIONS]
#
# Options:
#   -h,--help                   Print this help message and exit
#   -t,--time                   Measure execution time of the benchmark
#   -a,--answers                Print answers to the given range queries
#   --tree TEXT:{std::set,splay,splay+} REQUIRED
#                               The type of search tree to run benchmark on
```

Usage example:

```bash
./bin/driver -a --tree splay+
```

To have a test one might find **generator** useful. It has the following options:

```bash
./bin/generator --help
# Splay tree end-to-end tests generator
# Usage: ./bin/generator [OPTIONS]
#
# Options:
#   -h,--help                   Print this help message and exit
#   --n-keys UINT REQUIRED      Set the number of keys to generate
#   --n-queries UINT REQUIRED   Set the number of queries to generate
```

The **generator** generates a test of the format:

```text
((k\s-?\d+)|(q\s-?\d+\s-?\d+))+
```

Query **k N** means to insert the key **N** into the tree.

Query **q Q1 Q2** means to return the number of keys that belong to segment **[Q1; Q2]** if
**Q1** is less than or equal to **Q2** or return 0 otherwise.

## Behold... Threaded splay tree

![dump](/images/splay_tree.png)

## And its big bro containing subtree sizes

![dump](/images/augmented_splay_tree.png)
