## Generic monero scanner

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
However, the first projct got discontinued, while the second, is a
command-line program which limits its widespread use. 

The generic monero scanner is solution to these issues.
 

## Technologies

The key technologies used in the scanner are:

 - C++17
 - websockets - support porovided by [drogon](https://github.com/an-tao/drogon)
 - [FiberPool](https://github.com/moneroexamples/fiberpool) - single-file header-only C++17 boost::fiber thread pool library 
 - [xmregcore](https://github.com/moneroexamples/xmregcore) - modular Universal Identifier and Account representation for monero.
 - googletest - unit testing and mocking framework

## Example compilation on Ubuntu 18.04


#### Monero download and compilation

Follow instructions in the following link:

https://github.com/moneroexamples/monero-compilation/blob/master/README.md

#### Html fronted 
The project comes with basic html fronted located in `html` folder. The easiet
way to server it over http is to use python's build in http server

```bash
cd html
python -m http.server 9100
```
Go to `http:/127.0.0.1:9100`


#### Program options

```bash
xmrscanner: generic monero scanner:
  -h [ --help ]                         Help
  -n [ --nettype ] arg (=2)             network type: 0-MAINNET, 1-TESTNET, 
                                        2-STAGENET
  -b [ --blockchain-path ] arg (=/home/mwo2/.bitmonero/stagenet/lmdb)
                                        Path to lmdb folder containing the 
                                        blockchain
  -p [ --port ] arg (=8848)             Service port
```


## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.
