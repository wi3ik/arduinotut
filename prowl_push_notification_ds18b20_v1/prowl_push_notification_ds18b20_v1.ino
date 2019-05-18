#include <DallasTemperature.h>
#include <OneWire.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
//#include <EspProwl.h>
// https://github.com/marcos69/EspProwl

#include <string.h>


typedef enum DeliveryMechanism {
  PROWL
} DeliveryMechanism;

class EspProwlClass {
  public:
    EspProwlClass();
    void begin();
    void begin(DeliveryMechanism deliveryMechanism);
    void setApiKey(char *apiKey);
    void setApplicationName(char *applicationName);
    int push(char *eventStr, char *messageStr, int priority);

  private:
    byte notificationServiceIpAddr[4];
    char apiKey[51];
    char applicationName[257];
    char *notificationServiceHost;
    char *notificationServiceUrlPath;
};

extern EspProwlClass EspProwl;

/************************************/



#define PROWL_API_HOST "http://api.prowlapp.com"
#define PROWL_PUSH_PATH_URI "/publicapi/add"

#define EspProwl_DEBUG 0

EspProwlClass::EspProwlClass() {
}

void EspProwlClass::begin() {
  begin(PROWL);
}

void EspProwlClass::begin(DeliveryMechanism deliveryMechanism) {

/*
  if (deliveryMechanism == PROWL) {
    notificationServiceHost = PROWL_API_HOST;
    notificationServiceUrlPath = PROWL_PUSH_PATH_URI;
  }

  if (EspProwl_DEBUG) {
    if (ret == 1) {
      Serial.print("The IP address of ");
      Serial.print(notificationServiceHost);
      Serial.print(" is: ");
  //    remote_addr.printTo(Serial);
      Serial.println(".");
    } else if (ret == -1) {
      Serial.println("Timed out.");
    } else {
      Serial.print("Failed with error code: ");
      Serial.print(ret);
      Serial.println(".");
    }
  }  */
}

void EspProwlClass::setApiKey(char *aApiKey) {
  strncpy(apiKey, aApiKey, 51);
}

void EspProwlClass::setApplicationName(char *aApplicationName) {
  strncpy(applicationName, aApplicationName, 257);
}


int EspProwlClass::push(char *eventStr, char *messageStr, int priority) {
  if (EspProwl_DEBUG) {
    Serial.println(apiKey);
    Serial.println(applicationName);
    Serial.println(eventStr);
    Serial.println(messageStr);
    Serial.println(priority);
  }

  int result;
  char priorityStr[5];
  sprintf(priorityStr, "%d", priority);

  String push_arguments;
  push_arguments = "apikey="+ (String) apiKey ;
  push_arguments += "&application="+ (String) applicationName ;
  push_arguments += "&event="+ (String) eventStr ;
  push_arguments += "&description="+ (String) messageStr ;
  push_arguments += "&priority="+ (String) priorityStr ;

  if (EspProwl_DEBUG) {
    Serial.println(push_arguments);
  }

  /* #ifdef ESP8266
  String notifSHSUP = (String) notificationServiceHost + (String) notificationServiceUrlPath;
  Serial.println(notifSHSUP);

  HTTPClient client;
  client.begin( notifSHSUP );
  client.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int result = client.POST(push_arguments);


  if (result==HTTP_CODE_OK) {
    client.writeToStream(&Serial);
    client.end();
  }
  else {
    client.writeToStream(&Serial);
    client.end();
    if (EspProwl_DEBUG) {
      Serial.println("failed to connect");
    }
  }
  #else*/

  WiFiClient client;
  result = 0;
  if (client.connect("api.prowlapp.com", 80)) {
    client.println("POST /publicapi/add HTTP/1.1");
    client.println("Host: api.prowlapp.com" );
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(push_arguments.length());
    client.println();
    client.println(push_arguments);

    int incr=0;
    char response_buffer[255];
    char result_buffer[16];

    long interval = 5000;
    unsigned long currentMillis = millis(), previousMillis = millis();


    while(!client.available()){
      if( (currentMillis - previousMillis) > interval ) {
        Serial.println("Timeout");
        client.stop();
        return result=-1;
      }
      currentMillis = millis();
    }

    while (client.connected())
    {
      if ( client.available() )
      {
        char str=client.read();
        response_buffer[incr++]=str;
        if (incr>15) {
          client.stop();
          break;
        }
      }
    }

    response_buffer[incr]=0;
    strncpy(result_buffer, &response_buffer[9], 3);
    result_buffer[3] = '\0';
    result = atoi(result_buffer);

    if (EspProwl_DEBUG) {
      Serial.println("response: ");
      Serial.println(result);
    }
  }

  //#endif

  if (result==200) {
    if (EspProwl_DEBUG) {
      Serial.println("data uploaded");
    }
  }
  else {
    if (EspProwl_DEBUG) {
      Serial.print("ERROR: Server returned ");
      Serial.println(result);
    }
  }

  return result;
}

EspProwlClass EspProwl;




const char* WIFI_SSID =  "wi-fi";
const char* WIFI_PWD =  "4e6ypawka";

#define DEBUG 1


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);




void setup() {

  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();




  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  
  EspProwl.begin();
  // For Prowl, go to
  //   https://www.prowlapp.com/api_settings.php
  // to create an API key.
  // If you don't, the server will return a 401 error code.
  EspProwl.setApiKey("3be1c9427a8091e4b1388ed2b6dfc197cce9d749");

  EspProwl.setApplicationName("PushNotification");  
}

void loop() {
  char buffer[200] = {0};
  double threshold = 30;

  // put your main code here, to run repeatedly:
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  //Serial.print("Temperature for the device 1 (index 0) is: ");
  //Serial.println(sensors.getTempCByIndex(0));

  //ardprintf("Temperature is: %d", sensors.getTempCByIndex(0));
  sprintf (buffer, "Temperature is: %.2f\n",sensors.getTempCByIndex(0));
  Serial.print (buffer);

  delay(1000);

  if (threshold < sensors.getTempCByIndex(0))
  {
    sprintf (buffer, "Temperature (%.2f) is higher than threshold (%.2f)\n",sensors.getTempCByIndex(0), threshold);
    Serial.print (buffer);

    if (DEBUG) Serial.print("Sending push notification...");
    int returnCode = EspProwl.push("DS18B20_TEST", buffer, 0);
    if (returnCode == 200) {
      if (DEBUG) Serial.println("OK.");
    } else {
      if (DEBUG) Serial.print("Error. Server returned: ");
      if (DEBUG) Serial.print(returnCode);
    }
    
    delay(5000);
  }




}


/*
Temperature (30.25) is higher than threshold (30.00)
Sending push notification...3be1c9427a8091e4b1388ed2b6dfc197cce9d749
PushNotification
DS18B20_TEST
Temperature (30.25) is higher than threshold (30.00)

0
apikey=3be1c9427a8091e4b1388ed2b6dfc197cce9d749&application=PushNotification&event=DS18B20_TEST&description=Temperature (30.25) is higher than threshold (30.00)
&priority=0
response: 
200
data uploaded
OK.
Temperature is: 28.44
Temperature is: 28.12

*/
