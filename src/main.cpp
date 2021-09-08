#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET  4
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHT_PIN 2
#define DHT_TYPE DHT11

#define GYML8511_PIN A1

DHT_Unified dht(DHT_PIN, DHT_TYPE);

#define SENSOR_POLLING_DELAY 3000 // Must be greater than or equal to sensor.min_delay

unsigned long lastUpdate = millis();

void drawData(float temp, float humidity, float uvVolt) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.cp437(true);

    display.print(F("Temperature: "));
    display.println(!isnan(temp) ? String(temp) + "C" : "error");
    display.print(F("Humidity: "));
    display.println(!isnan(humidity) ? String(humidity) + "%" : "error");
    display.print(F("UV: "));
    display.println(!isnan(uvVolt) ? String(uvVolt) + "V (1-3.3V)" : "error");

    display.display();
}

void printSensorDetails() {
    Serial.println(F("Sensor details"));

    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print(F("Sensor Type: "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:  "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:   "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:   "));
    Serial.print(sensor.max_value);
    Serial.println(F("°C"));
    Serial.print(F("Min Value:   "));
    Serial.print(sensor.min_value);
    Serial.println(F("°C"));
    Serial.print(F("Resolution:  "));
    Serial.print(sensor.resolution);
    Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));

    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print(F("Sensor Type: "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:  "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:   "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:   "));
    Serial.print(sensor.max_value);
    Serial.println(F("%"));
    Serial.print(F("Min Value:   "));
    Serial.print(sensor.min_value);
    Serial.println(F("%"));
    Serial.print(F("Resolution:  "));
    Serial.print(sensor.resolution);
    Serial.println(F("%"));
    Serial.println(F("------------------------------------"));
}

void setup() {
    Serial.begin(9600);
    while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        delay(1000);
    }

    dht.begin();

    pinMode(GYML8511_PIN, INPUT);

    printSensorDetails();
}

void loop() {
    unsigned long curMillis = millis();

    if (curMillis - lastUpdate >= SENSOR_POLLING_DELAY) {
        sensors_event_t event;
        dht.temperature().getEvent(&event);
        float temp = event.temperature;

        dht.humidity().getEvent(&event);
        float humidity = event.relative_humidity;

        // @todo Преобразовывать выход датчика UV в понятный вид
        // 1 вольт = 0 интенсивность UV
        // 3.3 вольта = 15 интенсивность UV
        float uvData = (float) analogRead(GYML8511_PIN);
        float uvVolt = uvData / 1024 * 3.3;

        drawData(temp, humidity, uvVolt);

        lastUpdate = curMillis;
    }
}
