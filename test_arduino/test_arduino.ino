#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("#analog,random");
}
void loop() {
    if (!random(100)) Serial.println(String('@') + "log:" + millis());
    Serial.println(String('$') + analogRead(0) + ',' + random(255));
    delay(100);
}