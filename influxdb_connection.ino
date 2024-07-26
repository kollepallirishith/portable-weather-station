#include <WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <DHT11.h>



//sensor setups 
Adafruit_BMP280 bmp;
DHT11 dht11(15);


WiFiMulti wifiMulti;
#define DEVICE "ESP32"

  
// WiFi AP SSID
#define WIFI_SSID "xx"
// WiFi password
#define WIFI_PASSWORD "xxxx"
//go to influx db to get these 
#define INFLUXDB_URL "xxx"
#define INFLUXDB_TOKEN "xx"
#define INFLUXDB_ORG "xx"
#define INFLUXDB_BUCKET "xx"
     
    // Time zone info
#define TZ_INFO "UTC5.5"
    
    // Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
    
    // Declare Data point
Point sensor("wifi_status");
//temp test values
int val =2470;
int pressure = 973;
int altitude = 335;
int temperature = 31;
int humidity = 64;
//wifi setup code


void setup() {
  // put your setup code here, to run once:


  Serial.begin(115200);


  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP:");
  Serial.println(WiFi.localIP());
  Serial.println();



  pinMode(22,INPUT);

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  
    // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());


  //setup for bmp
      // wait for native usb
  Serial.println("BMP280 Sensor event test");

  unsigned status;
  status = bmp.begin(0x76);
  if (!status) {
  }


}




//---------------------------------------------------------------\

void loop() {
  // mq135 air quality sensor 


  val = analogRead(36);
 
   // bmp 280

  
  pressure = (bmp.readPressure()/100.0);
  altitude = bmp.readAltitude(1013.25);  
    
  //dht 11
  int result = dht11.readTemperatureHumidity(temperature, humidity);
    // Clear fields for reusing the point. Tags will remain the same as set above.
    sensor.clearFields();
  
    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("rssi", WiFi.RSSI());
    sensor.addField("air quality ", val);
    sensor.addField("preassure", pressure );
    sensor.addField("altitude", altitude );
    sensor.addField("temperature",temperature );
    sensor.addField("humidity",humidity );

    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());
  
    // Check WiFi connection and reconnect if needed
    client.writePoint(sensor);
  
    // Write point
    if(!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  
    Serial.println("Waiting 5 second");
    delay(5000);

}
