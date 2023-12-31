#include <M5Core2.h>
#include <M5_ENV.h>
#include <SD.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <string.h>

unsigned long sTim3 = 0, pTim3 = 0, cycleTime3 = 0, now4 = 0, last4 = 0;
WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "";
const char* password = "";
String value3;
char shortText[11];
String yearStr, monthStr, dayStr, hourStr, minStr, secStr;
const char* mqtt_server = "test.mosquitto.org";  // Adres brokera MQTT
// const char* mqtt_server = "broker.mqttdashboard.com";  // Adres brokera MQTT
#define MSG_BUFFER_SIZE (50)
int temperatureT = 0, pressureT = 0, humidityT = 0;
int lastTemp = 0, curentTemp = 0;
char msg[MSG_BUFFER_SIZE];
char msg2[300];
int value = 0;
void setupWifi();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reConnect();
String lastWatering = "YYYY-MM-DD HH:MM:SS";
RTC_TimeTypeDef RTCtime;
RTC_DateTypeDef RTCDate;
char timeStrbuff[64];
Preferences preferences;

String currentPlant = "";

float tempAvg = 0, humAvg = 0, soilHumAvg = 0, pressAvg = 0;
const unsigned long SOIL_MOISTURE_INTERVAL = 1000;
const float SOIL_MOISTURE_THRESHOLD = 44.5;

//const int SOIL_MOISTURE_THRESHOLD = 2100;

void page1a();
void page1b();
void page1c();
void page2();
void page3();
void page4();
void page5();
void page6();
void pageInfo();
void clearScreen();

void draw_switch_button();

TouchPoint_t touchPos;      // Pozycja punktu dotyku
bool touchPressed = false;  // Czy ekran jest wciśnięty?
bool buttonOn = false;      // Czy przycisk jest aktywny?
HotZone button(120, 120, 200, 200);

File dataFile;
char fileName[] = "/data2.txt";
SHT3X sht30;
QMP6988 qmp6988;
unsigned long now = 0, last = 0, nowPump = 0, pumpStartTime = 0, lastPump = 0, avrNow = 0, avrLast = 0, lastInfo = 0;
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

char refresh = 1;
int screenNum = 0, screenNumPrev = 0;
bool isFirstInfo = true;

#define INPUT_PIN 36
#define PUMP_PIN 26

#define E_CLICK 1

bool buttonsEnabled = false;
bool pumpRunning = false;

Button lt(14, 42, 110, 30, "left-top");
Button lb(177, 42, 110, 30, "left-bottom");
Button op(310 / 4, 220, 110, 30, "other-plant");

Button confirmationInfo(110, 200, 100, 35, "confirmationInfo");

boolean isButtonPressed = false;

float avg_temperature = 0, avg_pressure = 0, avg_humidity = 0, avg_soil_moisture = 0;

unsigned long lastWateringTime = 0;
int lastWateringDay = 0;

char* other_Plant[7];
boolean other_Plant_set = false;

void flushTime() {
  M5.Rtc.GetTime(&RTCtime);
  M5.Rtc.GetDate(&RTCDate);
  sprintf(timeStrbuff, "%d/%02d/%02d %02d:%02d:%02d", RTCDate.Year,
          RTCDate.Month, RTCDate.Date, RTCtime.Hours, RTCtime.Minutes,
          RTCtime.Seconds);
  M5.Lcd.setCursor(10, 100);
  M5.Lcd.println(timeStrbuff);
}

void setupTime() {
  RTCtime.Hours = 20;
  RTCtime.Minutes = 59;
  RTCtime.Seconds = 20;
  if (!M5.Rtc.SetTime(&RTCtime)) Serial.println("wrong time set!");
  RTCDate.Year = 2023;
  RTCDate.Month = 12;
  RTCDate.Date = 17;
  if (!M5.Rtc.SetDate(&RTCDate)) Serial.println("wrong date set!");
}

void colorButtons(Event& e) {
  Button& b = *e.button;
  if (!isButtonPressed) {
    M5.Lcd.fillRect(b.x, b.y, b.w, b.h, GREEN);
    isButtonPressed = true;
  } else {
    M5.Lcd.fillRect(b.x, b.y, b.w, b.h, BLACK);
    isButtonPressed = false;
  }
}

int choose = 0;

bool flag = true;
int rawADC = 0;
int maxMoisture = 2700; // Maksymalna wartość odczytu wilgotności
int minMoisture = 1000;    // Minimalna wartość odczytu wilgotności

float moisturePercentage = 0.0;


