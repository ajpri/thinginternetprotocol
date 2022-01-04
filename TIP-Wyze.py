import random
import string
import re
from wyze_sdk import Client
from wyze_sdk.errors import WyzeApiError
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse



client = Client(email="[REDACTED]", password="[REDACTED]")

TIPBulbs = []
AuthKeys = []

AuthKeys.append("BetBFDayGG7Q0kqgBBiSaJkGa")

ApplicationName = "Wyze Extender"
ApplicationVersion = "0.01"

try:
    response = client.devices_list()
    for device in client.devices_list():
        if device.product.model == "WLPA19":
            TIPBulbs.append(device.mac)
except WyzeApiError as e:
    # You will get a WyzeApiError is the request failed
    print(f"Got an error: {e}")
    
def generateAuth():
   letters = string.ascii_lowercase + string.ascii_uppercase + string.digits
   generatedAuth = ''.join(random.choice(letters) for i in range(25)) 
   AuthKeys.append(generatedAuth)


generateAuth()

# Get Switch (Mac)

def getSwitch(macAdd):
    if client.bulbs.info(device_mac=macAdd).is_on:
        return "on"
    else: 
        return "off"
    
def getColorTemp(macAdd):
    #bulb = client.bulbs.info(device_mac=macAdd)
    return client.bulbs.info(device_mac=macAdd).color_temp

def getLevel(macAdd):
    return client.bulbs.info(device_mac=macAdd).brightness

def getName(macAdd):
    return client.bulbs.info(device_mac=macAdd).nickname

#print(getSwitch(TIPBulbs[1]))
#print(getColorTemp(TIPBulbs[1]))
#print(getLevel(TIPBulbs[1]))

def setSwitch(macAdd, state):
    if state == "on":
        client.bulbs.turn_on(device_mac=macAdd, device_model="WLPA19")
    else:
        client.bulbs.turn_off(device_mac=macAdd, device_model="WLPA19")
        
def setColorTemp(macAdd, state):
    client.bulbs.set_color_temp(device_mac=macAdd, device_model="WLPA19", color_temp=state)
    
def setLevel(macAdd, state):
    client.bulbs.set_brightness(device_mac=macAdd, device_model="WLPA19", brightness=state)



class HTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Send response status code
        self.send_response(200)

        #unauthenticated endpoints
        if self.path == '/applicationName':
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(ApplicationName.encode())
        elif self.path == '/applicationVersion':
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(ApplicationVersion.encode())      
        elif self.path == '/generateAuth':
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            generateAuth()
            self.wfile.write(AuthKeys[-1].encode()) 
        else:
            query = urlparse(self.path).query
            query_components = dict(qc.split("=") for qc in query.split("&"))
            authKey = query_components["authKey"]
            
            if authKey in AuthKeys:
               #authenticated sucessfully        
               nPath = re.sub(r'[?][a][u][t][h][K][e][y][=][a-zA-Z0-9]+',"",self.path)
               newPath = nPath.split('/')
               print("Updated Path: ")
               print(newPath)
               print("len" + str(len(newPath)))
               if newPath[1]=="revokeAuth":
                   AuthKeys.remove(authKey)
                   self.send_header('Content-type', 'text/plain')
                   self.end_headers()
                   self.wfile.write("removed".encode())
               elif newPath[-1]=="devices":
                   self.send_header('Content-type', 'text/plain')
                   self.end_headers()
                   respStr = ""
                   for i in range(len(TIPBulbs)):
                       respStr += str(i) + ";"
                   respStr = respStr[:-1]
                   self.wfile.write(respStr.encode())
               elif ((newPath[-1]=="names") and (newPath[-2]=="devices")):
                   self.send_header('Content-type', 'text/plain')
                   self.end_headers()
                   respStr = ""
                  
                   for bulbMAC in TIPBulbs:
                       respStr += getName(bulbMAC) + ";"
                  
                   respStr = respStr[:-1]
                   self.wfile.write(respStr.encode())    
               elif ((newPath[1]=="devices") and (re.search(r'[0-9]+',newPath[2]))):
                   if len(newPath) == 3:
                       self.send_header('Content-type', 'text/plain')
                       self.end_headers()
                       respStr = "switch;level;colorTemperature"
                       self.wfile.write(respStr.encode())
                   if len(newPath) == 4:
                       print("hit")
                       if newPath[3] == "switch":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           respStr = getSwitch(TIPBulbs[int(newPath[2])])
                           self.wfile.write(respStr.encode())  
                       elif newPath[3] == "level":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           respStr = str(getLevel(TIPBulbs[int(newPath[2])]))
                           self.wfile.write(respStr.encode())
                       elif newPath[3] == "colorTemperature":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           respStr = str(getColorTemp(TIPBulbs[int(newPath[2])]))
                           self.wfile.write(respStr.encode())  
                   if len(newPath) == 6:
                       print("section hit")
                       if newPath[5] == "readOnly":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = "false"
                           self.wfile.write(respStr.encode())  
                       elif newPath[3] == "switch":
                           if newPath[5] == "on":                               
                               setSwitch(TIPBulbs[int(newPath[2])],"on")
                               self.send_header('Content-type', 'text/plain')
                               self.end_headers()
                               respStr = "on"
                               self.wfile.write(respStr.encode())  
                           elif newPath[5] == "off":
                               setSwitch(TIPBulbs[int(newPath[2])],"off")
                               self.send_header('Content-type', 'text/plain')
                               self.end_headers()
                               respStr = "off"
                               self.wfile.write(respStr.encode())
                       elif newPath[3] == "level":
                               self.send_header('Content-type', 'text/plain')
                               self.end_headers()
                               setLevel(TIPBulbs[int(newPath[2])], newPath[5])
                               respStr = newPath[5]
                               self.wfile.write(respStr.encode())
                       elif newPath[3] == "colorTemperature":
                               self.send_header('Content-type', 'text/plain')
                               self.end_headers()
                               setColorTemp(TIPBulbs[int(newPath[2])], newPath[5])
                               respStr = newPath[5]
                               self.wfile.write(respStr.encode())
                
        return
    
def main():
    print('Starting Server')
    server_address = ('0.0.0.0', 2021)
    httpd = HTTPServer(server_address, HTTPServer_RequestHandler)
    print('Server Running')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    print("Stopping Server")
    httpd.server_close()
    
main()
