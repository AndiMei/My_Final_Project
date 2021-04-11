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
BluetoothSerial SerialBT;                // Object Bluetooth Serial
WebSocketServer webSocketServer;         // Object Web server

/* <-----------| Object Variable|-----------> */
int incomingByte;
char data_serial_rx[21];
char data_serial_tx[] = "1#2#+12.0#12500#0.35";

String dataDariHP;
String dataKeHP;
char inCharBT;

String inString;
int8_t i = 0;

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
  /* Baca String Serial */
  while (Serial2.available()) {
    char inByte = Serial2.read();
    inString += inByte;
  }
  /* Menerjemahkan Perintah */
  if (inString.length() > 0) {
    Serial.println(inString);
    if (inString == "BTON$") {
      Switch_Bluetooth(1);
    }
    else if (inString == "BTOFF$") {
      Switch_Bluetooth(0);
    }
    else if (inString == "WIFION$") {
      Switch_WiFi(1);
    }
    else if (inString == "WIFIOFF$") {
      Switch_WiFi(0);
    }
    inString = "";
  }
  Serial2.print("WP$");
  delay(1);
  Serial2.print("0#1#2#3#4$");
  delay(100);
  }

/* ================================= Fungsi- Fungsi ================================= */
void send_data_STM32(char send_data[]) {
  Serial2.write(send_data);
  Serial2.write('\0');
}
void WiFi_Stream() {
  WiFiClient myClient = myServer.available();         // new Object Client
  //  if (myClient) {
  //    Serial.println("New Client !");
  if (myClient.connected() && webSocketServer.handshake(myClient)) {
    Serial.println("Client Connected !");
    webSocketServer.sendData("OK");
    while (myClient.connected()) {
      dataDariHP = webSocketServer.getData();
      if (dataDariHP.length() > 0) {
        Serial.println("recv: " + dataDariHP);
        dataKeHP = "replied:" + dataDariHP;
        webSocketServer.sendData(dataKeHP);
        if (dataDariHP == "CLOSE") {
          myClient.stop();
        }
      }
      delay(10);
      //              Serial.println("Client disconnected !");
    }
    Serial.println("Client disconnected !");
    delay(100);
    //        }
  }
  delay(100);
}
void Bluetooth_Stream() {
  if (SerialBT.available()) {
    inCharBT = SerialBT.read();
    dataDariHP += inCharBT;
    if (inCharBT == '\n') {
      Serial.print("recv: " + dataDariHP);
      dataKeHP = "replied:" + dataDariHP;
      SerialBT.println( dataKeHP);
      dataDariHP = "";
    }
  }
}
void Switch_WiFi(bool state) {
  switch (state) {
    case 0:
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);

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
