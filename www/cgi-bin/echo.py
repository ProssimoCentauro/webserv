#!/usr/bin/python3

import os
import sys

print("Content-Type: text/html\n")

print("<html><body>")
print("<h1>Echo CGI</h1>")

print("<h2>Environment:</h2>")
for k, v in os.environ.items():
    print(f"{k} = {v}<br>")

print("<h2>Body:</h2>")
data = sys.stdin.read()
print(data)

print("</body></html>")
