#!/usr/bin/env python

# python 3 simple https server with SSL
# based on: https://blog.anvileight.com/posts/simple-python-http-server/
#
# Usage: Just run it in the root folder of a website
#
# Example:
# python ~/pserverssl.py -k /path/to/privkey1.pem -c /path/to/cert1.pem -a "0.0.0.0"
#
# To test use curl, e.g.
# curl -k https://yourdomainname:4443

import argparse
import os
import ssl
import sys

from http.server import HTTPServer, SimpleHTTPRequestHandler

def parse_arguments():

    parser = argparse.ArgumentParser(
               description='Simple HTTPS server.')
       
    parser.add_argument("-p", "--port", 
        help="ssl port", type=int, default=4443) 
    
    parser.add_argument("-a", "--address", 
        help="ssl port", type=str, default="0.0.0.0") 
    
    parser.add_argument("-k", "--privkey", 
               help="path to your private_key.pem", type=str) 

    parser.add_argument("-c", "--cert", 
               help="path to your certificate.pem", type=str) 

    args = parser.parse_args()

    if not args.privkey or not args.cert:
        sys.stderr.write("Path to private key pem"  
                + " and/or certificate files not given!\n")
        sys.exit(1)
    
    if not os.path.exists(args.cert):
        sys.stderr.write("Certificate pem file \"" 
            + args.cert + "\" does not exist!\n")
        sys.exit(1)
    
    if not os.path.exists(args.privkey):
        sys.stderr.write("Private key pem file \"" 
            + args.privkey + "\" does not exist!\n")
        sys.exit(1)

    return args


def main():

    args = parse_arguments()

    httpd = HTTPServer((args.address, args.port), SimpleHTTPRequestHandler)

    httpd.socket = ssl.wrap_socket (
            httpd.socket, 
            keyfile=args.privkey, 
            certfile=args.cert, 
            server_side=True)

    print("waiting for connections...")
    httpd.serve_forever()

if __name__ == '__main__':
    main()
