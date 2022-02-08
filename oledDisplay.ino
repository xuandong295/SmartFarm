/***************************************************
 * Start OLED
 **************************************************/
void oledStart(void)
{
  Wire.begin(D2,D1);               //Thiết lập chân kết nối I2C (SDA,SCL);
  lcd.init();                      //Khởi tạo LCD
  lcd.clear();                     //Xóa màn hình
  lcd.backlight();                 //Bật đèn nền                  // Initialze SSD1306 OLED display

}

/***************************************************
 * Display data at Serial Monitora & OLED Display
 **************************************************/
void displayData(void)
{ 
  String pumpStatusStr;
  String lampStatusStr;
  if (pumpStatus == 1) pumpStatusStr = "on ";
  if (pumpStatus == 0) pumpStatusStr = "off";
  if (lampStatus == 1) lampStatusStr = "on ";
  if (lampStatus == 0) lampStatusStr = "off";
  lcd.print("Temp: ");
  lcd.print(airTemp);
  lcd.print("Humidity: ");
  lcd.print(airHum);
}

/***************************************************
 * Clear OLED Display
 **************************************************/
void clearOledDisplay()
{
  lcd.clear();             
}
