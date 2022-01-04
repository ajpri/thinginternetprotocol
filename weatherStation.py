#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import serial
import random
import string
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse


AuthKeys = []

AuthKeys.append("BetBFDayGG7Q0kqgBBiSaJkGa")

ApplicationName = "UHD Weather Station"
ApplicationVersion = "1.0"

def readValue(type):
    ser = serial.Serial("/dev/ttyACM0", 9600)
    cc = str(ser.readline())
    line=cc[2:][:-3]
    #print(line)
    lineList = line.split(";")
    print(lineList)
    ser.close()    
    return str(lineList[type - 1])

def writeButton():
    ser = serial.Serial("/dev/ttyACM0", 9600)
    ser.write("bp")
    ser.close()    

def generateAuth():
   letters = string.ascii_lowercase + string.ascii_uppercase + string.digits
   generatedAuth = ''.join(random.choice(letters) for i in range(25)) 
   AuthKeys.append(generatedAuth)


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
                   respStr = "a87;"

                   respStr = respStr[:-1]
                   self.wfile.write(respStr.encode())
               elif ((newPath[-1]=="names") and (newPath[-2]=="devices")):
                   self.send_header('Content-type', 'text/plain')
                   self.end_headers()
                   respStr = "default;"
                                    
                   respStr = respStr[:-1]
                   self.wfile.write(respStr.encode()) 
               elif ((newPath[1]=="devices") and (re.search(r'[0-9]+',newPath[2]))):
                   if len(newPath) == 3:
                       self.send_header('Content-type', 'text/plain')
                       self.end_headers()
                       respStr = "level;temperature;button;temperatureUnits;alarmSensor"
                       self.wfile.write(respStr.encode())
                   if len(newPath) == 4:
                       print("hit")
                       if newPath[3] == "temperature":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           respStr = readValue(1)
                           self.wfile.write(respStr.encode())  
                       elif newPath[3] == "level":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           respStr = readValue(2)
                           self.wfile.write(respStr.encode())
                       elif newPath[3] == "temperatureUnits":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = "F"
                           self.wfile.write(respStr.encode())
                       elif newPath[3] == "alarmSensor":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = ""
                           waterContent = int(readValue(5))
                           if waterContent >= 250:
                               respStr = "detected"
                           else:
                               respStr = "none"
                           self.wfile.write(respStr.encode())
                   if len(newPath) == 6:
                       print("section hit")
                       if newPath[5] == "readOnly":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           respStr = "true"
                           self.wfile.write(respStr.encode())  
                       elif newPath[5] == "press":
                           self.send_header('Content-type', 'text/plain')
                           self.end_headers()
                           writeButton()
                           self.wfile.write("press") 
                       
                
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
    
print(readValue(1))
main()