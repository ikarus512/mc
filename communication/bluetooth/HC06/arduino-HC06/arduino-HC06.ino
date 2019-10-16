// Pin connections:
//
// (Arduino)        (HC-06)
// pin 1 (TX)  ---- RXD
// pin 0 (RX)  ---- RXD
// GND         ---- GND
// 3.3V        ---- VCC

int LED = 13; // Builtin LED pin on Arduino

void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
}

void loop() {
    if (Serial.available()) {
        int val = Serial.read();
        switch (val) {
            case '1': // При символе "1" включаем светодиод
                digitalWrite(LED, HIGH);
                Serial.write("ON\n");
                break;
            case '0': // При символе "0" выключаем светодиод
                digitalWrite(LED, LOW);
                Serial.write("OFF\n");
                break;
        }
    }
}
