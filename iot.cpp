// --- Blynk defines (MUST be before Blynk includes) ---
#define BLYNK_TEMPLATE_ID "TMPL30wIOmglm"
#define BLYNK_TEMPLATE_NAME "STAFF MONITORING"
#define BLYNK_AUTH_TOKEN "15j2P3WAde2JuqLwQy2hCynVZzm-_an5"

// --- Libraries ---
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal.h>
#include <DFRobot_DHT11.h>
#include <SPI.h>
#include <MFRC522.h>

// LCD
LiquidCrystal lcd(15, 2, 4, 16, 17, 22);

// RFID
#define SS_PIN  5   // SDA
#define RST_PIN 0   // RST
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Correct RFID UID
String correctUID = "13582331";

// Toggle counter
int a = 1;

// WiFi credentials
char ssid[] = "Iot";    
char pass[] = "iot12345"; 

void setup() {
  Serial.begin(9600); 
  SPI.begin(18, 19, 23, SS_PIN);  
  mfrc522.PCD_Init();

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" STAFF LOCATION"); 
  lcd.setCursor(0, 1);
  lcd.print("MONITORING SYSTEM");
  delay(3000);         
  lcd.clear();

  // Connect to WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

String uidToHex(MFRC522::Uid uid) {
  String s = "";
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) s += "0";
    s += String(uid.uidByte[i], HEX);
  }
  s.toUpperCase();  
  return s;
}

void loop() {
  Blynk.run();  // Run Blynk

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String scannedUID = uidToHex(mfrc522.uid);
  Serial.print("Scanned UID: ");
  Serial.println(scannedUID);

  if (scannedUID == correctUID) {
    a = a + 1;
    if (a % 2 == 0) {
      Serial.println("Authorized access - Staff Entered");
      lcd.setCursor(0, 0);
      lcd.print("ENTER INTO CLASS"); 
      lcd.setCursor(0, 1);
      lcd.print("WELCOME     ");
      Blynk.virtualWrite(V0, "Staff is IN Classroom");  // Send to Blynk
      delay(1000);
      lcd.clear();
    } else {
      Serial.println("Staff Left");
      lcd.setCursor(0, 0);
      lcd.print("LEAVE FROM CLASS"); 
      lcd.setCursor(0, 1);
      lcd.print("THANK YOU  ");
      Blynk.virtualWrite(V0, "Staff is OUT of Classroom");  // Send to Blynk
      delay(1000);
      lcd.clear();
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Card Not Match"); 
    lcd.setCursor(0, 1);
    lcd.print("    ");
    delay(1000);
    lcd.clear();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(500);
}