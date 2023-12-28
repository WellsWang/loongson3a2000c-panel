# loongson3a2000c-panel
长城龙芯3A2000C开关机控制面板

## 文件清单

* SCH_PanelBoard.pdf 电路图
* PCB_PanelBoard.pdf PCB布线
* LoongsonPanel.ino 固件程序，使用arduino IDE编程
* panelAgent.py 服务器端Agent程序
* settings.py 服务器端配置文件

## 工作原理

### 连接

1. 控制面板需要连接到主板的内置板载USB端口（可以通过4针线连接）：PCB上的USB2接口连接到主板靠近SATA0接口的FRONT USB接口；
2. 同时，PCB上的电源开关和硬盘读写指示灯的接口也需要连接主板：PCB的MAIN BRD CONN连接到主板对应的FRONT PANEL接口

### 原理

主机端的panelAgent.py开机自动执行，将主机信息通过USB转串口和ESP8266通信，ESP8266解析后显示在OLED屏幕上。

### 未来拓展

可以利用ESP8266 Wifi的特性，做成远程连接开关设备，并获取设备的信息。PCB上的OUTPUT端口为预留的未来可进行IO拓展的端口。

### 许可证

基于MIT许可证，不承担任何风险，不提供任何保障。
