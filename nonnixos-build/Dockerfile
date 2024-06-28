FROM debian:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    libgl1-mesa-dev \
    pkg-config \
    libsfml-dev

WORKDIR /usr/src/app

RUN git clone https://github.com/blueOkiris/wordup-fe .
RUN git submodule update --init --recursive
RUN make -j$(nproc)

# Mount $(pwd) as /output
ENTRYPOINT [ "cp", "wordup-fe.bin", "/output" ]
