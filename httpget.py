import sys
import requests
command = sys.argv[1]
r =requests.get(command)
print(r.text)

