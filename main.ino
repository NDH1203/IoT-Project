#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "iot-control-b05da.firebaseio.com"
#define FIREBASE_AUTH "SAhsFAyLTWjfwqwgtebI9I23axUgSvv5x7rNj0CG"

#define WIFI_SSID "PHONG THOA"
#define WIFI_PASSWORD "sodienthoai"

#define Device1 2
#define Device2 5
#define Device3 4

void setup() {
    Serial.begin(9600);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    //Firebase.stream("/User1/devices");
}

void loop() {
    if (Firebase.failed()) {
        Serial.println("streaming error");
        Serial.println(Firebase.error());
    }

    if (Firebase.available()) {
        FirebaseObject event = Firebase.readEvent();
        String eventType = event.getString("type");
        eventType.toLowerCase();

        Serial.print("event: ");
        Serial.println(eventType);
        if (eventType == "put") {
            Serial.print("data: ");
            Serial.println(event.getString("data"));
            String path = event.getString("path");
            String data = event.getString("data");
            if (path == "/") {
                JsonVariant payload = event.getJsonVariant("data");
                Serial.print("data: ");
                bool device1 = payload["device1"];
                bool device2 = payload["device2"];
                bool device3 = payload["device3"];

                digitalWrite(Device1, device1);
                digitalWrite(Device2, device2);
                digitalWrite(Device3, device3);

                payload.printTo(Serial);
            }
            if (path == "/device1") {

                bool payload = event.getBool("data");
                digitalWrite(Device1, payload);
                Serial.print("data device1: ");
                Serial.println(payload);
            }
            if (path == "/device2") {

                bool payload = event.getBool("data");
                digitalWrite(Device2, payload);
                Serial.print("data device2: ");
                Serial.println(payload);
            }
            if (path == "/device3") {

                bool payload = event.getBool("data");
                digitalWrite(Device3, payload);
                Serial.print("data device3: ");
                Serial.println(payload);
            }
        }
    }
    delay(20000);
}