void ltClick(Event& e) {
  if (screenNum != 5) {
    choose = 1;

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawJpgFile(SD, "/ficus.jpg", 31, 84);
    M5.Lcd.setCursor(203, 50);
    M5.Lcd.print("Ficus");
    M5.Lcd.setCursor(145, 76);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("Comfort T: %d", temperatureT);

    M5.Lcd.setCursor(145, 102);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort % : %d", humidityT);

    M5.Lcd.setCursor(145, 128);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort P: %d", pressureT);

    M5.Lcd.setCursor(154, 153);
    M5.Lcd.print("description:");

    M5.Lcd.setCursor(157, 175);
    M5.Lcd.print("likes humidity;");

    M5.Lcd.setCursor(154, 193);
    M5.Lcd.print("good lighting");


    M5.Lcd.setCursor(153, 215);
    M5.Lcd.print("shiny leaves");
    
    lt.delHandlers();
    lb.delHandlers();
    op.delHandlers();
    buttonsEnabled = true;
  }
}

void lbClick(Event& e) {
  if (screenNum != 5) {
    choose = 2;

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.fillRect(66, 96, 20, 120, GREEN);
    M5.Lcd.fillCircle(75, 91, 15, GREEN);
    M5.Lcd.fillRect(122, 96, 15, 50, GREEN);
    M5.Lcd.fillRect(87, 141, 35, 24, GREEN);
    M5.Lcd.fillCircle(129, 154, 10, GREEN);
    M5.Lcd.fillRect(27, 141, 15, 40, GREEN);
    M5.Lcd.fillCircle(32, 171, 10, GREEN);
    M5.Lcd.fillRect(40, 163, 30, 20, GREEN);
    M5.Lcd.setCursor(203, 50);
    M5.Lcd.print("Cactus");
    M5.Lcd.setCursor(145, 76);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("Comfort T: %d", temperatureT);

    M5.Lcd.setCursor(145, 102);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort % : %d", humidityT);

    M5.Lcd.setCursor(145, 128);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort P: %d", pressureT);

    M5.Lcd.setCursor(154, 153);
    M5.Lcd.print("description:");

    M5.Lcd.setCursor(157, 175);
    M5.Lcd.print("long roots;");

    M5.Lcd.setCursor(154, 193);
    M5.Lcd.print("spines");

    M5.Lcd.setCursor(153, 215);
    M5.Lcd.print("stem is thick");
    lt.delHandlers();
    lb.delHandlers();
    op.delHandlers();
    buttonsEnabled = true;
  }
}

void confirmationInfoClick(Event& e) {
  if (screenNum == 5) {
    screenNum = screenNumPrev;
    refresh = 1;
  }
}

void otherPlant(Event& e) {
  if (other_Plant_set) {
    choose = 3;

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    String filePath = "/";
    filePath += other_Plant[0];
    filePath += ".jpg";
    M5.Lcd.drawJpgFile(SD, filePath.c_str(), 31, 84);
    M5.Lcd.setCursor(203, 50);
    M5.Lcd.print(other_Plant[0]);
    M5.Lcd.setCursor(145, 76);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("Comfort T: %s", other_Plant[1]);

    M5.Lcd.setCursor(145, 102);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort % : %s", other_Plant[2]);

    M5.Lcd.setCursor(145, 128);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("comfort P: %s", other_Plant[3]);

    M5.Lcd.setCursor(154, 153);
    M5.Lcd.print("description:");

    M5.Lcd.setCursor(157, 175);
    M5.Lcd.printf("%s", other_Plant[4]);

    M5.Lcd.setCursor(154, 193);
    M5.Lcd.printf("%s", other_Plant[5]);

    M5.Lcd.setCursor(153, 215);
    //M5.Lcd.printf("%s", other_Plant[6]);
    snprintf(shortText, sizeof(shortText), "%.10s", other_Plant[6]);
    M5.Lcd.printf("%s", shortText);  
    // usunięcie wszystkich zarejestrowanych funkcji dla przycisku lt
    lt.delHandlers();
    lb.delHandlers();
    op.delHandlers();
    buttonsEnabled = true;
  } else Serial.println("Nie ustawiono rosliny");
}

