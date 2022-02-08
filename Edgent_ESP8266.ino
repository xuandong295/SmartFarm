// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLtp5COjeS"
#define BLYNK_DEVICE_NAME "AutoFarmBot"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

#define SW_VERSION "   SW Ver. 4.0" // SW version will appears at innitial LCD Display
#include "stationDefines.h"       // Project definitions
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* ESP & Blynk */
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#define ssid "VNPT"
#define password "hoibabinh"
#define mqtt_server "broker.hivemq.com" // Thay báº±ng thÃ´ng tin cá»§a báº¡n
#define mqtt_topic_pub "smart-garden"  
#define mqtt_topic_sub "smart-garden"
WiFiClient espClient;
PubSubClient client(espClient);
const uint16_t mqtt_port = 1883; //Port của CloudMQTT TCP

WidgetLED PUMPs(V0);  // Echo signal to Sensors Tab at Blynk App
WidgetLED PUMPa(V5); // Echo signal to Actuators Tab at Blynk App
WidgetLED LAMPs(V1);  // Echo signal to Sensors Tab at Blynk App
WidgetLED LAMPa(V6); // Echo signal to Actuators Tab at Blynk App

/* TIMER */
#include <SimpleTimer.h>
SimpleTimer timer;

/* OLED */

#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>    //Thư viện LCD
LiquidCrystal_I2C lcd(0x27,16,2); //Thiết lập địa chỉ và loại LCD

/* DHT22*/
#include "DHT.h"
DHT dht(DHTPIN, DHTTYPE);

/* DS18B20 Temperature Sensor */
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("ArduFarmBot 2");
  Serial.println(".... Starting Setup");
  Serial.println(" ");

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LAMP_PIN, OUTPUT);
  pinMode(PUMP_ON_BUTTON, INPUT_PULLUP);
  pinMode(LAMP_ON_BUTTON, INPUT_PULLUP);
  pinMode(SENSORS_READ_BUTTON, INPUT_PULLUP);
  pinMode(soilMoisterVcc, OUTPUT);
  
  BlynkEdgent.begin();
  oledStart();
  dht.begin();
  DS18B20.begin();

  PUMPs.off();
  LAMPs.off();
  PUMPa.off();
  LAMPa.off();
  digitalWrite(PUMP_PIN, HIGH); // To be used with Relay module (inverted logic: normally HIGH)
  digitalWrite(LAMP_PIN, HIGH); // To be used with Relay module (inverted logic: normally HIGH)
  digitalWrite (soilMoisterVcc, LOW);
  
  waitButtonPress (SHOW_SET_UP); // Wait for Sensor Button to be pressed 
  lcd.clear();

  delay(10);
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//  client.setCallback(callback);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  startTimers();
}

void loop()
{
  if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();
  
  timer.run(); // Initiates SimpleTimer
  BlynkEdgent.run();
  
}


// Hàm kết nối wifi
void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/****************************************************************
* Read remote commands 
****************************************************************/
BLYNK_WRITE(V5) // Pump remote control
{
  int i=param.asInt();
  if (i==1) 
  {
    pumpStatus = !pumpStatus;
    aplyCmd();
  }
}

BLYNK_WRITE(V6) // Lamp remote control
{
  int i=param.asInt();
  if (i==1) 
  {
    lampStatus = !lampStatus;
    aplyCmd();
  }
}

/****************************************************************
* Read local commands (Pump and Lamp buttons are normally "HIGH"):
****************************************************************/
void readLocalCmd() 
{  
  boolean digiValue = debounce(PUMP_ON_BUTTON);
  if (!digiValue) 
  {
    pumpStatus = !pumpStatus;
    aplyCmd();
  }

  digiValue = debounce(LAMP_ON_BUTTON);
  if (!digiValue) 
  {
    lampStatus = !lampStatus;
    aplyCmd();
  }

  digiValue = debounce(SENSORS_READ_BUTTON);
  if (!digiValue) 
  {
    turnOffOLED = !turnOffOLED;
    if (!turnOffOLED)
    {
      getDhtData();
      getSoilMoisterData();
      getSoilTempData();
      oledStart();
      displayData();
    }else lcd.clear();
  }
}

