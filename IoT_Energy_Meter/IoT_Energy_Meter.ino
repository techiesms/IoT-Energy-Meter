
/*
   This code is for the project
   IoT Energy Meter made by
   techiesms

   You can head to my YouTube channel
   to see the tutorial of this and
   many other IoT projects

   http://www.youtube.com/techiesms

   .      techiesms
   explore | learn | share
*/


#include "UbidotsESPMQTT.h" //https://github.com/ubidots/ubidots-mqtt-esp

const int ACPin = A0;         //set arduino signal read pin
#define ACTectionRange 20;    //set Non-invasive AC Current Sensor tection range (5A,10A,20A)

// VREF: Analog reference
// For Arduino UNO, Leonardo and mega2560, etc. change VREF to 5
// For Arduino Zero, Due, MKR Family, ESP32, etc. 3V3 controllers, change VREF to 3.3
#define VREF 3.27

/****************************************
   Define Constants
 ****************************************/
#define TOKEN "Yout_ubidots_token" // Your Ubidots TOKEN
#define WIFINAME "SSID" //Your SSID
#define WIFIPASS "PASS" // Your Wifi Pass

Ubidots client(TOKEN);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


float Rate_of_Current_Value_per_min = 0;
float Current_Value = 0;
float Power  = 0;
float amount_consumed = 0;



float readACCurrentValue()
{
  float ACCurrtntValue = 0;
  float peakVoltage = 0;
  float voltageVirtualValue = 0;  //Vrms
  for (int i = 0; i < 5; i++)
  {
    peakVoltage += analogRead(ACPin);   //read peak voltage
    delay(1);
  }
  peakVoltage = peakVoltage / 5;
  voltageVirtualValue = peakVoltage * 0.707;    //change the peak voltage to the Virtual Value of voltage

  /*The circuit is amplified by 2 times, so it is divided by 2.*/
  voltageVirtualValue = (voltageVirtualValue / 1024 * VREF ) / 2;

  ACCurrtntValue = voltageVirtualValue * ACTectionRange;

  return ACCurrtntValue;
}

void setup()
{
  // put your setup code here, to run once:
  client.ubidotsSetBroker("business.api.ubidots.com"); // Sets the broker properly for the business account
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  Serial.begin(115200);
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);

}

void loop()
{

  for (int k = 0; k < 60 ; k++)
  {

    float ACCurrentValue = readACCurrentValue(); //read AC Current Value
    Serial.print(ACCurrentValue);
    Serial.println(" A");
    Current_Value += ACCurrentValue;
    delay(1000);
  }


  Rate_of_Current_Value_per_min = Current_Value / 60;

  Serial.print("Rate of Current in a min"); Serial.print(Rate_of_Current_Value_per_min); Serial.println(" Am");

  if (!client.connected())
  {
    client.reconnect();
  }

  client.add("current", Rate_of_Current_Value_per_min);//Insert your variable Labels and the value to be sent
  client.ubidotsPublish("nodemcu");
  client.loop();
  Rate_of_Current_Value_per_min = 0;
  Current_Value = 0;

}
