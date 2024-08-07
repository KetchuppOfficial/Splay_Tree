FROM ubuntu:24.04

RUN apt update --fix-missing

RUN apt install -y g++-14
RUN apt install -y make
RUN apt install -y cmake
RUN apt install -y git

RUN apt install -y libgtest-dev
RUN apt install -y libboost-program-options-dev

RUN git clone https://github.com/KetchuppOfficial/Splay_Tree.git

RUN cd Splay_Tree && \
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-14 && \
    cmake --build build -j12
