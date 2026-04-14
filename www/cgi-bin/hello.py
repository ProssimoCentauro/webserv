#!/usr/bin/env python3

import os

print("Content-Type: text/html\n")

print("<h1>🐍 Python CGI Works!</h1>")
print("<p>Method: " + os.environ.get("REQUEST_METHOD", "") + "</p>")
print("<p>Content-Length: " + os.environ.get("CONTENT_LENGTH", "") + "</p>")
