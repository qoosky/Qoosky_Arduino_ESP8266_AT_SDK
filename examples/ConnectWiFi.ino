#include <Qoosky_ESP8266_AT.h>

// Arduino UNO の場合は、例えばデジタル入出力の 2 番, 3 番ピンを利用して
// ESP-WROOM-02 とシリアル通信するように設定します。
const byte rxPin = 2; // ESP8266 の Tx ピンに接続してください。
const byte txPin = 3; // ESP8266 の Rx ピンに接続してください。
Qoosky_ESP8266_AT qoosky(rxPin, txPin);

void setup() {
    // PC と通信する HardwareSerial の baudrate を設定します。
    // ESP-WROOM-02 の baudrate とは関係ありません。
     Serial.begin(9600);

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
