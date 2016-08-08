Qoosky_Arduino_ESP8266_AT_SDK
==================
本ページは [Qoosky Cloud Controller](https://www.qoosky.io/help/api) を Arduino から利用するための情報をまとめたものです。

### 必要なもの
『[電子工作でインターネットに無線接続して通信を行う (Arduino)](https://www.qoosky.io/techs/14bfffdc82)』の回路図をもとに、ブレッドボード上で組まれた以下のパーツをご準備ください。

- Arduino (UNO など)
- ESP-WROOM-02
- 10k 抵抗 x3


ライブラリのインストール
==================
[zip 形式で github からダウンロードした Qoosky_Arduino_ESP8266_AT_SDK](https://github.com/qoosky/Qoosky_Arduino_ESP8266_AT_SDK/archive/master.zip) を Arduino IDE のメニューから追加してご使用ください。ライブラリ追加方法に関する公式ページは[こちら](https://www.arduino.cc/en/Guide/Libraries#toc4)です。


サンプルスケッチについて
==================
WiFi 接続が正常に行えることを確認するためには [ConnectWiFi.ino](https://github.com/qoosky/Qoosky_Arduino_ESP8266_AT_SDK/blob/master/examples/ConnectWiFi.ino) をご利用ください。正常に接続できることが確認できたら、次はいよいよ [QooskyCloudController.ino](https://github.com/qoosky/Qoosky_Arduino_ESP8266_AT_SDK/blob/master/examples/QooskyCloudController.ino) で Arduino をスマホや PC から遠隔操作してみましょう。

[ゲストモードで Qoosky Cloud Controller を起動](https://www.qoosky.io/account/api/cc)

コントローラで押したボタンが Arduino のシリアルモニタで確認できれば成功です。
