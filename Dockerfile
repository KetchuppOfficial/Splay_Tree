FROM ubuntu:latest

RUN apt-get update

RUN apt-get install -y g++ make cmake
RUN apt-get install -y python3 python3-venv pip
