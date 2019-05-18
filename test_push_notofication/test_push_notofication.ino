/************
GeeksTips.com
ESP8266 Arduino Tutorial - Push notification messages example
Arduino code example
www.geekstips.com

- replace the dots with your Wi-fi credentials and
- your deviceId from pushingBox account
*/
#include <ESP8266WiFi.h>

// PushingBox scenario DeviceId code and API
String deviceId = "v45C93044B336CCD";
const char* logServer = "api.pushingbox.com";

const char* ssid = "wi-fi";
const char* password = "4e6ypawka";

void setup() {
  Serial.begin(115200);
  // Sending a notification to your mobile phone
  // function takes the message as a parameter
  sendNotification("Hello World from ESP8266!");
}

void sendNotification(String message){

  Serial.println("- connecting to Home Router SID: " + String(ssid));
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("- succesfully connected");
  Serial.println("- starting client");
  
  WiFiClient client;

  Serial.println("- connecting to pushing server: " + String(logServer));
  if (client.connect(logServer, 80)) {
    Serial.println("- succesfully connected");
    /*
    String postStr = "devid=";
    postStr += String(deviceId);
    postStr += "&message_parameter=";
    postStr += String(message);
    postStr += "\r\n\r\n";
    
    Serial.println("- sending data...");
    Serial.println(postStr);
    
    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: api.pushingbox.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    */
    /*===============================*/
    String postStr = "--data-binary '{'device_iden': 'ujyz1kzBLUqsjw1ja8PeKa', 'type': 'note', 'title': 'hello from ESP', 'body': 'Push message from ESP: <TODO – some data >'}";
    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: -u o.QifFK2cDOSGVuVJ6IDprHimmlBRvQmoK: -X api.pushbullet.com/v2/pushes \n");
    client.print("Content-Type: application/json\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    /*
    curl -u o.QifFK2cDOSGVuVJ6IDprHimmlBRvQmoK: -X POST https://api.pushbullet.com/v2/pushes 
    --header 'Content-Type: application/json' 
    --data-binary '{"device_iden": "ujyz1kzBLUqsjw1ja8PeKa", "type": "note", "title": "hello from ESP", "body": "Push message from ESP: <TODO – some data >"}'
    */
  }
  client.stop();
  Serial.println("- stopping the client");




/*
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient Post;
    Post.begin("http://lemontechnologies.net/Services/Service.svc/SetData"); 
    Post.addHeader("operator", "text/plain");  
    Post.POST("Key=hi&val=jagrut1");
    String payload = Post.getString();
    Post.end();
  } 
*/



  
}

void loop() {}


/*
curl -u o.QifFK2cDOSGVuVJ6IDprHimmlBRvQmoK: -X POST https://api.pushbullet.com/v2/pushes --header 'Content-Type: application/json' --data-binary '{"device_iden": "ujyz1kzBLUqsjw1ja8PeKa", "type": "note", "title": "hello from ESP", "body": "Push message from ESP: <TODO – some data >"}'
*/
