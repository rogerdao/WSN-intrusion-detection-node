#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <base64.h>

// ======================
// EDIT THESE
// ======================
const char* WIFI_SSID = "lzlzlzl’s iPhone";
const char* WIFI_PASS = "wuwuwa41";

const char* SMTP_SERVER      = "smtp.gmail.com";
const int   SMTP_PORT        = 465;

const char* EMAIL_USER       = "ddidid482048@gmail.com";       // Gmail address
const char* EMAIL_PASS       = "zomgzomgzomgzomg";      // 16-char app password, no spaces
const char* EMAIL_RECIPIENT  = "zzz0666@jkjk.edu";
// ======================

const int PIR_PIN = 14;
bool lastState = LOW;

WiFiClient httpClient;

bool waitForResponse(WiFiClientSecure &client, const char* expect, uint32_t timeout = 10000) {
  uint32_t start = millis();
  String resp = "";

  while (millis() - start < timeout) {
    while (client.available()) {
      char c = client.read();
      resp += c;
      Serial.print(c);
    }

    if (resp.indexOf(expect) != -1) {
      return true;
    }
  }

  Serial.println("\nTimeout waiting for response: expected token not found.");
  Serial.print("Expected: ");
  Serial.println(expect);
  Serial.print("Got: ");
  Serial.println(resp);
  return false;
}

bool sendEmailMotionDetected() {
  Serial.println("[DEBUG] sendEmailMotionDetected() called");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[DEBUG] WiFi not connected inside sendEmailMotionDetected");
    return false;
  }

  WiFiClientSecure mailClient;
  mailClient.setInsecure();
  mailClient.setTimeout(15000);

  Serial.print("[DEBUG] Connecting to SMTP server ");
  Serial.print(SMTP_SERVER);
  Serial.print(":");
  Serial.println(SMTP_PORT);

  if (!mailClient.connect(SMTP_SERVER, SMTP_PORT)) {
    Serial.println("[ERROR] SMTP connection failed in sendEmailMotionDetected");
    return false;
  }

  Serial.println("[DEBUG] SMTP TCP connection established, waiting for 220...");

  if (!waitForResponse(mailClient, "220")) return false;

  mailClient.println("EHLO esp8266");
  if (!waitForResponse(mailClient, "250")) return false;

  mailClient.println("AUTH LOGIN");
  if (!waitForResponse(mailClient, "334")) return false;

  mailClient.println(base64::encode(String(EMAIL_USER)));
  if (!waitForResponse(mailClient, "334")) return false;

  mailClient.println(base64::encode(String(EMAIL_PASS)));
  if (!waitForResponse(mailClient, "235")) return false;

  mailClient.print("MAIL FROM:<");
  mailClient.print(EMAIL_USER);
  mailClient.println(">");
  if (!waitForResponse(mailClient, "250")) return false;

  mailClient.print("RCPT TO:<");
  mailClient.print(EMAIL_RECIPIENT);
  mailClient.println(">");
  if (!waitForResponse(mailClient, "250")) return false;

  mailClient.println("DATA");
  if (!waitForResponse(mailClient, "354")) return false;

  mailClient.print("From: ESP8266 PIR <");
  mailClient.print(EMAIL_USER);
  mailClient.println(">");
  mailClient.print("To: <");
  mailClient.print(EMAIL_RECIPIENT);
  mailClient.println(">");
  mailClient.println("Subject: Motion detected!");
  mailClient.println("Content-Type: text/plain; charset=UTF-8");
  mailClient.println();
  mailClient.println("Motion has been detected.");
  mailClient.println(".");
  if (!waitForResponse(mailClient, "250")) return false;

  mailClient.println("QUIT");
  waitForResponse(mailClient, "221");
  mailClient.stop();

  Serial.println("[DEBUG] Email sent successfully.");
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // implemented sleep cycle:
  // 1. Send the email immediately upon waking/powering on
  if (!sendEmailMotionDetected()) {
    Serial.println("[ERROR] Failed to send email.");
  }

  // 2. Immediately enter Deep Sleep to save power
  // This reduces "Idle Listening"
  Serial.println("Task complete. Entering Deep Sleep...");
  ESP.deepSleep(0); 

  // --- IMPLEMENTATION END ---

}

void loop() {
  /*
  bool motion = digitalRead(PIR_PIN);

  if (motion == HIGH && lastState == LOW) {
    Serial.println("MOTION DETECTED!");

    Serial.print("[DEBUG] WiFi.status(): ");
    Serial.println(WiFi.status());

    if (!sendEmailMotionDetected()) {
      Serial.println("[ERROR] Failed to send email.");
    }
  }

  lastState = motion;
  delay(150);
  */
}