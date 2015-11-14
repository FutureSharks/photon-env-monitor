// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMAX17043/SparkFunMAX17043.h"

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_BMP085/Adafruit_BMP085.h"
Adafruit_BMP085 bmp;

// This #include statement was automatically added by the Spark IDE.
#include "HTU21D/HTU21D.h"
HTU21D htu = HTU21D();

// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"
HttpClient http;
http_request_t request;
http_response_t response;
http_header_t headers[] = { { NULL, NULL } };

// Initialize BMP085
void InitializeBMP085(){
	if (!bmp.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	}
}

// BH1750 stuff
int BH1750_I2CADDR = 0x23;
int BH1750_MODE = 0x21;

void BH1750_SETUP(){
    Wire.begin();
    Wire.beginTransmission(BH1750_I2CADDR);
    Wire.write(BH1750_MODE);
    Wire.endTransmission();
    delay(150);
}

int BH1750_READ(){
    if (BH1750_MODE == 0x20 || BH1750_MODE == 0x21 || BH1750_MODE == 0x23) {
        BH1750_SETUP();
    }
    Wire.begin();
    Wire.beginTransmission(BH1750_I2CADDR);
    Wire.requestFrom(BH1750_I2CADDR, 2);
    int level = Wire.read();
    level <<= 8;
    level |= Wire.read();
    Wire.endTransmission();
    level = level/1.2;
    return level;
}

// Where to publish the data
String publishMethod = "dweet";

// The dweet thing name
String my_dweet_thing = "photon_env_monitor";

// Sleep calculation multiplier. Set higher when using a lower power solar panel.
int sleepCalculationMultiplier = 70;

// Enable debug or not
bool debugEnabled = false;

void setup() {
    if (debugEnabled) {
        Serial.begin(9600);
    }
    // Start I2C
    Wire.begin();
    // Initialize the MAX17043 LiPo fuel gauge
    lipo.begin();
    lipo.quickStart();
    lipo.setThreshold(10);
    // BMP085 sensor
	InitializeBMP085();
	// Initialise HTU21D sensor
	while(! htu.begin()){
	    Serial.println("HTU21D not found");
	    delay(1000);
	}
	//Initialise BH1750 sensor
	BH1750_SETUP();
}

int send_to_dweet(String thing, String key, float value) {
    request.hostname = "dweet.io";
    request.port = 80;
    request.path = "/dweet/for/" + thing + "?" + key + "=" + value;
    request.body = "";
    http.post(request, response, headers);
    if (debugEnabled) {
        String msg = "Sent key " + key + " with value " + value + " to dweet for the " + thing + " and got response: " + response.status;
        Serial.println(msg);
    }
    return response.status;
}

int flash_rgb(int r=0, int g=0, int b=0, long time_delay=40) {
    RGB.control(true);
    RGB.color(0, 0, 0);
    delay(time_delay);
    RGB.color(r, g, b);
    delay(time_delay);
    RGB.color(0, 0, 0);
    delay(time_delay);
    RGB.control(false);
}

int calculate_sleep(double soc) {
    return (105 - soc) * sleepCalculationMultiplier;
}

int flashLedByHttpCode(long httpStatus) {
    if (httpStatus == 200) {
        flash_rgb(0, 255);
    } else {
        flash_rgb(255, 0);
    }
}

void loop() {
    float temperature = htu.readTemperature();
    float airPressure = bmp.readPressure();
    float humidity = htu.readHumidity();
    float batteryCharge = lipo.getSOC();
    float batteryVoltage = lipo.getVoltage();
    float lightLevel = BH1750_READ();

    int sleepInterval = calculate_sleep(batteryCharge);

    if (publishMethod == "particle") {
        Particle.publish("temperature", String(temperature));
        delay(2000);
        Particle.publish("airPressure", String(airPressure));
        delay(2000);
        Particle.publish("humidity", String(humidity));
        delay(2000);
        Particle.publish("batteryCharge", String(batteryCharge));
        delay(2000);
        Particle.publish("batteryVoltage", String(batteryVoltage));
        delay(2000);
        Particle.publish("lightLevel", String(lightLevel));
        delay(2000);
    }

    if (publishMethod == "dweet") {
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "temperature", round(temperature)));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "airPressure", airPressure));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "humidity", round(humidity)));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "batteryVoltage", batteryVoltage));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "sleepInterval", sleepInterval));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "lightLevel", lightLevel));
        flashLedByHttpCode(send_to_dweet(my_dweet_thing, "batteryCharge", round(batteryCharge)));
    }

    System.sleep(SLEEP_MODE_DEEP, sleepInterval);
}
