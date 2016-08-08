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

    // SSID と PASSWORD でアクセスポイントに接続します。
    // シリアル接続確認と WiFi 接続確認を行い、設定不備があれば出力します。
    while(true) {
        if(qoosky.statusAT()) { Serial.println("AT status OK"); break; }
        else Serial.println("AT status NOT OK");
        delay(1000);
    }
    while(true) { // 書き換えてください↓
        if(qoosky.connectAP("SSID", "PASSWORD")) { Serial.println("Successfully connected to an AP"); break; }
        else Serial.println("Failed to connected to an AP. retrying...");
        delay(1000);
    }
    while(true) {
        if(qoosky.statusWiFi()) { Serial.println("WiFi status OK"); break; }
        else Serial.println("WiFi status NOT OK");
        delay(1000);
    }

    // Qoosky に接続します。
    while(true) { // 書き換えてください↓
        if(qoosky.connectQoosky("XXXX-XXXX-XXXX-XXXX")) { Serial.println("Successfully connected to Qoosky."); break; }
        else Serial.println("Failed to connect to Qoosky, retrying...");
        delay(1000);
    }
}

void loop() {
    // 押されたボタン (key) を取得します。
    int key = qoosky.popPushedKey();

    if(key) {
        // PC のシリアルモニタに出力します。
        Serial.println("Received: " + String(key));
    }
    else {
        Serial.println("No pushed key.");
    }

    // 本サンプルでは 0.1 秒毎に受信した key を確認し続けます。
    delay(100);
}
