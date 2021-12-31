# Use ubuntu:20.04 as base for builder stage image
# Dockerfile addapted from https://github.com/moneroexamples/onion-monero-blockchain-explorer/pull/254
# Example:
# docker build  -t xmrscanner .
# docker run -it -v /mnt/w7/bitmonero:/home/monero/.bitmonero -p 8848:8848  xmrscanner
# For stagenet: docker run -it -v /mnt/w7/bitmonero:/home/monero/.bitmonero -p 8848:8848  xmrscanner "./xmrscanner -n 2"
#
FROM ubuntu:20.04 as builder

# Set Monero branch/tag to be used for monerod compilation
ARG MONERO_BRANCH=master

# Added DEBIAN_FRONTEND=noninteractive to workaround tzdata prompt on installation
ENV DEBIAN_FRONTEND="noninteractive"

# Install dependencies for monerod and xmrscanner compilation
RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y --no-install-recommends \
    git \
    build-essential \
    cmake \
    miniupnpc \
    graphviz \
    libjsoncpp-dev uuid-dev \
    doxygen \
    pkg-config \
    ca-certificates \
    zip \
    libboost-all-dev \
    libunbound-dev \
    libunwind8-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    libgtest-dev \
    libreadline-dev \
    libzmq3-dev \
    libsodium-dev \
    libhidapi-dev \
    libhidapi-libusb0 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set compilation environment variables
ENV CFLAGS='-fPIC'
ENV CXXFLAGS='-fPIC'
ENV USE_SINGLE_BUILDDIR 1
ENV BOOST_DEBUG         1

WORKDIR /root

# Clone and compile monerod with all available threads
ARG NPROC
RUN git clone --recursive --branch ${MONERO_BRANCH} https://github.com/monero-project/monero.git \
    && cd monero \
    && test -z "$NPROC" && nproc > /nproc || echo -n "$NPROC" > /nproc && make -j"$(cat /nproc)"


# Copy and cmake/make xmrscanner with all available threads
COPY . /root/generic-xmr-scanner/
WORKDIR /root/generic-xmr-scanner/build
RUN cmake .. && make -j"$(cat /nproc)"

# Use ldd and awk to bundle up dynamic libraries for the final image
RUN zip /lib.zip $(ldd xmrscanner | grep -E '/[^\ ]*' -o)

# Use ubuntu:20.04 as base for final image
FROM ubuntu:20.04

# Added DEBIAN_FRONTEND=noninteractive to workaround tzdata prompt on installation
ENV DEBIAN_FRONTEND="noninteractive"

# Install unzip to handle bundled libs from builder stage
RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y --no-install-recommends unzip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /lib.zip .
RUN unzip -o lib.zip && rm -rf lib.zip

# Add user and setup directories for monerod and xmrscanner
RUN useradd -ms /bin/bash monero \
    && mkdir -p /home/monero/.bitmonero \
    && chown -R monero:monero /home/monero/.bitmonero
USER monero

# Switch to home directory and install newly built xmrscanner binary
WORKDIR /home/monero
COPY --chown=monero:monero --from=builder /root/generic-xmr-scanner/build/xmrscanner .
COPY --chown=monero:monero --from=builder /root/generic-xmr-scanner/build/config ./config/

# Expose volume used for lmdb access by xmrscanner
VOLUME /home/monero/.bitmonero

# Expose default xmrscanner http port
EXPOSE 8848

ENTRYPOINT ["/bin/sh", "-c"]

# Set sane defaults that are overridden if the user passes any commands
CMD ["./xmrscanner", "-n", "0"]
