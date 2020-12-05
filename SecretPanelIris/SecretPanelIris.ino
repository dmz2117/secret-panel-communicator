#include <FTDebouncer.h>
#include <WiFiNINA.h>
#include <Firebase_Arduino_WiFiNINA_HTTPClient.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <Servo.h>


#define WIFI_SSID "Golden Girls"
#define WIFI_PASSWORD "fantasia"

#define FIREBASE_HOST "skate-sharing.firebaseio.com"
#define FIREBASE_AUTH "eLMov0QaKPqk8TbPrbLtZ2jp9BCDLKxkqH9UOU41"

FTDebouncer buttons;
Servo myservo;
Servo readservo;

//Define Firebase data object

FirebaseData firebaseData;

//Define pins
int LED_B_PIN = 6;
int SEND_PIN = 7;
int LED_R_W_PIN = 8;

//Define variables
int blueled = 0;
int redledwrite = 0;

void setup() {
  Serial.begin(57600);
  //WIFI Connection in client mode
  while (!Serial) {
    ; // wait for the serial port
  }

  Serial.println("Connecting to Wi-Fi");
  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }

  Serial.print("Connected to Wi-Fi with this IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin("secret-panel.firebaseio.com", "wZAbJ5RsGj1QsivwtIqNJA4gWswPcJ0CQNJxsjb5", "Golden Girls", "fantasia");
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, "/hermes")) {
    //Could not begin stream connection, then print out the error detail
    Serial.println(firebaseData.errorReason());
  }

  buttons.addPin(LED_B_PIN, LOW);
  buttons.addPin(SEND_PIN, LOW);
  buttons.begin();

  pinMode(LED_R_W_PIN, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  buttons.update();

  if (!Firebase.readStream(firebaseData)) {
    //If read stream was failed, print the error detail.
    Serial.println(firebaseData.errorReason());
  }

  if (firebaseData.streamTimeout()) {
     //If stream timeout, just notify
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }

//  if (firebaseData.streamAvailable()) {
//   
//   //Print out value
//   //Stream data can be many types which can be determined from function dataType
//
//    if (firebaseData.dataType() == "int")
//      Serial.println(firebaseData.intData());
//    else if (firebaseData.dataType() == "float")
//      Serial.println(firebaseData.floatData());
//    else if (firebaseData.dataType() == "boolean")
//      Serial.println(firebaseData.boolData());
//    else if (firebaseData.dataType() == "string")
//      Serial.println(firebaseData.stringData());
//    else if (firebaseData.dataType() == "json")
//      Serial.println(firebaseData.jsonData());
//    
//  }
//  
//  if (Firebase.getInt(firebaseData, "/hermes/ledr")) {
//
//    if (firebaseData.dataType() == "int") {
//      redledwrite = firebaseData.intData();
//    }
//
//  } else {
//    //Failed, then print out the error detail
//    Serial.println(firebaseData.errorReason());
//  }

  if (redledwrite == 1) {
    digitalWrite(LED_R_W_PIN, HIGH);
  } else {
    digitalWrite(LED_R_W_PIN, LOW);
  }
}

void onPinActivated(int pinNr) {
  if (pinNr == 6) {
    blueled = 1;
    Serial.println(blueled);
  }
  
}

void onPinDeactivated(int pinNr) {
  if (pinNr == 6) {
    blueled = 0;
  }

  if (pinNr == 7) {
    String updateData = String("{\"ledb\": ") + blueled + " }";

    Serial.print(updateData);
  
    if (Firebase.updateNode(firebaseData, "/iris", updateData)) {
    
      //Success, then try to read the payload value
    
      //Database path that updated
      Serial.println(firebaseData.dataPath());
    
      //Data type at updated database path
      Serial.println(firebaseData.dataType()); //Should be "json"
    
      //Print the JSON string payload that returned from server
      Serial.println(firebaseData.jsonData()); //Should mathes the value in updateData variable
    
      //Actual sent payload JSON data
      Serial.println(updateData);
    
    } else {
      
      //Failed, then print out the error detail
      Serial.println(firebaseData.errorReason());
      
    }
  }
  
}

// bad request, Invalid data; couldn't parse JSON object. Are you sending a JSON object with valid key names?
