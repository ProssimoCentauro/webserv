#!/usr/bin/python3

import sys

print("Content-Type: text/html\n")

data = sys.stdin.read()

print("<html><body>")
print("<h1>POST DATA:</h1>")
print("<pre>" + data + "</pre>")
print("</body></html>")