void setup() {
  Serial.begin(9600);
  M5.begin();
  preferences.begin("my-app", false);
  lastWatering = preferences.getString("watering","2023-01-01 00:00:00");
  setupTime();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.fillScreen(BLACK);
  lastWateringDay = EEPROM.read(0);
  EEPROM.get(1, lastWateringTime);
  Serial.print("Last watering time: ");
  Serial.print(lastWateringDay);
  Serial.print(", ");
  Serial.println(lastWateringTime);
  SD.begin(TFCARD_CS_PIN, SPI, 40000000);
  Wire.begin();
  qmp6988.init();
  setupWifi();
  client.setServer(mqtt_server, 1883);  // Sets the server details.
  client.setCallback(mqtt_callback);

  client.subscribe("PIR/L1/Z3/buttB");  // ponowna subskrypcja tematu
  client.subscribe("PIR/L1/condition/temperature");
  client.subscribe("PIR/L1/condition/humidity");
  client.subscribe("PIR/L1/condition/pressure");
  client.subscribe("PIR/L1/otherPlant");

  pinMode(INPUT_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(25, 0);
  lb.addHandler(lbClick, E_CLICK);
  lt.addHandler(ltClick, E_CLICK);
  op.addHandler(otherPlant, E_CLICK);
  confirmationInfo.addHandler(confirmationInfoClick, E_CLICK);

  M5.Lcd.drawString("Choose plant", 100, 16);
  M5.Lcd.drawString("Ficus", 47, 47);
  M5.Lcd.drawString("Cactus", 210, 49);
  M5.Lcd.drawString("Other plant", 310 / 4, 220);
  M5.Lcd.drawJpgFile(SD, "/cactus.jpg", 187, 102);
  M5.Lcd.drawJpgFile(SD, "/ficus.jpg", 24, 102);
}
void setupWifi() {

  delay(10);
  // Serial.print("Connecting to %s", ssid);
  Serial.begin(115200);

  Serial.print("Connecting to");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);         // Set the mode to WiFi station mode.
  WiFi.begin(ssid, password);  // Start Wifi connection.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nSuccess\n");
}
void reConnect() {
  Serial.begin(115200);
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "M5Stack-";
    clientId += String(random(0xffff), HEX);
    // Attempting to connect
    if (client.connect(clientId.c_str())) {

      Serial.println("\nSuccess\n");
      client.subscribe("PIR/L1/Z3/buttB");  // ponowna subskrypcja tematu
      client.subscribe("PIR/L1/condition/temperature");
      client.subscribe("PIR/L1/condition/humidity");
      client.subscribe("PIR/L1/condition/pressure");
      client.subscribe("PIR/L1/otherPlant");

    } else {
      Serial.println("failed, rc=");
      Serial.println(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void freeOtherPlantMemory() {
  // Zwolnienie pamięci dla other_Plant
  for (int j = 0; j < 7; j++) {
    free(other_Plant[j]);
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.println((char)payload[i]);
  }
  int n, k, l, m;
  int tmpVal;
  int t, t1, t2;
  char wiadomosc[100];  // Przykładowa maksymalna długość wiadomości, dostosuj do potrzeb

  if (strcmp(topic, "PIR/L1/Z3/buttB") == 0) {
    n = sscanf((char*)payload, "%d", &tmpVal);
    if (n == 1) {
      if (tmpVal == 1) {
        Serial.println(tmpVal);
        digitalWrite(PUMP_PIN, HIGH);

        M5.Rtc.GetTime(&RTCtime);
        M5.Rtc.GetDate(&RTCDate);
        // lastWatering = String(RTCDate.Year) + "-" +
        //                   String(RTCDate.Month) + "-" +
        //                   String(RTCDate.Date) + " " +
        //                   String(RTCtime.Hours) + ":" +
        //                   String(RTCtime.Minutes) + ":" +
        //                   String(RTCtime.Seconds);
      yearStr = String(RTCDate.Year);
      monthStr = (RTCDate.Month < 10) ? "0" + String(RTCDate.Month) : String(RTCDate.Month);
      dayStr = (RTCDate.Date < 10) ? "0" + String(RTCDate.Date) : String(RTCDate.Date);
      hourStr = (RTCtime.Hours < 10) ? "0" + String(RTCtime.Hours) : String(RTCtime.Hours);
      minStr = (RTCtime.Minutes < 10) ? "0" + String(RTCtime.Minutes) : String(RTCtime.Minutes);
      secStr = (RTCtime.Seconds < 10) ? "0" + String(RTCtime.Seconds) : String(RTCtime.Seconds);

      lastWatering = yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minStr + ":" + secStr;

        preferences.putString("watering", lastWatering);
        Serial.print("Watering time saved: ");
        Serial.print(lastWatering);
        //pumpRunning = true;

      } else if (tmpVal == 0) {
        Serial.println(tmpVal);
        digitalWrite(PUMP_PIN, LOW);
      }
    }
  } else if (strcmp(topic, "PIR/L1/condition/temperature") == 0) {
    k = sscanf((char*)payload, "%d", &t);
    if (k > 0) temperatureT = t;
  } else if (strcmp(topic, "PIR/L1/condition/humidity") == 0) {
    l = sscanf((char*)payload, "%d", &t1);
    if (l > 0) humidityT = t1;
  } else if (strcmp(topic, "PIR/L1/condition/pressure") == 0) {
    m = sscanf((char*)payload, "%d", &t2);
    if (m > 0) pressureT = t2;
  } else if (strcmp(topic, "PIR/L1/otherPlant") == 0) {
    m = sscanf((char*)payload, "%s", wiadomosc);
    Serial.println("\nOdebrano nową roślinę\n");
    Serial.printf("%s\n", wiadomosc);
    if (m > 0) {
      freeOtherPlantMemory();  // Zwolnienie pamięci przed przypisaniem nowych wartości
      other_Plant_set = true;
      char* token = strtok(wiadomosc, ";");
      int i = 0;

      while (token != NULL && i < 7) {
        // Alokacja pamięci dla wyrazu
        other_Plant[i] = (char*)malloc(strlen(token) + 1);

        // Kopiowanie wyrazu do nowo alokowanej pamięci
        strcpy(other_Plant[i], token);

        // Przechodzenie do kolejnego tokenu
        token = strtok(NULL, ";");
        Serial.printf("%s", other_Plant[i]);
        i++;
      }
    } else {
      Serial.println("\nBłędne dane dla nowej rośliny\n");
      Serial.printf("%d", m);
      freeOtherPlantMemory();  // Zwolnienie pamięci w przypadku błędu
    }
  }
}

void loop() {
  M5.update();
  if (!client.connected()) { reConnect(); }  // Reconnect after disconnection
  client.loop();                             // Processing of incoming messages
  now = millis();
  if (now - last > 10000) {
    last = now;
    pressure = qmp6988.calcPressure();
    if (sht30.get() == 0) {       //Obtain the data of shT30
      temperature = sht30.cTemp;  //Store the temperature obtained from shT30
      humidity = sht30.humidity;  //Store the humidity obtained from the SHT30
    } else {
      temperature = 0.0, humidity = 0.0;
    }
    rawADC = analogRead(INPUT_PIN);
    // moisturePercentage = ((float)(rawADC - minMoisture) / (float)(maxMoisture - minMoisture)) * 100.0;
    moisturePercentage = 100.0 - (((float)(rawADC - minMoisture) / (float)(maxMoisture - minMoisture)) * 100.0);
    // if (rawADC < SOIL_MOISTURE_THRESHOLD) {
    if (moisturePercentage < SOIL_MOISTURE_THRESHOLD) {
      digitalWrite(PUMP_PIN, HIGH);
      
      M5.Rtc.GetTime(&RTCtime);
      M5.Rtc.GetDate(&RTCDate);
      yearStr = String(RTCDate.Year);
      monthStr = (RTCDate.Month < 10) ? "0" + String(RTCDate.Month) : String(RTCDate.Month);
      dayStr = (RTCDate.Date < 10) ? "0" + String(RTCDate.Date) : String(RTCDate.Date);
      hourStr = (RTCtime.Hours < 10) ? "0" + String(RTCtime.Hours) : String(RTCtime.Hours);
      minStr = (RTCtime.Minutes < 10) ? "0" + String(RTCtime.Minutes) : String(RTCtime.Minutes);
      secStr = (RTCtime.Seconds < 10) ? "0" + String(RTCtime.Seconds) : String(RTCtime.Seconds);

      lastWatering = yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minStr + ":" + secStr;

      preferences.putString("watering", lastWatering);
      Serial.print("Watering time saved: ");
      Serial.print(lastWatering);
      
      lastWateringDay = RTCDate.Date;
      lastWateringTime = millis();
      EEPROM.write(0, lastWateringDay);
      EEPROM.put(1, lastWateringTime);
      EEPROM.commit();
      pumpStartTime = millis();
      pumpRunning = true;
    }

    //zapis do pliku
    dataFile = SD.open("/daneToday3.txt", FILE_WRITE);

    if (dataFile) {
      dataFile.print(temperature);
      dataFile.print(",");
      dataFile.print(humidity);
      dataFile.print(",");
      dataFile.print(moisturePercentage);
      dataFile.print(",");
      dataFile.println(pressure / 100);
      dataFile.close();
    } else {
      Serial.println("Błąd otwarcia pliku!");
    }
    sprintf(msg, "%.1f", moisturePercentage);  // Control variable
    client.publish("PIR/L1/Z3/moilSoilture", msg);
    sprintf(msg, "%.1f", temperature);
    // Setpoint value
    client.publish("PIR/L1/Z3/temperature", msg);  // Publication of message
    sprintf(msg, "%.1f%", humidity);               // Control variable
    client.publish("PIR/L1/Z3/humidity", msg);     // Publication of message

    sprintf(msg, "%.1f", pressure / 100);
    // Setpoint value
    client.publish("PIR/L1/Z3/pressure", msg);  // Publication of message
    
    sprintf(msg2, "{\"name\":\"%s\", \"timeWatering\":\"%s\", \"moilSoilture\":%.1f, \"temperature\":%.1f, \"humidity\":%.1f, \"pressure\":%.1f}",
    currentPlant, lastWatering.c_str(), moisturePercentage, temperature, humidity, pressure / 100);
    client.publish("PIR/L1/Z3/jsonData", msg2);
  }
  if ((temperature != 0.00 && isFirstInfo == true) || (isFirstInfo == false))
    if (temperature > 30 || temperature < 20) {
      Serial.print("temperature");
      Serial.print(temperature);
      Serial.print(isFirstInfo);
      Serial.print(screenNum);
      if (now - lastInfo > 360000 || isFirstInfo == true) {
        Serial.print("warunek");
        isFirstInfo = false;
        lastInfo = now;
        screenNumPrev = screenNum;
        screenNum = 5;
        refresh = 1;
      }
    } else if ((now - lastInfo > 20000 && screenNum == 5) || (temperature < 30 && temperature > 20 && screenNum == 5)) {
      screenNum = screenNumPrev;
      refresh = 1;
    }

  // Przechodzenie do następnego ekranu za pomocą przycisku A
  if (buttonsEnabled) {
    if (M5.BtnB.wasPressed()) {
      digitalWrite(PUMP_PIN, flag);

      M5.Rtc.GetTime(&RTCtime);
      M5.Rtc.GetDate(&RTCDate);
      yearStr = String(RTCDate.Year);
      monthStr = (RTCDate.Month < 10) ? "0" + String(RTCDate.Month) : String(RTCDate.Month);
      dayStr = (RTCDate.Date < 10) ? "0" + String(RTCDate.Date) : String(RTCDate.Date);
      hourStr = (RTCtime.Hours < 10) ? "0" + String(RTCtime.Hours) : String(RTCtime.Hours);
      minStr = (RTCtime.Minutes < 10) ? "0" + String(RTCtime.Minutes) : String(RTCtime.Minutes);
      secStr = (RTCtime.Seconds < 10) ? "0" + String(RTCtime.Seconds) : String(RTCtime.Seconds);

      lastWatering = yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minStr + ":" + secStr;

      preferences.putString("watering", lastWatering);
      Serial.print("Watering time saved: ");
      Serial.print(lastWatering);
      
      lastWateringDay = RTCDate.Date;
      lastWateringTime = millis();
      EEPROM.write(0, lastWateringDay);
      EEPROM.put(1, lastWateringTime);
      EEPROM.commit();
      flag = !flag;
    }
    if (pumpRunning && millis() - pumpStartTime >= 5000) {
      digitalWrite(PUMP_PIN, LOW);  // wyłączamy pompę
      pumpRunning = false;          // ustawiamy flagę, że pompa jest wyłączona
    }
    switch (screenNum) {
      case 0:
        if (choose == 1) {
          page1a();
        } else if (choose == 2) {
          page1b();
        } else if (choose == 3) {
          page1c();
        }
        break;
      case 1:
        page2();
        break;
      case 2:
        page3();
        break;
      case 3:
        page4();
        break;
      case 4:
        page5();
        break;
      case 5:
        page6();
        break;
    }
    if (M5.BtnA.wasPressed() && screenNum != 5) {
      screenNum++;
      if (screenNum > 4) {
        screenNum = 0;
      }
      refresh = 1;
    }

    // Przechodzenie do poprzedniego ekranu za pomocą przycisku C
    if (M5.BtnC.wasPressed() && screenNum != 5) {
      screenNum--;
      if (screenNum < 0) {
        screenNum = 4;
      }
      refresh = 1;
    }
    odczytajIZapiszSrednia();
  }
}
void page1a() {

  clearScreen();
  currentPlant = "ficus";
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);

  M5.Lcd.drawJpgFile(SD, "/ficus.jpg", 31, 84);

  M5.Lcd.setCursor(203, 50);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("Ficus");
  M5.Lcd.setCursor(145, 76);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Comfort T: %d", temperatureT);

  M5.Lcd.setCursor(145, 102);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("comfort H : %d", humidityT);

  M5.Lcd.setCursor(145, 128);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("comfort P: %d", pressureT);

  M5.Lcd.setCursor(145, 153);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("description:");

  M5.Lcd.setCursor(145, 175);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("likes humidit;");

  M5.Lcd.setCursor(145, 193);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("good lighting;");


  M5.Lcd.setCursor(145, 215);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("shiny leaves");
}

void page1b() {

  clearScreen();
  currentPlant = "cactus";
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.fillRect(66, 96, 20, 120, GREEN);
  M5.Lcd.fillCircle(75, 91, 15, GREEN);
  M5.Lcd.fillRect(122, 96, 15, 50, GREEN);
  M5.Lcd.fillRect(87, 141, 35, 24, GREEN);
  M5.Lcd.fillCircle(129, 154, 10, GREEN);
  M5.Lcd.fillRect(27, 141, 15, 40, GREEN);
  M5.Lcd.fillCircle(32, 171, 10, GREEN);
  M5.Lcd.fillRect(40, 163, 30, 20, GREEN);
  M5.Lcd.setCursor(203, 50);

  M5.Lcd.print("Cactus");
  M5.Lcd.setCursor(145, 76);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Comfort T: %d", temperatureT);
  M5.Lcd.setCursor(145, 102);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Comfort H : %d", humidityT);
  M5.Lcd.setCursor(145, 128);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Comfort P: %d", pressureT);

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(145, 153);
  M5.Lcd.print("Description:");

  M5.Lcd.setCursor(145, 175);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("long roots;");
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(145, 193);
  M5.Lcd.print("spines;");

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(145, 215);
  M5.Lcd.print("stem is thick");
}

void page1c() {

  clearScreen();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);

  if (strcmp(other_Plant[0], "grudnik") == 0) {
    currentPlant = "grudnik";
    M5.Lcd.drawJpgFile(SD, "/grudnik.jpg", 31, 84);
  } else if (strcmp(other_Plant[0], "Amarylis") == 0) {
    currentPlant = "amarylis";
    M5.Lcd.drawJpgFile(SD, "/Amarylis.jpg", 31, 84);
  } else if (strcmp(other_Plant[0], "Skrzydlokwiat") == 0) {
    currentPlant = "Skrzydlokwiat";
    M5.Lcd.drawJpgFile(SD, "/Skrzydlokwiat.jpg", 31, 84);
  } else if (strcmp(other_Plant[0], "Storczyk") == 0) {
    currentPlant = "Storczyk";
    M5.Lcd.drawJpgFile(SD, "/Storczyk.jpg", 31, 84);
  } else {
    M5.Lcd.setCursor(31, 84);
    M5.Lcd.printf("brak zdjecia dla: %s", other_Plant[0]);
  }

  M5.Lcd.setCursor(203, 50);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print(other_Plant[0]);
  M5.Lcd.setCursor(145, 76);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("Comfort T:%s", other_Plant[1]);

  M5.Lcd.setCursor(145, 102);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("comfort H:%s", other_Plant[2]);

  M5.Lcd.setCursor(145, 128);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("comfort P:%s", other_Plant[3]);

  M5.Lcd.setCursor(145, 153);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("description:");

  M5.Lcd.setCursor(145, 175);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%s", other_Plant[4]);

  M5.Lcd.setCursor(145, 193);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%s", other_Plant[5]);


  M5.Lcd.setCursor(145, 215);
  M5.Lcd.setTextColor(WHITE, BLACK);
  //M5.Lcd.printf("%s", other_Plant[6]);
  snprintf(shortText, sizeof(shortText), "%.10s", other_Plant[6]);
  M5.Lcd.printf("%s", shortText);
}

void page2() {

  clearScreen();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  if (temperature >= 10 && temperature <= 20) {

    M5.Lcd.drawJpgFile(SD, "/a.jpg", 29, 16);
    M5.Lcd.drawJpgFile(SD, "/b.jpg", 171, 13);
    M5.Lcd.setTextColor(WHITE, BLACK);  // Ustaw kolor tekstu na biały
    M5.Lcd.setTextSize(3);              // Ustaw rozmiar czcionki
    M5.Lcd.setCursor(20, 140);          // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.printf("Temperat:%.1f C", temperature);  // Wypisz tekst na ekranie

    M5.Lcd.setCursor(20, 170);  // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.printf("Pressure:%.1f", pressure / 100);  // Wypisz tekst na ekranie
    M5.Lcd.setCursor(20, 200);                       // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.printf("Humadity:%.1f%%", humidity);  // Wypisz tekst na ekranie
  } else {
    M5.Lcd.setTextColor(WHITE, BLACK);  // Ustaw kolor tekstu na biały
    M5.Lcd.setTextSize(3);              // Ustaw rozmiar czcionki

    M5.Lcd.drawJpgFile(SD, "/c.jpg", 29, 16);
    M5.Lcd.drawJpgFile(SD, "/d.jpg", 171, 13);



    M5.Lcd.setCursor(29, 140);  // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.printf("Temperat: %.1f C", temperature);  // Wypisz tekst na ekranie
    M5.Lcd.setCursor(29, 170);                       // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);

    M5.Lcd.printf("Pressure: %.1f", pressure / 100);  // Wypisz tekst na ekranie
    M5.Lcd.setCursor(29, 200);                        // Ustaw pozycję kursora
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("Humadity: %.1f%%", humidity);
  }
}
void page3() {
  clearScreen();
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawLine(37, 118, 261, 118, WHITE);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(24, 24);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("Last watering");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(80, 76);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print(lastWatering);

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.fillCircle(39, 91, 15, BLUE);
  M5.Lcd.fillRect(31, 52, 15, 30, BLUE);
  M5.Lcd.fillRect(23, 156, 30, 30, GREEN);

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(17, 134);
  M5.Lcd.print("Soil moisture");
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(76, 175);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print(String(moisturePercentage));
  M5.Lcd.fillRect(24, 180, 10, 30, BLUE);
  M5.Lcd.fillRect(44, 180, 10, 30, YELLOW);
  draw_switch_button();
}

void page4() {
  clearScreen();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(BLACK, WHITE);

  M5.Lcd.drawLine(14, 118, 313, 119, BLACK);
  M5.Lcd.drawLine(150, 237, 150, 5, BLACK);

  M5.Lcd.drawJpgFile(SD, "/a1.jpg", 43, 11);  // Przeskalowany dwukrotnie na mniejszy
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 83);
  M5.Lcd.setTextColor(BLACK, WHITE);
  int intTempAvg = int(tempAvg);
  M5.Lcd.printf("%d C", intTempAvg);

  M5.Lcd.drawJpgFile(SD, "/b1.jpg", 156, 11);  // Przeskalowany dwukrotnie na mniejszy
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(254, 83);
  M5.Lcd.setTextColor(BLACK, WHITE);
  int iSoilHumAvg = int(soilHumAvg);
  M5.Lcd.printf("%d %%", iSoilHumAvg);

  M5.Lcd.drawJpgFile(SD, "/c1.jpg", 43, 123);  // Przeskalowany dwukrotnie na mniejszy
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(7, 205);
  M5.Lcd.setTextColor(BLACK, WHITE);
  int iPressAvg = int(pressAvg);
  M5.Lcd.printf("%d hPa", iPressAvg);

  M5.Lcd.drawJpgFile(SD, "/d1.jpg", 156, 123);  // Przeskalowany dwukrotnie na mniejszy
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(258, 205);
  int intHumAvg = int(humAvg);
  M5.Lcd.printf("%d %%", intHumAvg);
}


