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
- gtest library
- boost::program_options
- fmt library

There is a [Dockerfile](/Dockerfile) presented for your convenience.

## How to install

```bash
git clone git@github.com:KetchuppOfficial/Splay_Tree.git
cd Splay_Tree
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Splay_Tree/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build [--target <tgt>]
```

**tgt** can be

- **driver**: a program that takes a test as an input, runs it on the splay tree and produces the
              answer;
- **augmented_driver**: the same as **driver** except that augmented splay tree is used;
- **std_driver**: the same as **driver** except that `std::set` is used;
- **stopwatch**: a program that takes a test as an input, runs it on the splay tree and produces
                 execution time;
- **augmented_stopwatch**: the same as **stopwatch** except that augmented splay tree is used;
- **std_stopwatch**: the same as **stopwatch** except that `std::set` is used;
- **generator**: a program that generates a test;
- **unit_tests**: self-explanatory.

If --target option is omitted, all targets will be built.

The **generator** generates queries of the format:

```
((k\s-?\d+)|(q\s-?\d+\s-?\d+))+
```

Query **k N** means to insert the key **N** into the tree.

Query **q Q1 Q2** means to return the number of keys that belong to segment **[Q1; Q2]** if
**Q1** is less than or equal to **Q2** or return 0 otherwise.

## How to run unit tests

```bash
ctest --test-dir build
```

## How to run end-to-end tests

If you want to run some tests on my tree, look at [test/end_to_end](/test/end_to_end/) directory.
There you will find a special script **checker.sh** provided for such purpose.

```bash
Usage: /path/to/checker.sh [OPTIONS]

-m <testing-mode>         Set the tree to test on: <splay> or <splay+>
-k <number-of-keys>       Set the number of keys for the test
-q <number-of-queries>    Set the number of queries for the test
```

Example of usage:

```bash
./test/end_to_end/checker.sh -m tree -k N -q M
```

where **tree** is either *splay* or *splay+*; **N**, and **M** are positive integer numbers.

The script generates a test with **N** keys and **M** queries. The test is saved in **N_M.test**.
After that this script runs **ans_generator**, gets answers that are supposed to be correct and saves
them in file **N_M.ans**. Then, **driver** or **augmented_driver** (depending on **tree**) does the same.
The results are saved in **N_M.res**. Finally, files **N_M.ans** and **N_M.res** are compared. If
they differ, then this test is considered "failed". It is considered "passed" otherwise.

**tree** argument has to be of value **splay** for testing splay tree or **splay+** for testing
augmented splay tree.

P.s. all above mentioned files locate in test/end_to_end/data directory.

If you want to run tests manually, you can install all targets except for **unit_tests** in *./bin*.

```bash
cmake --install build
```

and then pass tests to drivers and stopwatches as follows:

```bash
./bin/driver < /path/to/file/with/test
```

To have a test one might find **generator** useful. It has the following options:

```bash
./bin/generator --help
# Allowed options:
#   --help                Produce help message
#   --n-keys arg          Set the number of keys to generate
#   --n-queries arg       Set the number of queries to generate
```

## Behold... Threaded splay tree

![dump](/images/splay_tree.png)

## And its big bro containing subtree sizes

![dump](/images/augmented_splay_tree.png)
