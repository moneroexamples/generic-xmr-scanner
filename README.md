# Generic Monero Scanner

The project aims to develop generic online scanner/searcher for Monero. Its possible capability includes searching for:

 - outputs associated with given address and viewkey (already implemented with full support for subaddresses),
 - possible spendings based on only address and viewkey,
 - transactions which use our outputs as decoys/ring members,
 - transactions associated with given legacy payment ids,
 - multi-output transactions to subaddresses,
 - transactions with integrated payment ids,
 - and more ...

## Screenshot

![Generic Monero Scanner](/img/screenshot.png)

## Motivation

The idea is based on two projects:

 - [onion-monero-viewer](https://github.com/moneroexamples/onion-monero-viewer)
 - [transactions-export](https://github.com/moneroexamples/transactions-export)

The first project allowed for online identification of outputs associated with 
given address and viewkey. The second project additionally
searched for transactions which use the outputs as decoys/ring members. 
However, the first project got discontinued, while the second is a
command-line program which limits its widespread use. 

The generic Monero scanner is solution to these issues.

## Technologies

The key technologies used in the scanner are:

- [C++17](https://en.wikipedia.org/wiki/C%2B%2B17) - recent version of the C++ programming language.
- [websockets](https://en.wikipedia.org/wiki/WebSocket) -  protocol for duplex communication between frontend and backend (support provided by [drogon](https://github.com/an-tao/drogon))
- [FiberPool](https://github.com/moneroexamples/fiberpool) - single-file header-only C++17 [boost::fiber](https://github.com/boostorg/fiber) thread pool library
- [xmregcore](https://github.com/moneroexamples/xmregcore) - core utilities used in moneroexample projects.

## Example compilation on Ubuntu 18.04

[![CircleCI](https://circleci.com/gh/moneroexamples/generic-xmr-scanner.svg?style=svg)](https://circleci.com/gh/moneroexamples/generic-xmr-scanner)

### Monero download and compilation

To compile the project, most recent Monero development version (i.e., its `master` branch)
needs to be compiled first. 

Instruction for compiling monero are here: https://github.com/moneroexamples/monero-compilation

Using `USE_SINGLE_BUILDDIR` flag is important. Without the flag, cmake of the project
will not be able to locate the compiled monero.

### Project download and compilation

```bash
# go to home folder
cd ~

# drogon dependencies
sudo apt install libjsoncpp-dev uuid-dev

git clone --recurse-submodules https://github.com/moneroexamples/generic-xmr-scanner

cd generic-xmr-scanner

mkdir build && cd build

cmake ..

# altearnatively can use cmake -DMONERO_DIR=/path/to/monero_folder ..
# if monero is not in ~/monero

make
```

Following the compilation, `xmrscanner` executable should be available in the `build` folder.

#### Html fronted

The project comes with basic html fronted located in `html` folder. Basic instruction for running `xmrscanner` over `http` or `https` (SSL) protocoles are given here:

- https://github.com/moneroexamples/generic-xmr-scanner/tree/master/html#basic-html-client-for-the-scanner

#### Using Docker

Instead of building and compiling everything from scrach your own, you can use `Docker`. You still need to have full blockchain on your host, which you share with the docker. Do docker containers do not run monero themselfs. So basically you still need to have full node before you can run the `xmrscanner`.

```
git clone --recurse-submodules https://github.com/moneroexamples/generic-xmr-scanner

cd generic-xmr-scanner

# build backend image (using 2 cpus)
docker build --build-arg NPROC=2  . -t xmrscannerbackend

# build frontend image
docker build ./html -t xmrscannerfrontend
```

When the two images are build, you can run containers for backend and frontend:

```
# run backend container on mainnet (-n 0),
docker run -it -v <path-to-monero-blockckain-on-the-host>:/home/monero/.bitmonero -p 8848:8848  xmrscannerbackend "./xmrscanner -n 0"

# run frontend container
docker run -it -p 9100:80  xmrscannerfrontend
```

Once the containers are running, open browser: `http://0.0.0.0:9100/`.


#### Program options

```bash
xmrscanner: generic monero scanner:
  -h [ --help ]                         Help
  -n [ --nettype ] arg (=2)             network type: 0-MAINNET, 1-TESTNET, 
                                        2-STAGENET
  -b [ --blockchain-path ] arg          Non-default path to lmdb folder 
                                        containing the blockchain. Default is 
                                        /home/mwo2/.bitmonero/stagenet/lmdb
  -d [ --da-path ] arg (=./config/default_addresses.json)
                                        Non-default path to default_addresses.j
                                        son file
  -c [ --config-path ] arg (=./config/config.json)
                                        Non-default path to config.json file
  -f [ --fiberthreads ] arg (=0)        Number of fiberpool thread workers. 
                                        Default is 0 - based on hardware
  -p [ --port ] arg (=8848)             Service port
```

## How can you help

Constructive criticism, code and website edits are always good. They can be made through github.
