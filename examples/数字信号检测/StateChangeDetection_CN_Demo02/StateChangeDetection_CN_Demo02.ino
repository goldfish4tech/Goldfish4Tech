/**
   传感器数字信号状态改变检测，状态改变，仅打印一次（高低电平切换）

   代码用处：适用于检测高低电平传感器
    开发板：适用于Arduino系列开发板（如：NANO、UNO、Mega 2560、ESP32、ESP8266等）
   注意事项：输出0 为 低电平，输出1 为 高电平
   流程图链接：
   【金山文档】 StateChangeDetection_CN_Demo02
    https://kdocs.cn/l/cvP4z0tXwuuj
*/

#define sensorPin        2        // 传感器引脚
#define runIntervalTime  20       // 运行检测间隔时间
unsigned long runTime    = 0;     // 阶段运行记录时间

bool lastTimeState = LOW;         // 上次状态值  默认为您当前初始化传感器的状态

void setup() {
  Serial.begin(9600);             // 设置波特率
  pinMode(sensorPin, INPUT);      // 设置引脚输入
  Serial.println(F("Start successfully"));          // 打印提示
}

void loop() {
  // 运行
  if (millis() - runTime >= runIntervalTime) {      // 当前时间如果减去阶段记录时间大于并等于间隔时间
    runTime = millis();                             // 记录阶段时间
    if (digitalRead(sensorPin) != lastTimeState) {  // 判断当前状态是否不等于上次的值
      lastTimeState = !lastTimeState;               // 状态反转
      Serial.print("The current status is: ");      // 输出打印
      Serial.println(lastTimeState);                // 输出打印 状态
    } 
  }

  // loop回括号
}
