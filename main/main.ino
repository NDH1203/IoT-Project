#include <Firebase.h>
#include <FirebaseArduino.h>

#include <ESP8266WiFi.h>

#define FIREBASE_HOST "iot-control-b05da.firebaseio.com"
#define FIREBASE_AUTH "SAhsFAyLTWjfwqwgtebI9I23axUgSvv5x7rNj0CG"

#define USER_UID "tHiJ8OSxd1TXxAdZ6WFMhqtDVB22"

#define WIFI_SSID "PHONG THOA"
#define WIFI_PASSWORD "sodienthoai"

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
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println();
    Serial.print("Wifi connected!");
}

void Reconnect()
{
    ConnectWifi();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.stream("/devices/"+(String)USER_UID+"/devicesList");
    Serial.print("Connecting...");
}

// void GetWifiID(){
//     Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//     Serial.print("Getting wifi ID...");
//     wifi_ssid = Firebase.getString("/users/"+(String)USER_UID+"/wifiName");
//     wifi_password = Firebase.getString("/users/"+(String)USER_UID+"/wifiPassword");
// }

void loop()
{
    // if (wifi_ssid.equals("") || wifi_password.equals("")){
    //     GetWifiID();
    // }
    /*if (!wifi_ssid.equals("") || !wifi_password.equals("")){
        ConnectWifi(wifi_ssid, wifi_password);
    }*/
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
                  byte t = i+1;
                    String device = "device";
                    device += t;
                    //Serial.print(device);
                    JsonVariant d = payload[device];
                    JsonVariant load = d["state"];
                    if(load.success()){
                        dStates[i] = load;
                        //load.printTo(Serial);
                    }
                    digitalWrite(Devices[i], !dStates[i]);
                }
            }
            if(path.length() > 6){
                byte len = path.length();
                byte uid = path.substring(len-1).toInt()-1;
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
