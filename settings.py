DEVICEPORT  = '/dev/ttyUSB0'  # USB端口，Linux下运行程序的用户需要由读写此端口的权限
BAUDRATE    = 115200          # 串口速率，需要和面板固件中设置一致
TIMEOUT     = 1               # 超时时间
NETDEV      = 'ens33'         # 网卡设备号，需要和操作系统中认到的一致，长城的这块板子在debian11下默认是这个
LOGFILE     = 'msg.log'       # 日志文件句柄，临时用的这个，可以是由其它程序提供的文件句柄，文本内容在操作面板上可以直接显示，上下翻页观看
TPP         = 84              # Text Per Page，显示日志时每页的文字数量，建议使用84，否则UI会乱
