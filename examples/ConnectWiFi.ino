#include <Qoosky_ESP8266_AT.h>

// 三種類のコンストラクタから一つ選びます。

// 1. デジタル入出力ピンを二つ指定して Qoosky_ESP8266_AT クラス内部で
//    SoftwareSerial を生成するためのコンストラクタです。
const byte rxPin = 2; // ESP8266 の Tx ピンに接続してください。
const byte txPin = 3; // ESP8266 の Rx ピンに接続してください。
Qoosky_ESP8266_AT qoosky(rxPin, txPin);

// 2. SoftwareSerial を外部から渡すタイプのコンストラクタです。
//SoftwareSerial softwareSerial(rxPin, txPin);
//Qoosky_ESP8266_AT qoosky(softwareSerial);

// 3. HardwareSerial を外部から渡すタイプのコンストラクタです。
//Qoosky_ESP8266_AT qoosky(Serial);


void setup() {
    // シリアル通信の baudrate を設定します。

    // '1' の場合の設定です。
    // PC と通信する HardwareSerial の baudrate を設定します。
    // ESP-WROOM-02 の baudrate とは関係ありません。
    Serial.begin(9600);

    // '2' の場合の設定です。
    //Serial.begin(9600);
    //softwareSerial.begin(115200);

    // '3' の場合の設定です。
    //Serial.begin(115200);


    // ESP-WROOM-02 との接続確認を行います。
    while(true) {
        if(qoosky.statusAT()) {
            Serial.println("AT status OK");
            break;
        }
        else {
            Serial.println("AT status NOT OK");
        }
        delay(1000);
    }

    // SSID と PASSWORD でアクセスポイントに接続します。
    while(true) {
        if(qoosky.connectAP("SSID", "PASSWORD")) { // <- 書き換えてください。
            Serial.println("Successfully connected to an AP");
            break;
        }
        else {
            Serial.println("Failed to connected to an AP. retrying...");
        }
        delay(1000);
    }
}

void loop() {
    // 本サンプルでは特に通信はせず、WiFi の接続が切れていないかどうかを
    // 1 秒毎に確認して結果を出力しつづけます。
    if(qoosky.statusWiFi()) {
        Serial.println("WiFi status OK");
    }
    else {
        Serial.println("WiFi status NOT OK");
    }
    delay(1000);
}
