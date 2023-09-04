# HWT. Level 2: splay tree

This project is a task on course "Uses and applications of C++ language" by [K.I.Vladimirov](https://github.com/tilir). The task was to implement a class representing splay tree.

## Requirements

The following application have to be installed:
- CMake of version 3.15 (or higher)

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
cmake --build build
```

## How to run unit tests
```bash
ctest --test-dir build
```

# Behold... Splay tree

![dump](/images/splay_tree.png)

# And its big bro containing subtree sizes

![dump](/images/augmented_splay_tree.png)
