# Basic html client for the scanner

`index.html` contains the basic html and JavaScript client 
to interact with the `xmrscanner` over the Internet. The design of the `index.html` was based on the [Restbed](https://github.com/Corvusoft/restbed) example code provided [here](https://github.com/Corvusoft/restbed/blob/master/documentation/example/WEB_SOCKET.md#client).

## HTTP

The easiest way to serve it is to use  python 3's build in `http` server:

```bash
# go to where generic-xmr-scanner is located, e.g.,
cd ~/generic-xmr-scanner/html

# run python 3 http server on port 9100
python -m http.server 9100

# for python 2 use
# python -m SimpleHTTPServer 9100
```

Then just go to `http://127.0.0.1:9100` using your browser and you should see the web interface.

## HTTPS (SSL encryption)

The `xmrscanner` supports ssl encription. To enable the support the following three things needs to be performed:

1. Enable ssl support in [config.json](https://github.com/moneroexamples/generic-xmr-scanner/blob/master/config/config.json) and provide paths to your `privkey.pem` and `cert.pem`.
2. In [index.html](https://github.com/moneroexamples/generic-xmr-scanner/blob/master/html/index.html):
   - use domain for which you got your cerficate for `window.xmrscanner_base_url`.
   - change `"http://"` to `"https://"` for `window.xmrscanner_url_http`.
   - change `ws://` to `wss://` for `window.xmrscanner_url_ws`.
3. For basic `https` server in python 3 use the provided python script [pserverssl.py](https://github.com/moneroexamples/generic-xmr-scanner/blob/master/scripts/pserverssl.py). It should be run in the `generic-xmr-scanner/html/` folder.
