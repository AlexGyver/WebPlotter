#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

WebSocketsServer ws(81, "", "esp");

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());

    ws.begin();
    ws.onEvent([](uint8_t id, WStype_t type, uint8_t* data, size_t len) {
        switch (type) {
            case WStype_DISCONNECTED:
                Serial.println("disconnected");
                break;

            case WStype_CONNECTED:
                Serial.println("connected");
                break;

            case WStype_TEXT:
                Serial.print("got text: ");
                Serial.write(data, len);
                Serial.println();

                ws.broadcastTXT(data, len);
                break;
        }
    });
}

void loop() {
    ws.loop();

    static uint32_t tmr;

    if (millis() - tmr >= 100) {
        tmr = millis();

        if (!random(100)) {
            ws.broadcastTXT(String('@') + "log:" + millis());
        }

        static float x;
        x += PI / 20;
        ws.broadcastTXT(String('$') + (sin(x) * 50 + 50) + ',' + random(100));
    }
}