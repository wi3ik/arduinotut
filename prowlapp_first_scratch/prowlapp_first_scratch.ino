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


















/************************************/

const char* WIFI_SSID =  "wi-fi";
const char* WIFI_PWD =  "4e6ypawka";

#define DEBUG 1

int counterValue;

void setup() {
  counterValue = 10;
  Serial.begin(115200);
  if (DEBUG) Serial.println("Attempting to obtain a DHCP lease...");

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

  EspProwl.setApplicationName("EspProwl");
  
}

void loop() {
  if (counterValue > 0) {
    if (DEBUG) Serial.println(counterValue);
    counterValue--;
    delay(300);
  } else if (counterValue == 0) {
    counterValue--;
    if (DEBUG) Serial.print("Sending push notification...");
    int returnCode = EspProwl.push("RingTheRedPhone_TEST_Vitaly", "Hi,Comrad", 0);
    if (returnCode == 200) {
      if (DEBUG) Serial.println("OK.");
    } else {
      if (DEBUG) Serial.print("Error. Server returned: ");
      if (DEBUG) Serial.print(returnCode);
    }
  }
}

/*
void setup() {
       // Start Serial
       Serial.begin(115200);
       WiFi.begin(ssid, password);
       while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
       }
       Serial.println("");
       Serial.println("WiFi now connected at address");
       // Print the IP address
       Serial.println(WiFi.localIP());
       EspProwl.begin();
       EspProwl.setApiKey("MY_PROWL_API_CODE");
       EspProwl.setApplicationName("ESP6266 Prowl test");
       int returnCode = EspProwl.push("Hello World event", "Initial notification", 0);
}
*/
