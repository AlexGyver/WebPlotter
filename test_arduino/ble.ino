#include <Arduino.h>
#include <BLESerial.h>

void setup() {
    Serial.begin(115200);

    BLESerial.begin("ESP32C3");

    BLESerial.onData([](uint8_t* data, size_t len) {
        Serial.print("got data:");
        Serial.write(data, len);
        Serial.println();
    });

    BLESerial.onState([](bool conn) {
        Serial.println(conn ? "connected" : "disconnected");
    });

    BLESerial.send("#analog,random");
}

void loop() {
    static uint32_t tmr;

    if (millis() - tmr >= 100) {
        tmr = millis();

        if (!random(100)) {
            BLESerial.send(String('@') + "log:" + millis());
        }

        static float x;
        x += PI / 20;
        BLESerial.send(String('$') + (sin(x) * 50 + 50) + ',' + random(100));
    }
}