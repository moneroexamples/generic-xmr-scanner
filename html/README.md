## Basic html client for the scanner

`index.html` contains the most basic html and JavaScript client 
to interact with the `xmrscanner` over the Internet. The design of the `index.html` was based on the [Restbed](https://github.com/Corvusoft/restbed) example code provided [here](https://github.com/Corvusoft/restbed/blob/master/documentation/example/WEB_SOCKET.md#client).

The easiest way to serve it is to use  python 3's build in `http` server:

```bash
# go to where generic-xmr-scanner is located, e.g.,
cd ~/generic-xmr-scanner/html

# run python 3 http server on port 9100
python -m http.server 9100

# for python 2 use
# python -m SimpleHTTPServer 9100
```

Then just go to `http://127.0.0.1:9100` using your browser and you should see
the web interface.

