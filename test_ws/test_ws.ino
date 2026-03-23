#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

#define WIFI_SSID "AlexMain"
#define WIFI_PASS "lolpass12345"

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

            case WStype_BIN:
                Serial.print("got bin: ");
                while (len--) {
                    Serial.print(*data++);
                    Serial.print(',');
                }
                Serial.println();
                break;
        }
    });
}

void loop() {
    ws.loop();

    static uint32_t t;
    static float x;
    if (millis() - t >= 1000) {
        t = millis();
        x += PI / 20;
        String s('$');
        s += sin(x) * 10;
        ws.broadcastTXT(s);
        Serial.println(s);
    }
}