#ifndef QOOSKY_ESP8266_AT_H_
#define QOOSKY_ESP8266_AT_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

class Qoosky_ESP8266_AT
{
 private:
    // SoftwareSerial で使用する受信 (rx) と送信 (tx) ピンです。
    uint32_t m_rxPin;
    uint32_t m_txPin;

    // ESP8266 とシリアル通信するためのインターフェースです。
    Stream *m_serial; // SoftwareSerial と HardwareSerial が継承するベースクラスです。

 public:
    // 三種類のコンストラクタから好きなものを一つ選びます。
    // - デジタル入出力ピンを二つ指定して Qoosky_ESP8266_AT クラス内部で
    //   SoftwareSerial を作成するためのコンストラクタです。
    //   rxPin: 指定したピンを ESP8266 の Tx ピンに接続してください。
    //   txPin: 指定したピンを ESP8266 の Rx ピンに接続してください。
    Qoosky_ESP8266_AT(uint32_t rxPin, uint32_t txPin, uint32_t baud = 115200);
    // - SoftwareSerial または HardwareSerial を外部から渡すタイプのコンストラクタです。
    Qoosky_ESP8266_AT(SoftwareSerial &serial);
    Qoosky_ESP8266_AT(HardwareSerial &serial);

    // デストラクタ
    ~Qoosky_ESP8266_AT();

 public:
    // シリアル接続が正常であるかを確認します。
    bool statusAT();

    // WiFi 接続が正常であるかを確認します。
    bool statusWiFi();

    // WiFi アクセスポイントに接続または切断します。
    bool connectAP(String ssid, String password);
    bool disconnectAP();

 private:
    // シリアル受信バッファを空にします。
    void rxClear();

    // ESP8266 を再起動します。
    bool restart();

    // AT コマンドの実行結果が "OK" であったかどうかを確認します。
    bool checkATResponse(String target = "OK", uint32_t timeout = 1000);
    bool checkATResponse(String *buf, String target = "OK", uint32_t timeout = 1000); // コマンドの実行結果を変数 buf に格納します。

    // ESP8266 の IPSTATUS (WiFi 接続や TCP 通信の状態) を返します。
    //   2: ESP8266 はアクセスポイントに接続して IP を取得できている。
    //   3: ESP8266 は 2 の状態に加えて TCP または UDP 通信まで確立している。
    //   4: 確立した 3 の TCP または UDP 通信が切断された。2 の状態は維持している。
    //   5: ESP8266 はアクセスポイントに接続できていない。
    uint8_t ipStatus();

 public:
    // 事前に取得しておいた認証トークンで Qoosky に接続します。
    bool connectQoosky(String apiToken);

    // メッセージの送受信を行います。
    bool sendMessage(const String& msg); // Qoosky Cloud Controller の下部に表示されるメッセージを送信します。
    int popPushedKey(); // 0: 押されていない, 1-4: 右側ボタンの↑←→↓, 5-8: 左側ボタンの↑←→↓

 private:
    // TCP 通信を開始または終了します。
    bool connectTcp(String host, uint32_t port);
    bool disconnectTcp();
    bool connectedTcp(); // TCP コネクションが存在すれば true を返します。
};

#endif // #ifndef QOOSKY_ESP8266_AT_H_