void pageInfo() {
  if (temperature <= 20) {
    curentTemp = 0;
  } else if (temperature >= 30) {
    curentTemp = 1;
  } else {
    curentTemp = 2;
  }

  if (curentTemp != lastTemp) {
    refresh = 1;
    clearScreen();
  }

  if (temperature <= 20) {
    M5.Lcd.drawRoundRect(45, 70, 255, 120, 20, WHITE);  // rysowanie przycisku z zielonym kolorem
    M5.Lcd.setTextColor(BLACK, BLUE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(100, 86);
    M5.Lcd.print("Temperatura");
    M5.Lcd.setCursor(125, 106);
    M5.Lcd.print(" za mala:");
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(113, 143);
    M5.Lcd.printf("%.1fC", temperature);
    lastTemp = 0;
  } else if (temperature >= 30) {
    M5.Lcd.drawRoundRect(45, 70, 255, 120, 20, WHITE);  // rysowanie przycisku z zielonym kolorem
    M5.Lcd.setTextColor(BLACK, RED);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(100, 86);
    M5.Lcd.print("Temperatura");
    M5.Lcd.setCursor(125, 106);
    M5.Lcd.print("za wysoka:");
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(113, 143);
    M5.Lcd.printf("%.1fC", temperature);
    lastTemp = 1;
  } else {
    M5.Lcd.drawRoundRect(45, 70, 255, 120, 20, WHITE);  // rysowanie przycisku z zielonym kolorem
    M5.Lcd.setTextColor(BLACK, GREEN);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(100, 86);
    M5.Lcd.print("Temperatura");
    M5.Lcd.setCursor(127, 106);
    M5.Lcd.print("dobra:");
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(113, 143);
    M5.Lcd.printf("%.1fC", temperature);
    lastTemp = 2;
  }
}

void page5() {
  clearScreen();
  pageInfo();
}
void page6() {
  clearScreen();
  M5.Lcd.drawRect(110, 200, 100, 35, WHITE);

  if (temperature <= 20) {
    M5.Lcd.setTextColor(BLACK, BLUE);
  } else {
    M5.Lcd.setTextColor(BLACK, RED);
  }
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(157, 207);
  M5.Lcd.print("OK");
  pageInfo();
}


void clearScreen() {
  if (refresh == 1) {
    if (screenNum == 3) {
      M5.Lcd.fillScreen(WHITE);
    } else if (screenNum == 4 || screenNum == 5) {
      if (temperature <= 20) {
        M5.Lcd.fillScreen(BLUE);
      } else if (temperature >= 30) {
        M5.Lcd.fillScreen(RED);
      } else {
        M5.Lcd.fillScreen(GREEN);
      }
    } else {
      M5.Lcd.clear();
    }
    refresh = 0;
  }
}
void draw_switch_button() {
  M5.Lcd.setCursor(242, 165);  // ustawienie kursora
  M5.Lcd.setTextColor(WHITE);  // ustawienie koloru tekstu na biały
  M5.Lcd.setTextSize(2);       // ustawienie rozmiaru tekstu na 1
  M5.Lcd.print("PUMP");        // wyświetlenie napisu "Switch" na przycisku
  if (flag == 1) {
    M5.Lcd.drawRoundRect(224, 188, 80, 40, 20, RED);  // rysowanie przycisku z zielonym kolorem
    M5.Lcd.setCursor(242, 200);         // ustawienie kursora
    M5.Lcd.setTextColor(WHITE, BLACK);  // ustawienie koloru tekstu na biały
    M5.Lcd.setTextSize(3);              // ustawienie rozmiaru tekstu na 1
    M5.Lcd.print("OFF");                // wyświetlenie napisu "Switch" na przycisku
  } else {
    M5.Lcd.drawRoundRect(224, 188, 80, 40, 20, GREEN);  // rysowanie przycisku z zielonym kolorem
    M5.Lcd.setCursor(242, 200);         // ustawienie kursora
    M5.Lcd.setTextColor(WHITE, BLACK);  // ustawienie koloru tekstu na biały
    M5.Lcd.setTextSize(3);              // ustawienie rozmiaru tekstu na 1
    M5.Lcd.print("ON1");                // wyświetlenie napisu "Switch" na przycisku
  }
}
void odczytajIZapiszSrednia() {
  File myFile = SD.open("/daneToday3.txt", FILE_READ);
  if (!myFile) {
    Serial.println("Nie można otworzyć pliku");
    return;
  }
  float tempSum = 0, humSum = 0, soilHumSum = 0, pressSum = 0;
  int count = 0;
  while (myFile.available()) {
    String line = myFile.readStringUntil('\n');
    if (line.length() > 0) {
      float temp, hum, soilHum, press;
      int pos = line.indexOf(',');
      temp = line.substring(0, pos).toFloat();
      line = line.substring(pos + 1);
      pos = line.indexOf(',');
      hum = line.substring(0, pos).toFloat();
      line = line.substring(pos + 1);
      pos = line.indexOf(',');
      soilHum = line.substring(0, pos).toFloat();
      line = line.substring(pos + 1);
      press = line.toFloat();

      tempSum += temp;
      humSum += hum;
      soilHumSum += soilHum;
      pressSum += press;
      count++;
    }
  }
  myFile.close();
  if (count > 0) {
    avrNow = millis();
    if (avrNow - avrLast > 10000) {
      avrLast = avrNow;
      tempAvg = tempSum / count;
      humAvg = humSum / count;
      soilHumAvg = soilHumSum / count;
      pressAvg = pressSum / count;
    }
  } else {
    Serial.println("Brak danych do obliczenia średniej");
  }
}