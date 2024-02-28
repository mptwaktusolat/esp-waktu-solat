#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>
#include <TimeLib.h>

const int timeZoneOffset = 3600 * 8; // Adjust this value according to your time zone

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wifi
const char *ssid = "iqf11";        // Change this to your WiFi SSID
const char *password = "meow4000"; // Change this to your WiFi password

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    // Below function is not working to offset the time later in our JSON response
    // adjustTime(timeZoneOffset);

    // Setup OLED display
    display.setRotation(2); // https://learn.adafruit.com/adafruit-gfx-graphics-library/rotating-the-display
    display.begin(0x3C, true);
    display.setContrast(0x20); // dim display (0 -127)
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    display.print("Connecting to ");
    display.println(ssid);
    display.display();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connected");
    display.println("IP address: ");
    display.println(WiFi.localIP());
    display.display();

    makeGETRequest();
}

void loop()
{
}
void makeGETRequest()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Your REST API endpoint URL
        String url = "https://api.waktusolat.app/v2/solat/sgr01";

        Serial.print("Making GET request to: ");
        Serial.println(url);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Making GET request to: ");
        display.println(url);
        display.display();

        http.begin(url);                   // Specify the URL
        int httpResponseCode = http.GET(); // Make the GET request

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            // Get response payload
            String payload = http.getString();
            Serial.println("Response payload:");
            Serial.println(payload);
            // display.setCursor(0, 0);
            // display.clearDisplay();
            // display.print(payload);
            // display.display();

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, payload);
            if (err)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(err.c_str());
                return;
            }

            const char *zone = doc["zone"];
            JsonArray prayers = doc["prayers"].as<JsonArray>();
            Serial.println(zone);

            // loop through the array and dsiplay the "hijri" date
            for (JsonVariant v : prayers)
            {
                const char *hijri = v["hijri"];
                const int day = v["day"];
                const int fajr = v["fajr"].as<int>() + timeZoneOffset;
                const int syuruk = v["syuruk"].as<int>() + timeZoneOffset;
                const int dhuhr = v["dhuhr"].as<int>() + timeZoneOffset;
                const int asr = v["asr"].as<int>() + timeZoneOffset;
                const int maghrib = v["maghrib"].as<int>() + timeZoneOffset;
                const int isha = v["isha"].as<int>() + timeZoneOffset;
                // Convert Unix timestamps to human-readable format
                char buffer[30]; // Buffer for holding the formatted time

                // Serial prints
                Serial.print(day);
                // Serial.print(": ");
                // Serial.println(hijri);
                // Serial.print("Fajr: ");
                // sprintf(buffer, "%02d:%02d", hour(fajr), minute(fajr));
                // Serial.println(buffer);
                // Serial.print("Syuruk: ");
                // sprintf(buffer, "%02d:%02d", hour(syuruk), minute(syuruk));
                // Serial.println(buffer);
                // Serial.print("Dhuhr: ");
                // sprintf(buffer, "%02d:%02d", hour(dhuhr), minute(dhuhr));
                // Serial.println(buffer);
                // Serial.print("Asr: ");
                // sprintf(buffer, "%02d:%02d", hour(asr), minute(asr));
                // Serial.println(buffer);
                // Serial.print("Maghrib: ");
                // sprintf(buffer, "%02d:%02d", hour(maghrib), minute(maghrib));
                // Serial.println(buffer);
                // Serial.print("Isha: ");
                // sprintf(buffer, "%02d:%02d", hour(isha), minute(isha));
                // Serial.println(buffer);
                // Serial.println();

                // Display prints
                display.setCursor(0, 0);
                display.clearDisplay();
                display.print(day);
                display.print(": ");
                display.println(hijri);
                display.print("Fajr: ");
                sprintf(buffer, "%02d:%02d", hour(fajr), minute(fajr));
                display.println(buffer);
                display.print("Syuruk: ");
                sprintf(buffer, "%02d:%02d", hour(syuruk), minute(syuruk));
                display.println(buffer);
                display.print("Dhuhr: ");
                sprintf(buffer, "%02d:%02d", hour(dhuhr), minute(dhuhr));
                display.println(buffer);
                display.print("Asr: ");
                sprintf(buffer, "%02d:%02d", hour(asr), minute(asr));
                display.println(buffer);
                display.print("Maghrib: ");
                sprintf(buffer, "%02d:%02d", hour(maghrib), minute(maghrib));
                display.println(buffer);
                display.print("Isha: ");
                sprintf(buffer, "%02d:%02d", hour(isha), minute(isha));
                display.println(buffer);
                display.display();
            }
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
            display.setCursor(0, 0);
            display.clearDisplay();
            display.print(httpResponseCode);
            display.display();
        }

        http.end(); // Close connection

        // Disconnect from WiFi
        WiFi.disconnect();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}