FROM ubuntu

RUN apt-get update

RUN apt-get install -y g++
RUN apt-get install -y make
RUN apt-get install -y cmake
RUN apt-get install -y git

RUN apt-get install -y libgtest-dev
RUN apt-get install -y libboost-program-options-dev
RUN apt-get install -y libfmt-dev

RUN git clone https://github.com/KetchuppOfficial/Splay_Tree.git

RUN cd Splay_Tree && \
    cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j12
