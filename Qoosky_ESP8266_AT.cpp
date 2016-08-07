#include "Qoosky_ESP8266_AT.h"

Qoosky_ESP8266_AT::Qoosky_ESP8266_AT(uint32_t rxPin, uint32_t txPin, uint32_t baud) :
    m_rxPin(rxPin), m_txPin(txPin), m_webSocketStatus(false), m_apiToken("")
{
    SoftwareSerial *serial = new SoftwareSerial(rxPin, txPin);
    serial->begin(baud);
    m_serial = serial;
}

Qoosky_ESP8266_AT::Qoosky_ESP8266_AT(SoftwareSerial &serial) :
    m_rxPin(0), m_txPin(0), m_serial(&serial), m_webSocketStatus(false), m_apiToken("")
{
}

Qoosky_ESP8266_AT::Qoosky_ESP8266_AT(HardwareSerial &serial) :
    m_rxPin(0), m_txPin(0), m_serial(&serial), m_webSocketStatus(false), m_apiToken("")
{
}

Qoosky_ESP8266_AT::~Qoosky_ESP8266_AT() {
    disconnectAP();
    if(m_rxPin != 0 && m_txPin !=0) delete m_serial;
}

void Qoosky_ESP8266_AT::rxClear() {
    while(m_serial->available() > 0) m_serial->read();
}

bool Qoosky_ESP8266_AT::checkATResponse(String *buf, String target, uint32_t timeout) {
    *buf = "";
    char c;
    const unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_serial->available() > 0) {
            c = m_serial->read(); // 1 バイトずつ読み出します。
            if(c == '\0') continue;
            *buf += c;
        }
        if (buf->indexOf(target) != -1) return true;
    }
    return false;
}

bool Qoosky_ESP8266_AT::checkATResponse(String target, uint32_t timeout) {
    String buf;
    return checkATResponse(&buf, target, timeout);
}

bool Qoosky_ESP8266_AT::statusAT() {
    rxClear();
    m_serial->println("AT");
    return checkATResponse();
}

bool Qoosky_ESP8266_AT::restart() {
    rxClear();
    m_serial->println("AT+RST");
    if(!checkATResponse()) return false;
    delay(2000);
    const unsigned long start = millis();
    while(millis() - start < 3000) {
        if(statusAT()) {
            delay(1500);
            return true;
        }
        delay(100);
    }
    return false;
}

bool Qoosky_ESP8266_AT::connectAP(String ssid, String password) {
    rxClear();
    m_serial->println("AT+CWMODE_DEF=1"); // 1: station(client) mode, 2: softAP(server) mode, 3: 1&2
    if(!(checkATResponse() && restart())) return false; // "DEF"ault の cwMode を変更して再起動します。

    uint8_t retry = 5;
    do {
        // アクセスポイントに接続します。
        rxClear();
        delay(500);
        m_serial->print("AT+CWJAP_DEF=\"");
        m_serial->print(ssid);
        m_serial->print("\",\"");
        m_serial->print(password);
        m_serial->println("\"");
        if(checkATResponse("OK", 10000)) return true;
    } while(--retry);
    return false;
}

bool Qoosky_ESP8266_AT::disconnectAP() {
    rxClear();
    m_serial->println("AT+CWQAP");
    return checkATResponse();
}

uint8_t Qoosky_ESP8266_AT::ipStatus() {
    String buf;
    rxClear();
    m_serial->println("AT+CIPSTATUS");
    checkATResponse(&buf, "S:");
    uint32_t index = buf.indexOf(":");
    return buf.substring(index + 1, index + 2).toInt();
}

bool Qoosky_ESP8266_AT::statusWiFi() {
    uint8_t checkCnt = 5;
    do {
        if(ipStatus() == 5) return false;
        delay(100);
    } while(--checkCnt);
    return true;
}

bool Qoosky_ESP8266_AT::connectedTcp() {
    uint8_t retry = 5;
    do {
        if(ipStatus() == 3) return true;
        delay(100);
    } while(--retry);
    return false;
}

bool Qoosky_ESP8266_AT::disconnectTcp() {
    rxClear();
    m_serial->println("AT+CIPCLOSE");
    return checkATResponse();
}

bool Qoosky_ESP8266_AT::connectTcp(String host, uint32_t port) {
    if(connectedTcp()) disconnectTcp();
    String buf;
    uint8_t retry = 10;
    do {
        rxClear();
        m_serial->print("AT+CIPSTART=\"TCP\",\"");
        m_serial->print(host);
        m_serial->print("\",");
        m_serial->println(port);
        checkATResponse(&buf);
        if(buf.indexOf("OK") != -1 || buf.indexOf("ALREADY") != -1) {
            return true;
        }
        delay(100);
    } while(--retry);
    return false;
}

