#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Host Config
const char* host = "notify-api.line.me";
const char* path = "/api/notify";
const char* token   = "GvGmRRl3xjIO9y4ztnYAa81e39XWafKs1WOATIloDnZ";
const char* OpenBody = "message=OPEN%20!!&stickerPackageId=8515&stickerId=16581246";
const char* CloseBody = "message=Close%20!!&stickerPackageId=8515&stickerId=16581242";

// WIFI Config
const char* ssid = "SoGood";  
const char* password = "123789456";

// ESP8266
// #define SENSOR 6
#define SENSOR 4

int flag;

WiFiClientSecure client;

void setup() {

  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(SENSOR,INPUT);
  flag = digitalRead(SENSOR);
}

void loop() {
  unsigned int sensor = digitalRead(SENSOR);
  if( sensor && (flag == 0) ){
    httpRequest(OpenBody);
    flag = 1;
  }else if( !sensor && (flag == 1) ){
    httpRequest(CloseBody);
    flag = 0;
  }
  delay(400);
   
}

void httpRequest(const char* httpBody) {

  client.setInsecure();//skip verification
  if (client.connect( host , 443)) {
    Serial.println("connecting...");
    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Authorization: Bearer ");
    client.println(token);
    client.print("Host: ");
    client.println(host);
    client.println("Accept: */*");
    client.println("User-Agent: Arduino");
    client.println("Accept-Encoding: gzip, deflate, br");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(String(httpBody).length());
    client.println("Connection: close");
    client.println();
    // Body //
    client.println(httpBody);
    //client.print("message=");
    //client.println(message);
    //client.print("&stickerPackageId=");
    //client.println(Stick_Pack_Id);
    //client.print("&stickerId=");
    //client.println(Stick_Id);

    while (client.connected()) {
    String line = client.readStringUntil('\n');
     if (line == "\r") {
      Serial.println("headers received");
      break;
     }
    }
    
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    client.stop();
  } else {
    Serial.println("connection failed");
  }
}
