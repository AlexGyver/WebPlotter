#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("#analog,random");
}

void loop() {
    static uint32_t tmr;

    if (millis() - tmr >= 100) {
        tmr = millis();

        if (!random(100)) {
            Serial.println(String('@') + "log:" + millis());
        }

        static float x;
        x += PI / 20;
        Serial.println(String('$') + (sin(x) * 50 + 50) + ',' + random(100));
    }
}