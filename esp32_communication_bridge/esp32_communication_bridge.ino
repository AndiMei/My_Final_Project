//#define SERIAL_DEBUG

enum state {
  BT_on, BT_off, WiFi_on, WiFi_off
} radio_state;
/* <-----------| Include Library |-----------> */
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <WebSocketServer.h>

/* <-----------| Global Definition |-----------> */
#define AP_SSID           "EQ-JOSS"
#define AP_PASS           "ANDI1234"
#define WIFI_PORT         80
#define LED_BLUE          15
#define LED_RED           2

/* <-----------| Object Definition|-----------> */
WiFiServer myServer(WIFI_PORT);          // Object WiFi server
//WiFiClient myClient = myServer.available();
BluetoothSerial SerialBT;                // Object Bluetooth Serial
WebSocketServer webSocketServer;         // Object Web server
WiFiClient myClient;

/* <-----------| Object Variable|-----------> */
//int incomingByte;
//char data_serial_rx[21];
//char data_serial_tx[] = "1#2#+12.0#12500#0.35";

char inByteS2;
char inByteBT;
//char inByteWiFi;
//String dataDariHP;
//String dataKeHP;
//char inCharBT;

//bool detectNewLine=false;

bool readyReceiveWifi = 0;
bool isConnectedWifi = 0;

//String inString;
String strSerial2;
String strBT;
String strWiFi;

/* =============================  Fungsi - Prototipe =============================  */
void Switch_WiFi(bool state);
void WiFi_Stream(void);
void Switch_Bluetooth(bool state);
void Bluetooth_Stream(void);
void send_data_STM32(char send_data[]);

/* ================================= Fungsi - Setup ==============================  */
void setup() {
  /* <-----------| Inisialisasi Serial 0 |-----------> */
  Serial.begin(115200);

  /* <-----------| Inisialisasi Serial 1 |-----------> */
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_RED, HIGH);
}

/* ============================= Fungsi - Perulangan ============================== */
void loop() {

  /* Baca String Serial 2 */
  if (Serial2.available()) {
    inByteS2 = Serial2.read();
    strSerial2 += inByteS2;
  }

  if (radio_state == BT_on) {
    /* Baca String Serial BT */
    if (SerialBT.available()) {
      inByteBT = SerialBT.read();
      strBT += inByteBT;
    }
  }

  if (inByteS2 == '\n') {
    inByteS2 = '\0';
    if (strSerial2 == "BTON\n") {
      Switch_Bluetooth(1);
      radio_state = BT_on;
    }
    else if (strSerial2 == "BTOFF\n") {
      Switch_Bluetooth(0);
      radio_state = BT_off;
    }
    else if (strSerial2 == "WIFION\n") {
      Switch_WiFi(1);
      radio_state = WiFi_on;
    }
    else if (strSerial2 == "WIFIOFF\n") {
      Switch_WiFi(0);
      radio_state = WiFi_off;
    }
    else {
#ifdef SERIAL_DEBUG
      Serial.print("head:");
      Serial.print(strSerial2);
#endif
      /* data serial_2 dikirim ke BT */
      if (radio_state == BT_on) {
        SerialBT.print(strSerial2);
      }
      /* data serial_2 dikirim ke WiFi */
      else if (radio_state == WiFi_on) {
#ifdef SERIAL_DEBUG
        webSocketServer.sendData(strSerial2);
#endif
        Serial.println(strSerial2);
      }
    }
    strSerial2 = "";
  }

  if (radio_state == BT_on) {
    if (inByteBT == '\n') {
      inByteBT = '\0';
#ifdef SERIAL_DEBUG
      Serial.print(strBT);
#endif
      Serial2.print(strBT);
      strBT = "";
    }
  }
  if (readyReceiveWifi) {
    if (!isConnectedWifi) {
      myClient = myServer.available();
      if (myClient.connected() && webSocketServer.handshake(myClient)) {
#ifdef SERIAL_DEBUG
        Serial.println("Client Connected !");
#endif
        webSocketServer.sendData("OK");
        isConnectedWifi = true;
      }
    }
    if (myClient.connected()) {
      strWiFi = webSocketServer.getData();
      if (strWiFi.length() > 0) {
#ifdef SERIAL_DEBUG
        Serial.println(strWiFi);
#endif
        Serial2.print(strWiFi);
        strWiFi = "";
      }
    }
  }
}

void Switch_WiFi(bool state) {
  switch (state) {
    case 0:
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      readyReceiveWifi = false;
      isConnectedWifi = false;
      digitalWrite(LED_BLUE, HIGH);
      break;
    case 1:
      /* <-----| Inisialisasi WiFi |-----> */
      WiFi.mode(WIFI_AP);
      WiFi.softAP(AP_SSID, AP_PASS);      // Init soft AP
      delay(200);                         // Tunggu untuk Init
      Serial.println("Setting WiFi Access Point ...");
      IPAddress IP(192, 168, 1, 1);       // Atur IP Address
      IPAddress NMask(255, 255, 255, 0);  // Atur Mask
      WiFi.softAPConfig(IP, IP, NMask);   // Konfigurasi Soft AP
      IPAddress myIP = WiFi.softAPIP();   // Ambil IP setelah config
      Serial.print("AP IP Address: ");
      Serial.println(myIP);
      myServer.begin();                 // Start WiFi
      Serial.println("WiFi Server Started !");
      readyReceiveWifi = 1;
      digitalWrite(LED_BLUE, LOW);
      break;
  }
}
void Switch_Bluetooth(bool state) {
  switch (state) {
    case 0:
      btStop();
      digitalWrite(LED_RED, HIGH);
      break;
    case 1:
      btStart();
      /* <-----------| Inisialisasi Bluetooth Serial |-----------> */
      Serial.println("Setting BLE ...");
      SerialBT.begin(AP_SSID);            // Start Bluetooth
      Serial.println("BLE Server Started !");
      digitalWrite(LED_RED, LOW);
      break;
  }
}
