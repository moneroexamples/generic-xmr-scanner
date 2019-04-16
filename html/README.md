## Basic html client for the scanner

`index.html` contains the most basic html and JavaScript client 
to interact with the `xmrscanner` over the internet.

To server it over `http`, the easiest is to use python 3's build in http server

```bash
# go to where generic-xmr-scanner is located, e.g.,
cd ~/generic-xmr-scanner/html

# run python's http server on port 9100
python -m http.server 9100
```

Then just go to `http://127.0.0.1:9100` using your browser and you should see
the web interface.

