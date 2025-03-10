本系统的核心组成包括主控STM32、LCD1602、DS18B20、液位检测和WiFi。
ESP8266无线模块通过其内置的WiFi模块与手机的WiFi网络进行通信，实现饮水机的智能控制；DS18B20温度传感器获取当前温度，液晶显示模块初始化后展示当前信息和参数；如果饮水机内无水，则蜂鸣器会发出警报提醒，以防止干烧。
若有水，会判断是常温模式还是加热模式，并相应控制继电器进行出水。若为加热模式，会先判断温度是否低于下限，如果低于下限，则先加热再控制继电器出水。
