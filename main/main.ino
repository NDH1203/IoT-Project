#include <FirebaseArduino.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define FIREBASE_HOST "iot-control-b05da.firebaseio.com"
#define FIREBASE_AUTH "SAhsFAyLTWjfwqwgtebI9I23axUgSvv5x7rNj0CG"

char uid[30];
WiFiManagerParameter user_uid("uid", "user uid", uid, 30);

#define dSizes 4

byte Devices[dSizes] = {16, 5, 4, 0};

bool dStates[dSizes];

byte i;
void setup()
{
    Serial.begin(9600);
    for(i = 0; i < dSizes; i++)
    {
        pinMode(Devices[i], OUTPUT);
        //Serial.println(i);
    }

    Reconnect();
}

void ConnectWifi()
{
    WiFiManager wifiManager;
    wifiManager.addParameter(&user_uid);
    wifiManager.resetSettings();
    wifiManager.autoConnect("ESP8266");
}

void Reconnect()
{
    ConnectWifi();
    strcpy(uid, user_uid.getValue());
    //Serial.print(uid);
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.stream("/devices/"+(String)uid+"/devicesList");
    Serial.print("Connecting...");
}

void loop()
{
    if (Firebase.available())
    {
        FirebaseObject event = Firebase.readEvent();
        String eventType = event.getString("type");
        eventType.toLowerCase();

        Serial.print("event: ");
        Serial.println(eventType);
        if (eventType == "put" || eventType == "patch")
        {
          
            Serial.print("data: ");
            Serial.println(event.getString("data"));
            String path = event.getString("path");
            String data = event.getString("data");
            Serial.println(path);
            JsonVariant payload = event.getJsonVariant("data");
            payload.printTo(Serial);
            if (path == "/")
            {
                for(i = 0; i < dSizes; i++)
                {
                    //byte t = i;
                    //String device = "device";
                    //device += t;
                    //Serial.print(device);
                    JsonVariant d = payload[i];
                    JsonVariant load = d["state"];
                    if(load.success()){
                        dStates[i] = load;
                        //load.printTo(Serial);
                    }
                    digitalWrite(Devices[i], !dStates[i]);
                }
            }
            else {
                //byte len = path.length();
                //byte uid = path.substring(len-1).toInt()-1;
                String tmp = path;
                tmp.remove(0, 1);
                int uid = tmp.toInt();
                //Serial.print(uid);
                JsonVariant state = payload["state"];
                if(state.success()){
                    dStates[uid] = state;
                    digitalWrite(Devices[uid], !dStates[uid]);
                }
            }
        }
    }
    else if (Firebase.failed())
    {
        Serial.println("streaming error");
        Serial.println(Firebase.error());
        Reconnect();
    }

    delay(1000);
}
