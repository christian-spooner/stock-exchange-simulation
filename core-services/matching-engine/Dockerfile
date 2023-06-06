FROM ubuntu:22.04

WORKDIR /usr/src/app

RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y libssl-dev
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y cmake
RUN apt-get install -y git

RUN git clone https://github.com/alanxz/rabbitmq-c.git \
    && cd rabbitmq-c \
    && mkdir build && cd build \
    && cmake .. \
    && cmake --build . \
    && cmake --build . --target install

RUN git clone https://github.com/alanxz/SimpleAmqpClient.git \
    && cd SimpleAmqpClient \
    && mkdir build && cd build \
    && cmake .. \
    && cmake --build . \
    && cmake --build . --target install

COPY src ./src
COPY include ./include
COPY libs ./libs
COPY Makefile ./

RUN make

ENV LD_LIBRARY_PATH /usr/local/lib:${LD_LIBRARY_PATH}

EXPOSE 2401

CMD sleep 20 && ./bin/main
