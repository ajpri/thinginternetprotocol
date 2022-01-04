#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

class device {
  public:
    string id;   
    string name;
    vector<string> types;
};

//Helper Funtion - Text Input
string textin(){
    string outStr = "";
    getline(cin, outStr);
    return outStr;
}

//Helper Funtion - Output Input
void println(string outStr){
    cout << outStr << endl;
}

//string to vector
vector <string> strtov(string input){
    vector<string> v;
 
    stringstream ss(input);
 
    while (ss.good()) {
        string substr;
        getline(ss, substr, ';');
        v.push_back(substr);
    }
    return v;
}

//Get HTTP GET Output
string httpGet(string url){
    url = "python3 httpget.py http://" + url;
    system((url + " > temp.txt").c_str());

    ifstream ifs("temp.txt");
    string ret{ istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() };
    ifs.close();
    if (remove("temp.txt") != 0) {
        perror("Error deleting temporary file");
    }
    return ret.erase(ret.size() - 1);
}

int main(int argc, char **argv) {
    string serverURL;
    string appName;
    string authStr;

    println("TIP TOOLKIT - Endpoint Client");
    println("Enter Server Address of TIP Server:");
    serverURL = textin();
    serverURL += ":2021/";
    println("Connecting to TIP Server " + serverURL);
    appName = httpGet(serverURL + "applicationName");
    println("Connected to: " + appName);
    println("Requesting Auth Code");
    string authCode = httpGet(serverURL + "generateAuth");
    println("authCode: " + authCode);
    authStr = "?authKey=" + authCode;
    println("Getting list of devices");
    
    string deviceIDString = httpGet(serverURL + "devices" + authStr);
    string deviceNameString = httpGet(serverURL + "devices/names" + authStr);
    //println(deviceIDString);
    //println(deviceNameString);
    vector<device> devices;

    vector<string> dIDs = strtov(deviceIDString);
    vector<string> dNames = strtov(deviceNameString);
    
    for (size_t i = 0; i < dIDs.size(); i++){
        //println("ID: " + dIDs[i] + "\t Name: " + dNames[i]);
        
        device newDevice;
        newDevice.id = dIDs[i];
        newDevice.name = dNames[i];
        string typeString = httpGet(serverURL + "devices/" + dIDs[i] + authStr);
        newDevice.types = strtov(typeString);
        devices.push_back(newDevice);
        
    }
   
    MENU_MAIN:
    println("\nPlease select the device you would like to control:");
    
    for (int i = 0; i < devices.size(); i++){
        println(to_string(i+1) + ": " + devices[i].name);
    }
    println("Z. Exit");
    
    string selection = textin();
    
    if(selection == "Z"){
        httpGet(serverURL + "revokeAuth" + authStr);
        println("exiting");
    }else{
        
        int selIndex = stoi(selection) - 1;
        device selectedDevice = devices[selIndex];
        println("Device: " + selectedDevice.name + " (ID = " + selectedDevice.id + ")");
        println("\nWhich type would you like to control?");
        
        for (int i = 0; i < selectedDevice.types.size(); i++){
            println(to_string(i+1) + ": " + selectedDevice.types[i]);
        }
        println("Z. return to Device Selection");
        string typeSelection = textin();
    
        if(typeSelection == "Z"){
            goto MENU_MAIN;
        }else{
            int typeSelIndex = 0;
            //println(to_string(typeSelIndex));
            typeSelIndex = stoi(typeSelection) - 1;
            string selectedType = selectedDevice.types[typeSelIndex];
            
            //println("You selected: " + selectedType);
            
            //if block for every type. Generate list 
            if(selectedType == "switch"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\ton");
                    println("2.\toff");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        //Turn device on
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/on" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/off" + authStr);
                    }
                }
            }else if(selectedType == "presence"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\tpresent");
                    println("2.\tnotPresent");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/present" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/notPresent" + authStr);
                    }
                }
            }else if(selectedType == "alarmSensor"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{ 
                    
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\tdetected");
                    println("2.\tnone");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/detected" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/none" + authStr);
                    }
                }
            }else if(selectedType == "contact"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\topen");
                    println("2.\tclosed");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/open" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/closed" + authStr);
                    }
                }
            }else if(selectedType == "lock"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\tlock");
                    println("2.\tunlock");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/lock" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/unlock" + authStr);
                    }
                }
            }else if(selectedType == "level"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    
                    println("Please enter new level 0-100");
                    string resp = textin();
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/" + resp + authStr);
                }
            }else if(selectedType == "temperature"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Please enter new temperature");
                    string resp = textin();
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/" + resp + authStr);
                }
            }else if(selectedType == "batteryLevel"){              
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Please enter new batteryLevel");
                    string resp = textin();
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/" + resp + authStr);
                }
            }else if(selectedType == "colorTemperature"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Please enter new colorTemperature (2700-6500");
                    string resp = textin();
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/" + resp + authStr);
                }
            }else if(selectedType == "rgbColor"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Please enter new rgbColor (#000000 - #FFFFFF");
                    string resp = textin();
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/" + resp + authStr);
                }
            }else if(selectedType == "powerType"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Which would you like to send to " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\tbattery");
                    println("2.\tmains");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/battery" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/mains" + authStr);
                    }
                }
            }else if(selectedType == "thermostatMode"){
                println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\theat");
                    println("2.\tcool");
                    println("2.\tauto");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/heat" + authStr);
                    }else if (resp == "2"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/cool" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/auto" + authStr);
                    }
                }
            }else if(selectedType == "fanMode"){
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\ton");
                    println("2.\toff");
                    println("2.\tauto");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/on" + authStr);
                    }else if (resp == "2"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/off" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/auto" + authStr);
                    }
                }
            }else if(selectedType == "temperatureUnits"){
                if(httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/readOnly" + authStr) == "true"){
                    println("Value is read only - Cannot be modified");
                }else{                 
                    println("Current State: " + httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + authStr));
                    println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                    println("1.\tFahrenheit");
                    println("2.\tCelcius");
                    
                    string resp = textin();
                    
                    if(resp == "1"){
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/F" + authStr);
                    }else{
                        httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/C" + authStr);
                    }
                }
            }else if(selectedType == "button"){
                println("Which command would you like to perform on " + selectedDevice.name + "(" + selectedType + ")?");
                println("1.\tpress");
                println("2.\tdoublePress");
                println("2.\theld");
                
                string resp = textin();
                
                if(resp == "1"){
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/press" + authStr);
                }else if (resp == "2"){
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/doublePress" + authStr);
                }else{
                    httpGet(serverURL + "devices/" + selectedDevice.id + "/" + selectedType + "/0/held" + authStr);
                }
            }
           
            
            
            goto MENU_MAIN;
        }
        
        
    }
       
    
    return 0;


}



