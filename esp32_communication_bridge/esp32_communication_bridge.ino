#define SERIAL_DEBUG

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
BluetoothSerial SerialBT;                // Object Bluetooth Serial
WebSocketServer webSocketServer;         // Object Web server

/* <-----------| Object Variable|-----------> */
int incomingByte;
char data_serial_rx[21];
char data_serial_tx[] = "1#2#+12.0#12500#0.35";

char inByteS2;
char inByteBT;
String dataDariHP;
String dataKeHP;
char inCharBT;
//bool detectNewLine=false;

String inString;
String strSerial2;
String strBT;
int8_t i = 0;
int8_t cnt=0;

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


  //  if (strSerial2.length() > 0) {
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
      if (radio_state == BT_on) {
//        cnt++;
//        Serial.print("head:");
//        Serial.print(strSerial2);
        SerialBT.print(strSerial2);
//        Serial.print("ke-" + strSerial2);
      }
//      Serial.print(strSerial2);
    }
    strSerial2 = "";
  }

  if (inByteBT == '\n') {
    inByteBT = '\0';
//    Serial.print(strBT);
    Serial2.print(strBT);
    strBT = "";
    
  }

  //  /* Baca String Serial */
  //  while (Serial2.available()) {
  //    char inByte = Serial2.read();
  //    inString += inByte;
  //  }
  //  /* Menerjemahkan Perintah */
  //  if (inString.length() > 0) {
  //#ifdef SERIAL_DEBUG
  //    Serial.println(inString);
  //#endif
  //    if (inString == "BTON\n") {
  //      Switch_Bluetooth(1);
  //      radio_state = BT_on;
  //    }
  //    else if (inString == "BTOFF\n") {
  //      Switch_Bluetooth(0);
  //      radio_state = BT_off;
  //    }
  //    else if (inString == "WIFION\n") {
  //      Switch_WiFi(1);
  //      radio_state = WiFi_on;
  //    }
  //    else if (inString == "WIFIOFF\n") {
  //      Switch_WiFi(0);
  //      radio_state = WiFi_off;
  //    }
  //
  //
  //    inString = "";
  //  }
  //  switch (radio_state) {
  //    case BT_on:
  //      if (SerialBT.available()) {
  //        inCharBT = SerialBT.read();
  //        dataDariHP += inCharBT;
  //        if (inCharBT == '\n') {
  //          Serial2.print(dataDariHP);
  //          Serial.print(dataDariHP);
  //          dataDariHP = "";
  //        }
  //      }
  //#ifdef SERIAL_DEBUG
  //      Serial.println("keHP" + inString);
  //#endif
  //      SerialBT.print(inString);
  //      break;
  //    case BT_off:
  //      break;
  //
  //    case WiFi_on:
  //      break;
  //    case WiFi_off:
  //      break;
  //  }



      switch (Serial.read()) {
        case 'a':
          Serial.print("kirim komplit\n");
          SerialBT.print("MWP\n");
          SerialBT.print("0#0#0#0#12.5#180\n");
          break;
  
        case 'b':
          Serial.print("kirimWMP\n");
          SerialBT.print("MWP\n");
//          Serial2.print("0#1#0#100#12.5#180#98\n");
          break;
  
        case 'c':
          Serial.print("kirim data\n");
//          Serial2.print("RP0\n");
           SerialBT.print("0#0#0#0#12.5#180\n");
          break;
  
      }
  //  Bluetooth_Stream();
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
    if (inCharBT == '\0') {
      Serial2.print(dataDariHP);
      Serial.print("recv: " + dataDariHP);
      //      dataKeHP = "replied:" + dataDariHP;
      //      SerialBT.println(dataKeHP);
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
