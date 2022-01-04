


/*
 *  Use:    Reads in atmospheric conditions and displays results on lcd screen 
 *  Input:  DHT11 (temp and humidity), CDS*3 (light levels), Push Button (pressed/not pressed)
 *  Output: LCD screen
 */
 
#include <LiquidCrystal.h>
#include <dht11.h>

dht11 DHT;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define lightSensor A5
#define waterSensor A3
#define gasSensor A0
#define DHT11_PIN 13


void setup(){
  //button
  pinMode(8, INPUT_PULLUP);

  //rgb led
  pinMode(9, OUTPUT);   //red
  pinMode(10, OUTPUT);  //blue
  
  Serial.begin(9600);
  // lcd dimensions
  //Serial.println("Startup");
  lcd.begin(16, 2);
   lcd.setCursor(0,0);
  lcd.print("Startup");
}

//records and stores button pressed status
int lastButton = 0;
//determines which screen should be displayed
int screenDisplay = 1;

void loop(){
  /*
   * read in sensor values (DHT, photo, and button
   */
  //read in button status
  lastButton = digitalRead(8);
  // read DHT11 for temp and humidity
  int chk = DHT.read(DHT11_PIN);
  // read CDS*3 for light intensity
  int lightLevel = analogRead(lightSensor);
  // read gas sensor
  int gasLevel = analogRead(gasSensor);
  // read water sensor
  int waterLevel = analogRead(waterSensor);
  // read humidity
  int humidity = DHT.humidity;

  


  //convert from Celcius to Farenheit
  int temp = (DHT.temperature * 9/5) + 32;
  //TODO: convert light sensor to readable display


  //determine if button was pressed to change the screen
  if(lastButton == LOW){
    //flip screen
    if(screenDisplay == 4){
      screenDisplay = 1;
    }
    else{
      screenDisplay ++;
    }
    //clear last display
    lcd.clear();
  }

  /*
   * screen controls
   * 1 = humidity
   * 2 = light level
   * 3 = gas sensor
   * 4 = water depth
   */
  //temp to lcd screen first row
  lcd.setCursor(0,0);
  lcd.print("Temp = ");
  lcd.print(temp);
  lcd.print("'F");

  switch (screenDisplay) {
  case 1:
    //humidity to second row of lcd
    lcd.setCursor(0,1);
    lcd.print("Humidity = ");
    lcd.print(humidity);
    lcd.print("%");
    break;
  case 2:
    //light to second row of lcd
    lcd.setCursor(0,1);
    lcd.print("Light lvl = ");
    lcd.print(lightLevel);
    break;
  case 3:
    //gas to second row of lcd
    lcd.setCursor(0,1);
    lcd.print("Gas lvl = ");
    lcd.print(gasLevel);
    break;
  case 4:
    //water depth to second row of lcd
    lcd.setCursor(0,1);
    lcd.print("                             ");
    lcd.setCursor(0,1);
    lcd.print("Water: ");

    if(waterLevel < 450){
      lcd.print("none");
    }
    else if(waterLevel >= 450 & waterLevel <620){
      lcd.print("low");
    }
    else if(waterLevel >= 620 & waterLevel <670){
      lcd.print("mid");
    }
    else{
      lcd.print("high");
    }
    break;
  default:
    // error, should be either 1 2 3 or 4
    lcd.setCursor(0,1);
    lcd.print("input error");
    break;
}

//
//
//
//
//  ADDED SENIOR PROJECT C
//
//
//
  Serial.print("");
  Serial.print(temp);
  Serial.print(";");
  Serial.print(humidity);
  Serial.print(";");
  Serial.print(lightLevel);
  Serial.print(";");
  Serial.print(gasLevel);
  Serial.print(";");
  Serial.print(waterLevel);
  Serial.print("\n");
  delay(2000);
  
  //check if system variables are reasonable and display rgb color
  check_stat(temp, humidity, lightLevel, gasLevel, waterLevel);
  
  delay(150);
  
}

/*
 * RGB_color takes in values that change the color of the light
 */
void RGB_color(int red_light_value, int blue_light_value){
  analogWrite(9, red_light_value);    //green
  analogWrite(10, blue_light_value);    //blue
}

/*
 * check stat monitors system variables and calls for light changes based on good and bad environments
 */
void check_stat(int temp, int humidity, int light, int gas, int water){
  //start with red and blue light variables at 0
  int red = 0;
  int blue = 0;

  //Temperature variable
  if(temp > 80 | temp < 65){
    //bad, increase RGB red
    red += 64;
  }
  else{
    //good, increase rgb blue
    blue += 64;
  }

  //Humidity variable
  if(humidity > 70 | humidity < 20){
    //bad, increase rgb red
    red += 64;
  }
  else{
    //good, increase rgb blue
    blue += 64;
  }

  //Light variable
  if(light > 850 | light < 450){
    //bad, increase rgb red
    red += 64;
  }
  else{
    //good, increase rgb blue
    blue += 64;
  }

  //Gas variable
  if(gas > 300){
    //bad, increase rgb red
    red += 63;
  }
  else{
    //good, increase rgb blue
    blue += 63;
  }

  //change light to current setup
  RGB_color(red, blue);
  
}

void serialEvent() {
  while (Serial.available()) {
    //flip screen
    if(screenDisplay == 4){
      screenDisplay = 1;
    }
    else{
      screenDisplay ++;
    }
  }
}
