
/* Automatic Control Parameters Definition */
#define DRY_SOIL      66
#define WET_SOIL      85
#define COLD_TEMP     12
#define HOT_TEMP      22
#define TIME_PUMP_ON  15
#define TIME_LAMP_ON  15

/* TIMER */
#define READ_BUTTONS_TM   1L  // definitions in seconds
#define READ_SOIL_TEMP_TM 2L
#define READ_SOIL_HUM_TM  10L
#define READ_AIR_DATA_TM  2L
#define SEND_UP_DATA_TM   10L
#define AUTO_CTRL_TM      60L     

/* OLED */
boolean turnOffOLED = 1;
#define SHOW_SET_UP   30

/* DHT22*/
#define DHTPIN D3  
#define DHTTYPE DHT11 
float airHum = 0;
float airTemp = 0;

/* Soil Moister */
#define soilMoisterPin A0
#define soilMoisterVcc D8
int soilMoister = 0;

/* DS18B20 Temperature Sensor */
#define ONE_WIRE_BUS D5 // DS18B20 on NodeMCU pin D5 corresponds to GPIO 014 on Arduino
float soilTemp;

/* Relays */
#define PUMP_PIN D6              //PUMP (Red LED)
#define LAMP_PIN D7              //LAMP (Green LED)
boolean pumpStatus = 0;
boolean lampStatus = 0;

/* Buttons */
#define PUMP_ON_BUTTON D9        //push-button PUMP (Red)
#define LAMP_ON_BUTTON D10       //push-button LAMP (Green)
#define SENSORS_READ_BUTTON D4   //push-button SENSOR (yellow)

/* Wifi*/

#define ssid "VNPT"
#define password "hoibabinh"
#define mqtt_server "broker.hivemq.com" // Thay bằng thông tin của bạn
#define mqtt_topic_pub "smart-garden"  
#define mqtt_topic_sub "smart-garden"
