#!/usr/bin/python3
import sys

print("Content-Type: text/html")
print()

body = sys.stdin.read()

print("<h1>POST RECEIVED</h1>")
print("<pre>")
print(body)
print("</pre>")
