#include <ESP32Servo.h>
#include "WiFi.h"

/**
 *      One Day Of Code
 * Arduino Car - Example code
 *       Board: Esp32
 *  Control Bluetooth Serial
 */
#define PORT 50123 //Change PORT if you need, alsop you have to set the same port in the mobile app
IPAddress ipAddress = IPAddress(192, 168, 0, 12); // Smartphone ip
WiFiClient client;

const char *ssid = "<YOUR WIFI SSID>";
const char *password = "<YOUR WIFI PASS>";

// Pins to use
int PIN_SERVO_A = 18;
int PIN_SERVO_B = 19;

// Left tires
int PIN_IN1 = 27;
int PIN_IN2 = 26;

// Right tires
int PIN_IN3 = 25;
int PIN_IN4 = 33;

// Led/Light
int PIN_LED = 12;
int PIN_LED_2 = 14;

// Distance sensor HC-SR04
int PIN_ECHO = 22;
int PIN_TRIG = 23;

// Servos
Servo servoA;
Servo servoB;

//Aux
String numericPart = "";
char codeReceived;

void setup() {
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // Init serial
    Serial.begin(115200);

    // Init wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while (1) {
            delay(1000);
        }
    }
    Serial.print("WiFi connected with IP:");
    Serial.println(WiFi.localIP());

    // Config pins
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    pinMode(PIN_IN3, OUTPUT);
    pinMode(PIN_IN4, OUTPUT);

    // Init servo
    servoA.setPeriodHertz(50);// Standard 50hz servo
    servoA.attach(PIN_SERVO_A, 500, 2400);
    servoB.setPeriodHertz(50);// Standard 50hz servo
    servoB.attach(PIN_SERVO_B, 500, 2400);

    // Init led/light
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_LED_2, OUTPUT);

    // Init distance sensor
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);

    connect();
}

void connect() {
    if (!client.connect(ipAddress, PORT)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    Serial.printf("Connected to: %s\n", client.remoteIP().toString());

}

void loop() {

    if (!client.connected()) { // Reconnect if not connected
        connect();
    }


    while (client.available() > 0) {
        String line = client.readStringUntil('\n');
        Serial.printf("code received: %s\n", line);
        numericPart = "";
        for (int i = 0; i < line.length(); i++) {
            int character = line[i];
            if (isDigit(character)) {
                numericPart += (char) character;
            } else if (character != '\n') {
                codeReceived = character;
            } else {
                break;
            }
        }
    }

    switch (codeReceived) {
        case 'F': goForward(); break;
        case 'L': turnLeft(); break;
        case 'B': goBack(); break;
        case 'R': turnRight(); break;
        case 'S': stop(); break;
        case 'M': turnOnLed(); break;
        case 'm': turnOffLed(); break;
        case 'N': turnOnLed2(); break;
        case 'n': turnOffLed2(); break;
        case 'J':
            if (numericPart != "") {
                rotateServoA(numericPart.toInt());
            }
            break;
        case 'K':
            if (numericPart != "") {
                rotateServoB(numericPart.toInt());
            }
            break;
    }

    client.printf("D%d cm\n", measureDistance());

    delay(10);
}

long measureDistance() {
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(5);
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    long duration = pulseIn(PIN_ECHO, HIGH);

    return microsecondsToCentimeters(duration);
}

long microsecondsToCentimeters(long microseconds) {
    return microseconds / 29 / 2;
}

void turnOnLed() {
    digitalWrite(PIN_LED, HIGH);
}

void turnOffLed() {
    digitalWrite(PIN_LED, LOW);
}

void turnOnLed2() {
    digitalWrite(PIN_LED_2, HIGH);
}

void turnOffLed2() {
    digitalWrite(PIN_LED_2, LOW);
}

void rotateServoA(int grados) {
    servoA.write(grados);
}

void rotateServoB(int grados) {
    servoB.write(grados);
}

void goForward() {
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);

    digitalWrite(PIN_IN3, HIGH);
    digitalWrite(PIN_IN4, LOW);
}

void goBack() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, HIGH);

    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);
}

void stop() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);

    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
}

void turnLeft() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, HIGH);

    digitalWrite(PIN_IN3, HIGH);
    digitalWrite(PIN_IN4, LOW);
}

void turnRight() {
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);

    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);
}