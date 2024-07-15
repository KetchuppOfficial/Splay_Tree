# HWT. Level 2: threaded splay tree

This project is a task on course "Uses and applications of C++ language" by
[K.I.Vladimirov](https://github.com/tilir). The task was to implement a class representing splay
tree.

During code review of an intermediate version of this project it was mentioned that `operator++`
and `operator--` of the iterator over my tree have *O(log(n))* time complexity that contradicts
standard requirements. Thus, I implemented a threaded splay tree instead of a regular one.

## Requirements

The following application has to be installed:

- CMake of version 3.20 (or higher)

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

**tgt** can be **driver**, **augmented_driver**, **generator** or **ans_generator**.
The **generator** generates queries of the format:

```
N K1 ... KN M Q11 Q12 ... QM1 QM2
```

where **N** - the number of keys; **K1**, ..., **KN** - keys; **M** - the number of queries;
**Q11** **Q12**, ..., **QM1** **QM2** - queries.

To answer a query **Q1** **Q2** means to return the number of keys that belong to segment
**[Q1; Q2]**.

The **driver** is a program that receives queries for Splay_Tree from stdin and prints the answers
on stdout.

The **augmented_driver** is the same as **driver** except that splay tree there contains sizes of
subtrees in each node and is called Augmented_Splay_Tree.

The **ans_generator** is a program that does the same as **driver** but uses std::set.

If --target option is omitted, all targets will be built.

## How to run unit tests

```bash
ctest --test-dir build
```

## How to run end-to-end tests

If you want to run some tests on my tree, look at [test/end_to_end](/test/end_to_end/) directory.
There you will find a special script **checker.sh** provided for such purpose.

Let **N** be the number of keys and **M** be the number of queries, then command sequence:

```bash
./test/end_to_end/checker.sh tree N M
```

generates a test with **N** keys and **M** queries. The test is saved in **N_M.test**. After that
this script runs **ans_generator**, gets answers that are supposed to be correct and saves them in
file **N_M.ans**. Then, **driver** or **augmented_driver** (depending on **tree**) does the same.
The results are saved in **N_M.res**. Finally, files **N_M.ans** and **N_M.res** are compared. If
they differ, then this test is considered "failed". It is considered "passed" otherwise.

**tree** argument has to be of value **splay** for testing splay tree or **splay+** for testing
augmented splay tree.

P.s. all above mentioned files locate in test/end_to_end/data directory.

P.p.s. **driver**, **augmented_driver** and **ans_generator** measure the time spent on running a
test. This information is saved in **splay_tree.info**, **augmented_splay_tree.info** and
**std_set.info** files respectively.

## Behold... Threaded splay tree

![dump](/images/splay_tree.png)

## And its big bro containing subtree sizes

![dump](/images/augmented_splay_tree.png)
