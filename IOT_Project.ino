#include <ThingSpeak.h>

#include "FirebaseESP8266.h"

#define FIREBASE_HOST "node-eece6-default-rtdb.firebaseio.com"

#include <ESP8266WiFi.h>

int led = 14;

#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
int pump=16;//D0 pin
int fan=12;//D3 pin
#include "DHT.h"//Libary should be installed before hand 

#define DHTYPE DHT11
DHT dht(2, DHTYPE);


#define FIREBASE_AUTH "Wrdq4GwnN7ce5XS1sY7ZGNZhYUwVcTuQdvdMyf9f"


#define WIFI_SSID "FAADU STAR" 
#define WIFI_PASSWORD "9007216498@Va" 

FirebaseData firebaseData;
FirebaseData ledData;FirebaseData ledData1;FirebaseData ledData2;
WiFiClient client;



long myChannelNumber = 1266071;
const char myWriteAPIKey[] = "27AEPEBUD61C9GFK";
//setup---------------------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);

  dht.begin();
  pinMode(led,OUTPUT);pinMode(pump,OUTPUT);pinMode(fan,OUTPUT);
  pinMode(15,INPUT);pinMode(A0,INPUT);

  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 ThingSpeak.begin(client);
  
  lcd.begin(); //Init with pin default ESP8266 or ARDUINO

  lcd.backlight();
  lcd.setCursor(0, 0);
#if defined(ESP8266)
  lcd.print("ESP8266");
#else
  lcd.print("ARDUINO");
#endif
  lcd.setCursor(0, 1);
  lcd.print("Hello Friends");
  delay(2000);
  lcd.clear();
  delay(100);

}

void sensorUpdate(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);delay(10);
  float t = dht.readTemperature();
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);delay(10);
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

 float m = analogRead(A0);//soil moisture calculation
ThingSpeak.writeField(myChannelNumber, 4, m, myWriteAPIKey);delay(10);
int seq=map(m, 1023, 0, 0,100);


  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("C  ,"));
  Serial.print(f);
  Serial.println(F("F  "));

if (Firebase.setFloat(firebaseData, "/FirebaseIOT/temperature", t))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/humidity", h))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/moisture", m))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/light", seq))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  
}

//loop -----------------------------------------------------------------------------------------------------------------------------------------
void loop()
{
 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
 int m = analogRead(A0);//soil moisture calculation
  
int seq=digitalRead(13);

  sensorUpdate();


  if(m>650)
  {
    digitalWrite(pump,LOW);
    delay(10000);
  }
  else
{  digitalWrite(pump,HIGH);}

  if (Firebase.getString(ledData, "/FirebaseIOT/led")){
    Serial.println(ledData.stringData());
    if (ledData.stringData() == "1") {
    digitalWrite(led, HIGH);

   Serial.println("ON Light");
    }
  else if (ledData.stringData() == "0"){
    Serial.println("OFF Light");
    digitalWrite(led, LOW);
    }
  }
  delay(50);

  if (Firebase.getString(ledData1, "/FirebaseIOT/pump")){
    Serial.println(ledData1.stringData());
    if (ledData1.stringData() == "1") {
    digitalWrite(pump, LOW);

   Serial.println("pump on for 10 seconds");
   delay(10000);
    }
  else if (ledData1.stringData() == "0"){
    Serial.println("pump off");
    digitalWrite(pump, HIGH);
    }
  }
  delay(50);

   if (Firebase.getString(ledData2, "/FirebaseIOT/fan")){
    Serial.println(ledData2.stringData());
    if (ledData2.stringData() == "1") {
    digitalWrite(fan, LOW);

   Serial.println("fan on");
    }
  else if (ledData2.stringData() == "0"){
    Serial.println("fan off");
    digitalWrite(fan, HIGH);
    }
  }
  delay(50);
  lcd.setCursor(0, 0);
  lcd.print("T:");
  
  Serial.println(t); lcd.print(t);lcd.print("C");
  lcd.print(",");
delay(50);

if(seq=='1')
{
lcd.print("NIGHT");Serial.println("seq");}
else
{
   lcd.print("DAY");Serial.println("seq");
}
delay(50);

 
  lcd.setCursor(0, 1);
  lcd.print("H:");
 Serial.println(h);

  lcd.print(h);
   lcd.print(" ,");
  delay(50);
  lcd.print(" M:");
  Serial.println(m); //print the value to serial port
lcd.print(m);
  
}