/***************************************************
* Receive Commands and act on RELAYs - Logic LOW
****************************************************/
void aplyCmd()
{
  if (pumpStatus == 1) 
  {
    Blynk.notify("ArduFarmBot2: Warning ==>> Pump ON"); 
    digitalWrite(PUMP_PIN, LOW); // To be used with Relay module (inverted logic: activate with LOW)
    if (!turnOffOLED) displayData();
    PUMPs.on();
    PUMPa.on();
  }
  else
      {
        digitalWrite(PUMP_PIN, HIGH); // To be used with Relay module (inverted logic: normally HIGH)
        if (!turnOffOLED) displayData();
        PUMPs.off();
        PUMPa.off();
      }
  
  if (lampStatus == 1) 
  {
    Blynk.notify("ArduFarmBot2: Warning ==>> Lamp ON");
    digitalWrite(LAMP_PIN, LOW); // To be used with Relay module (inverted logic: activate with LOW)
    if (!turnOffOLED) displayData();
    LAMPs.on();
    LAMPa.on();
  }
  else
      {
        digitalWrite(LAMP_PIN, HIGH); // To be used with Relay module (inverted logic: normally HIGH)
        if (!turnOffOLED) displayData();
        LAMPs.off();
        LAMPa.off();
      }
      sendUptimeMQTT();
}

/***************************************************
* Automatically Control the Plantation based on sensors reading
****************************************************/
void autoControlPlantation(void)
{ 
  if (soilMoister < DRY_SOIL) 
  {
    turnPumpOn();
  }

  if (airTemp < COLD_TEMP) 
  {
    turnLampOn();
  }
}

/***************************************************
* Turn Pump On for a certain amount of time
****************************************************/
void turnPumpOn()
{
  pumpStatus = 1;
  aplyCmd();
  delay (TIME_PUMP_ON*1000);
  pumpStatus = 0;
  aplyCmd();
  sendUptimeMQTT();
}

/***************************************************
* Turn Lamp On for a certain amount of time 
****************************************************/
void turnLampOn()
{
  lampStatus = 1;
  aplyCmd();
  delay (TIME_LAMP_ON*1000);
  lampStatus = 0;
  aplyCmd();
  sendUptimeMQTT();
}

/***************************************************
 * Send data to Blynk
 **************************************************/
void sendUptime()
{
  Blynk.virtualWrite(10, airTemp); //virtual pin V10
  Blynk.virtualWrite(11, airHum); // virtual pin V11
  Blynk.virtualWrite(12, soilMoister); // virtual pin V12
  Blynk.virtualWrite(13, soilTemp); //virtual pin V13
}
void sendUptimeMQTT()
{
  String airTemperature  = "airTemp " + (String)airTemp;
  client.publish(mqtt_topic_pub, airTemperature.c_str());
  String airHumidity  = "airHum " + (String)airHum;
  client.publish(mqtt_topic_pub, airHumidity.c_str());
  String soilTemperature  = "soilTemp " + (String)soilTemp;
  client.publish(mqtt_topic_pub, soilTemperature.c_str());
  String soilMoisture  = "soilMoister " + (String)soilMoister;
  client.publish(mqtt_topic_pub, soilMoisture.c_str());
  String pumpStatusString  = "pumpStatus " + (String)pumpStatus;
  client.publish(mqtt_topic_pub, pumpStatusString.c_str());
  String lampStatusString  = "lampStatus " + (String)lampStatus;
  client.publish(mqtt_topic_pub, lampStatusString.c_str()); 
}
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Co tin nhan moi tu topic:");
  Serial.println(topic);
  String receivedChar = "";
  for (int i = 0; i < length; i++) {
    receivedChar += (char)payload[i];
  }
  Serial.print(receivedChar);
  if (receivedChar == "lampOn")
    turnLampOn();
  if (receivedChar == "pumpOn")
    turnPumpOn();
  Serial.println();;
}
// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() 
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266_id1","ESP_offline",0,0,"ESP8266_id1_offline"))  //kết nối vào broker
    {
      Serial.println("Đã kết nối:");
      client.subscribe(mqtt_topic_sub); //đăng kí nhận dữ liệu từ topic IoT47_MQTT_Test
    }
    else 
    {
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
