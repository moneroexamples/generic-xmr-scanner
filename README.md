## Generic Monero Scanner of Monero blockchain

The project aims to develop generic online scanner/searcher for monero. Its capability includes searching for:

 - outputs associated with given address and viewkey,
 - possible spendings based on only address and viewkey,
 - txs which use our outputs as decoys/ring members,
 - txs associated with given legacy payment ids,
 - multi-output txs to subaddresses,
 - txs with integrated payment ids,
 - and more

## Motivation


The idea is based on two projects:

 - [onion-monero-viewer](https://github.com/moneroexamples/onion-monero-viewer)
 - [transactions-export](https://github.com/moneroexamples/transactions-export)

The first project allowed for online identification of outputs 
associated with given address and viewkey. The second project additionally
searched for transactions which use the outputs as decoys/ring members. 
However, the first projcet got discontinued, while the second is a
command-line program which limits its widespread use. 

The generic monero scanner is solution to these issues.
 
## Technologies

The key technologies used in the scanner are:

 - [C++17](https://en.wikipedia.org/wiki/C%2B%2B17) - recent version of the C++ programming language.
 - [websockets](https://en.wikipedia.org/wiki/WebSocket) - support provided by [drogon](https://github.com/an-tao/drogon)
 - [FiberPool](https://github.com/moneroexamples/fiberpool) - single-file header-only C++17 [boost::fiber](https://github.com/boostorg/fiber) thread pool library 
 - [xmregcore](https://github.com/moneroexamples/xmregcore) - core utilities used in moneroexample projects.

## Example compilation on Ubuntu 18.04


#### Monero download and compilation

Follow instructions in the following link:

https://github.com/moneroexamples/monero-compilation/blob/master/README.md

#### Project download and compilation

```bash
git clone --recurse-submodules https://github.com/moneroexamples/generic-xmr-scanner

cd generic-xmr-scanner

mkdir build && cd build

cmake ..

# altearnatively can use cmake -DMONERO_DIR=/path/to/monero_folder ..
# if monero is not in ~/monero
```

Following the compilation, `xmrscanner` executable should be avaliable in the `build` folder. 

#### Html fronted 
The project comes with basic html fronted located in `html` folder. Read more here: 

- https://github.com/moneroexamples/generic-xmr-scanner/tree/master/html#basic-html-client-for-the-scanner

#### Program options

```bash
xmrscanner: generic monero scanner:
  -h [ --help ]                  Help
  -n [ --nettype ] arg (=2)      network type: 0-MAINNET, 1-TESTNET, 2-STAGENET
  -b [ --blockchain-path ] arg   Non-default path to lmdb folder containing the
                                 blockchain. Default is /home/mwo2/.bitmonero/s
                                 tagenet/lmdb
  -f [ --fiberthreads ] arg (=0) Number of fiberpool thread worksers. Default 
                                 is 0 - based on hardware
  -p [ --port ] arg (=8848)      Service port
```


## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.
