// Chage These Credentials with your Blynk Template credentials
#define BLYNK_TEMPLATE_ID "TMPL61PGKOXdK"
#define BLYNK_TEMPLATE_NAME "FinalProject"
#define BLYNK_AUTH_TOKEN "PlJ5YLCJWmevODnXG8zuEUWzDt_jXKII"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <HTTPClient.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Realmi 8i";  // Change your Wifi/ Hotspot Name
char pass[] = "DimasFirman";    // Change your Wifi/ Hotspot Password

BlynkTimer timer;

#define fire 13
#define buzzer 4
#define MQ135_PIN 34
#define DHTPIN 15
#define DHTTYPE DHT22
#define RELAY_PIN 5

DHT dht(DHTPIN, DHTTYPE);

String url;
WiFiClient client;

int fire_Val = 0;
float h = 0;
float t = 0;
int mq135Value = 0;

void setup()  //Setup function - only function that is run in deep sleep mode
{
  Serial.begin(115200);  //Start the serial output at 9600 baud
  pinMode(fire, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  dht.begin();
  Blynk.begin(auth, ssid, pass);  //Splash screen delay
}

void loop()  //Loop function
{
  Blynk.run();
  mySensor();
  delay(200);
}

void mySensor() {
  fire_Val = digitalRead(fire);

  if (fire_Val == LOW) {
    Blynk.virtualWrite(V2, 1);
    Serial.print("fIRE Level: ");
    Serial.println(fire_Val);
  }

  else {
    Blynk.virtualWrite(V2, 0);
    Serial.print("fIRE Level: ");
    Serial.println(fire_Val);
  }
  mq135Value = analogRead(MQ135_PIN);
  Blynk.virtualWrite(V3, mq135Value);
  Serial.println("MQ 135: ");
  Serial.println(mq135Value);

  h = dht.readHumidity();
  t = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit
  Serial.println("");
  Serial.print(F("Humidity Room: "));
  Serial.print(h);
  Serial.println(F("% "));
  Serial.print(F("Temperature Room: "));
  Serial.print(t);
  Serial.println(F("°C  "));

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  delay(1000);

  if (fire_Val == LOW) {
    // If any condition is met, activate the buzzer and relay
    Blynk.logEvent("fire_event");
    kirimwa("Bahaya!!! Telah ditemukan api di tempat anda.\nSegera Evakuasi dari tempat tersebut.");
    digitalWrite(buzzer, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Fire detected! Activating buzzer and relay.");

  } else if (mq135Value >= 1000) {
    kirimwa("Bahaya!!! Telah ditemukan gas di tempat anda. \n Nilai Gas : " + String(mq135Value) + "\n     Segera Evakuasi dari tempat tersebut.");
    digitalWrite(buzzer, HIGH);
  } else if (t >= 37) {
    kirimwa("Peringatan!!! Suhu di tempat anda mencapai " + String(t) + " °C. Segera Ambil Tindakan.");
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("No fire detected.");
  }
}

void kirimwa(String pesan) {

  url = "http://api.callmebot.com/whatsapp.php?phone=6282334641928&text=" + urlencode(pesan) + "&apikey=5487003";
  postData();
}

void postData() {
  int httpCode;
  HTTPClient http;
  http.begin(client, url);
  httpCode = http.POST(url);
  if (httpCode == 200) {
    Serial.println("Notifikasi WhatsApp Berhasil Terkirim");
  } else {
    Serial.println("Notifikasi WhatsApp Gagal Terkirim");
  }
  http.end();
}


String urlencode(String str) {

  String encodedString = "";
  char c;
  char code0, code1, code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  Serial.println(encodedString);
  return encodedString;
}