bool Qoosky_ESP8266_AT::connectQoosky(String apiToken) {
    if(m_webSocketStatus) return true; // 既に正常な接続が存在する場合は何もせず終了します。
    m_apiToken = apiToken; // API トークンのキャッシュを更新します。

    // (既に TCP 接続があれば切断して) TCP 接続を確立します。
    connectTcp("api.qoosky.io", 80);

    // RFC 6455 に準じた WebSocket handshake
    // https://tools.ietf.org/html/rfc6455
    const uint8_t nUpgradeRequest = 7;
    String upgradeRequest[] = {
        "GET /v1/controller/actuator/ws HTTP/1.1\r\n",
        "Host: api.qoosky.io\r\n",
        "Upgrade: websocket\r\n",
        "Connection: Upgrade\r\n",
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n", // RFC から引用した固定値
        "Sec-WebSocket-Version: 13\r\n",
        "User-Agent: Arduino ESP8266\r\n\r\n",
    };
    uint32_t len = 0;
    for(uint8_t i = 0; i < nUpgradeRequest; ++i) len += upgradeRequest[i].length();

    // データ送信をするための AT コマンド
    uint8_t retry = 15;
    do {
        String buf;
        rxClear();
        m_serial->print("AT+CIPSEND=");
        m_serial->println(len);
        if(checkATResponse(&buf, "> ")) break;
        if(!(--retry)) return false; // タイムアウトエラー
    } while(true);

    // データ送信
    uint32_t sentLen = 0;
    for(uint8_t i = 0; i < nUpgradeRequest; ++i) {
        for(uint32_t j = 0; j < upgradeRequest[i].length(); ++j) {
            if(++sentLen % 64 == 0) delay(20); // シリアルバッファが 64 バイトの Arduino が存在することを考慮 (空になるまで少し待ちます)
            m_serial->write(upgradeRequest[i][j]);
        }
    }

    // シリアルバッファオーバーフローを起こす前に変数に格納します。
    unsigned long start = millis();
    String response = "";
    uint32_t lenLimit = 64;
    while (millis() - start < 1000) {
        if(m_serial->available() > 0) {
            response += (char)m_serial->read();
            if(--lenLimit == 0) break;
        }
    }

    // ノイズの影響で "HTTP/1.1 101 Switching Protocols" が完全に受信できないことがあるため、
    // リクエストを送信できていれば (SEND OK) よいとします。
    if(response.indexOf("OK") == -1) return false;

    // API トークンを送信して認証します。JSON 文字列を用意します。
    String json = "{\"token\":\"" + apiToken + "\"}";
    if(json.length() > 125) return false; // 不正な API トークンです。

    // WebSocket フレームを作ります。
    // https://tools.ietf.org/html/rfc6455#section-5.2
    String frame = "";
    frame += (char)(0x81); // フラグメント化していないテキストメッセージであることを表現します。
    frame += (char)(0x80 + json.length()); // クライアントからのフレームは常にマスクされている必要があります。ペイロード長は 125 byte 以下のみサポート。
    frame += (char)random(0xff); // Arduino の char 型の範囲でマスク時のキーを乱数から生成します。
    frame += (char)random(0xff);
    frame += (char)random(0xff);
    frame += (char)random(0xff);
    // RFC より引用
    //> Octet i of the transformed data ("transformed-octet-i") is the XOR of
    //> octet i of the original data ("original-octet-i") with octet at index
    //> i modulo 4 of the masking key ("masking-key-octet-j"):
    //> j = i MOD 4
    //> transformed-octet-i = original-octet-i XOR masking-key-octet-j
    for(uint32_t i = 0; i < json.length(); ++i) frame += (char)(json[i] ^ frame[i % 4 + 2]);
    len = 6 + json.length();

    // データ送信をするための AT コマンド
    retry = 15;
    do {
        String buf;
        rxClear();
        m_serial->print("AT+CIPSEND=");
        m_serial->println(len);
        if(checkATResponse(&buf, "> ")) break;
        if(!(--retry)) return false;
    } while(true);

    // データを送信します。
    sentLen = 0;
    for(uint8_t i = 0; i < len; ++i) {
        if(++sentLen % 64 == 0) delay(20);
        m_serial->write(frame[i]);
    }

    // レスポンスをバッファリングします。
    start = millis();
    response = "";
    lenLimit = 256;
    while (millis() - start < 3000) {
        if(m_serial->available() > 0) {
            response += (char)m_serial->read();
            if(--lenLimit == 0) break;
        }
    }
    if(response.indexOf("suc") == -1) return false; // Autheotication success.
    return (m_webSocketStatus = true);
}

bool Qoosky_ESP8266_AT::sendMessage(const String& msg) {
    uint32_t len = msg.length();
    if(len > 125) return false; // 本ライブラリでは 125 文字までの送信をサポートします。
    if(!m_webSocketStatus && !connectQoosky(m_apiToken)) return false;
    return true; // TODO
}

int Qoosky_ESP8266_AT::popPushedKey() {
    if(!m_webSocketStatus && !connectQoosky(m_apiToken)) return false;
    return 1; // TODO (skip 2byte)
}
