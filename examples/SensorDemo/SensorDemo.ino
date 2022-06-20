/*
  程序目的：
    1、单个传感器先进先出滤波。
    2、传感器批量化先进先出滤波。
    3、如温度、湿度、光照度、距离值、气压值、重量值，都可以采用以下滤波思路。
    一、硬件材料
    1*Arduino uno开发板
    1*电位器
  二、电路接线
    Arduino UNO   的   GND   接   传感器  的   GND
    Arduino UNO   的   5V    接   传感器  的   VCC
    Arduino UNO   的   A0    接   传感器  的   信号端
    详见接线图
  三、程序上传
    第1步：打开Arduino软件，依次点击 -> 工具 -> 开发板选择"Arduino Mega"
    第2步：依次点击 -> 工具 -> 端口选择你自己电脑Arduino的COM口，一般是com3开始，如COM3，COM7
    第3步：点击上传（Arduino软件左上角对勾旁边的图标）
  四、程序原理
    1、首先是有效值滤波：目的是去掉不合理的原始值。假设一个传感器有效值是100-300，如果读出来是小于100或者大于300，那么显然是有问题的，因此要直接舍弃。
    2、然后是前后两次数值的变化幅度滤波：一般来说在100ms内，传感器的值不会发生显著变化。如果连续多次（如3次）则可以以当前读取的值作为新的值。
    3、最后是先进先出的原则，进行取平均滤波。一般取5到10次。
  五、预期效果
    1、支持单个或多个传感器滤波
  六、可能的问题
    1、接线不对
    仔细核对接线图接线
  七、参考的资料
    传感器十大滤波方法https://www.geek-workshop.com/thread-7694-1-1.html
    Arduino教程
    https://space.bilibili.com/103589285/video?tid=0&page=4&keyword=&order=pubdate
    https://www.geek-workshop.com/thread-2719-1-1.html
    https://www.yiboard.com/forum-70-1.html
  八、特别注意
    1、本滤波方法没有采用递推平均滤波，也就是没有for，因此效率更高。
*/
#define sensorPin        A0           //传感器引脚

struct SmoothSensor {                 //建立SmoothSensor结构体
  float Now = 0;                      //当前值
  float Old = 0;                      //上一次的值
  float Delta = 0;                    //当前值和上一次的值之差(Now - Old)
  float Max = 650;                    //最大有效值/合理值
  float Min = -210;                   //最小有效值/合理值
  int Count = 200;                    //传感器初始化时采样次数
  float FilterBuf[10] = {0};          //滤波数组，长度越大，数据越平缓，灵敏度也越低
  float FilterBufSum = 0;             //滤波数组求和
  float RangeOfChange = 20;           //前后两次的变化幅度，防止非正常的数据突变
  int ExceedingTimes = 3;             //连续超过变化限定幅度RangeOfChange的次数
  unsigned long UpdateTime = 0;       //记录每一次的更新实际
  boolean UpdateState = false;        //更新状态
};
SmoothSensor sensor, sensors[2];      //电位器、温度、湿度、光照度、距离值、气压值、重量值等传感器均适用

void setup() {
  Serial.begin(9600);                                     //初始化硬件串口
  initSmoothSensor(random(0, 100));                       //初始化传感器
  initSmoothSensor(0, random(0, 100));                    //初始化1号传感器
  initSmoothSensor(1, random(0, 100));                    //初始化1号传感器
  Serial.println("System Ready!");                        //串口打印"System Ready!"
}

void loop() {
  getSmoothSensor(random(0, 100), 100);                  //每100ms更新一次传感器的滤波值
  getSmoothSensor(0, random(0, 100), 100);               //1号传感器每100ms更新一次传感器的滤波值
  getSmoothSensor(1, random(0, 100), 200);               //2号传感器每200ms更新一次传感器的滤波值
}
/*********************************************************
   函数功能：初始化传感器，自适应每次开机的环境状态
   入口参数：float 传感器滤波前的值
   返回  值：无
*********************************************************/
void initSmoothSensor(float _RawValue) {
  for (int i = 0; i < sensor.Count; i++) {          //循环采用环境的实时值
    sensor.Now = _RawValue;                         //获取传感器的原始值
    if (sensor.Now > sensor.Max) {                  //最大有效值滤波，也就是数值如果超过有效值，说明数据是不可靠的
      sensor.Now = sensor.Old;                      //数值则采用上一次的值
    }
    else if (sensor.Now < sensor.Min) {             //最小有效值滤波，也就是数值如果低于有效值，说明数据是不可靠的
      sensor.Now = sensor.Old;                      //数值则采用上一次的值
    }
    else {
      sensor.FilterBufSum += sensor.Now;            //对环境的实时值做累加和
      sensor.Old = sensor.Now;                      //上一次的值更新为当前值
    }
  }
  sensor.Now = sensor.FilterBufSum / sensor.Count;  //计算环境的实时值的平均值作为开机时的传感器值
  sensor.FilterBufSum = 0;                          //累加和变量清零
  sensor.Old = sensor.Now;                          //上一次的值更新为当前值
  for (int i = 0; i < sizeof(sensor.FilterBuf) / sizeof(sensor.FilterBuf[0]); i++) {
    sensor.FilterBuf[i] = sensor.Now;               //滤波数组的初始值填充为当前值
    sensor.FilterBufSum += sensor.FilterBuf[i];     //滤波数组求累加和，目的是使得getSmoothSensor函数平均值滤波不因为前面计算拉低计算值而出错。
  }
}
/*********************************************************
   函数功能：获取传感器滤波后的值，使得数据更可靠
   入口参数：int 传感器滤波前的值，long 滤波的采样时间
   返回  值：无
*********************************************************/
void getSmoothSensor(float _RawValue, unsigned long _SampleTime) {
  unsigned long tempTime = millis();                //读取当前时间
  if (tempTime - sensor.UpdateTime >= _SampleTime) {//是否已经过去了"_SampleTime" ms
    sensor.UpdateTime = tempTime;                   //记录传感器数值更新的时间片刻
    sensor.Now = _RawValue;                         //获取传感器的原始值
    if (sensor.Now > sensor.Max) {                  //最大有效值滤波，也就是数值如果超过有效值，说明数据是不可靠的
      sensor.Now = sensor.Old;                      //数值则采用上一次的值
    }
    else if (sensor.Now < sensor.Min) {             //最小有效值滤波，也就是数值如果低于有效值，说明数据是不可靠的
      sensor.Now = sensor.Old;                      //数值则采用上一次的值
    }
    else {
      if (abs(sensor.Now - sensor.Old) > sensor.RangeOfChange) {//限幅滤波（过滤突变）
        static int i = 0;
        if (i++ < sensor.ExceedingTimes) {          //实际连续次数是否成功限定次数
          sensor.Now = sensor.Old;                  //如果否，说明传感器可能只是噪声数据（突变），并不是是真的发生了较大的改变
        }
        else {
          i = 0;                                    //如果是，实际连续次数清零，说明传感器是真的发生了较大的改变
          sensor.Old = sensor.Now;                  //数值则采用上一次的值
        }
      }
      else { //先进先出平均滤波法（比滑动平均滤波法节省for的时间）
        static int i = 0;
        float temp = sensor.FilterBuf[i];           //把当前下标的传感器值赋给temp变量
        sensor.FilterBuf[i] = sensor.Now;           //更新当前下标的传感器值为当前值
        sensor.FilterBufSum += (sensor.FilterBuf[i] - temp);//累加和变量增加（减少）当前值和原有值的差值
        if (++i >= sizeof(sensor.FilterBuf) / sizeof(sensor.FilterBuf[0])) {
          i = 0; //如果达到滤波数组长度，则从0开始。也就是实现先进先出
        }
        sensor.Now = sensor.FilterBufSum / (sizeof(sensor.FilterBuf) / sizeof(sensor.FilterBuf[0]));//计算出滤波后的值
        sensor.Old = sensor.Now;                    //上一次的值更新为当前值
      }
    }
    Serial.print("Sensor value is ");
    Serial.println(sensor.Now);                     //打印滤波后的传感器值
  }
}
/*********************************************************
   函数功能：初始化传感器，自适应每次开机的环境状态
   入口参数：int 传感器编号，int 传感器滤波前的值
   返回  值：无
*********************************************************/
void initSmoothSensor(int _index, float _RawValue) {
  for (int i = 0; i < sensors[_index].Count; i++) {           //循环采用环境的实时值
    sensors[_index].Now = _RawValue;                          //获取传感器的原始值
    if (sensors[_index].Now > sensors[_index].Max) {          //最大有效值滤波，也就是数值如果超过有效值，说明数据是不可靠的
      sensors[_index].Now = sensors[_index].Old;              //数值则采用上一次的值
    }
    else if (sensors[_index].Now < sensors[_index].Min) {     //最小有效值滤波，也就是数值如果低于有效值，说明数据是不可靠的
      sensors[_index].Now = sensors[_index].Old;              //数值则采用上一次的值
    }
    else {
      sensors[_index].FilterBufSum += sensors[_index].Now;    //对环境的实时值做累加和
      sensors[_index].Old = sensors[_index].Now;              //上一次的值更新为当前值
    }
  }
  sensors[_index].Now = sensors[_index].FilterBufSum / sensors[_index].Count;//计算环境的实时值的平均值作为开机时的传感器值
  sensors[_index].FilterBufSum = 0;                           //累加和变量清零
  sensors[_index].Old = sensors[_index].Now;                  //上一次的值更新为当前值
  for (int i = 0; i < sizeof(sensors[_index].FilterBuf) / sizeof(sensors[_index].FilterBuf[0]); i++) {
    sensors[_index].FilterBuf[i] = sensors[_index].Now;       //滤波数组的初始值填充为当前值
    sensors[_index].FilterBufSum += sensors[_index].FilterBuf[i];//滤波数组求累加和，目的是使得getSmoothSensor函数平均值滤波不因为前面计算拉低计算值而出错。
  }
}
/*********************************************************
   函数功能：获取传感器滤波后的值，使得数据更可靠
   入口参数：int 传感器编号，float 传感器滤波前的值，unsigned long 滤波的采样时间
   返回  值：无
*********************************************************/
void getSmoothSensor(int _index, float _RawValue, unsigned long _SampleTime) {
  unsigned long tempTime = millis();                          //读取当前时间
  if (tempTime - sensors[_index].UpdateTime >= _SampleTime) { //是否已经过去了"_SampleTime" ms
    sensors[_index].UpdateTime = tempTime;                    //记录传感器数值更新的时间片刻
    sensors[_index].Now = _RawValue;                          //获取传感器的原始值
    if (sensors[_index].Now > sensors[_index].Max) {          //最大有效值滤波，也就是数值如果超过有效值，说明数据是不可靠的
      sensors[_index].Now = sensors[_index].Old;              //数值则采用上一次的值
    }
    else if (sensors[_index].Now < sensors[_index].Min) {     //最小有效值滤波，也就是数值如果低于有效值，说明数据是不可靠的
      sensors[_index].Now = sensors[_index].Old;              //数值则采用上一次的值
    }
    else {
      if (abs(sensors[_index].Now - sensors[_index].Old) > sensors[_index].RangeOfChange) {//限幅滤波（过滤突变）
        static int i = 0;
        if (i++ < sensors[_index].ExceedingTimes) {           //实际连续次数是否成功限定次数
          sensors[_index].Now = sensors[_index].Old;          //如果否，说明传感器可能只是噪声数据（突变），并不是是真的发生了较大的改变
        }
        else {
          i = 0;                                              //如果是，实际连续次数清零，说明传感器是真的发生了较大的改变
          sensors[_index].Old = sensors[_index].Now;          //数值则采用上一次的值
        }
      }
      else { //先进先出平均滤波法（比滑动平均滤波法节省for的时间）
        static int i = 0;
        float temp = sensors[_index].FilterBuf[i];            //把当前下标的传感器值赋给temp变量
        sensors[_index].FilterBuf[i] = sensors[_index].Now;   //更新当前下标的传感器值为当前值
        sensors[_index].FilterBufSum += (sensors[_index].FilterBuf[i] - temp);//累加和变量增加（减少）当前值和原有值的差值
        if (++i >= sizeof(sensors[_index].FilterBuf) / sizeof(sensors[_index].FilterBuf[0])) {
          i = 0; //如果达到滤波数组长度，则从0开始。也就是实现先进先出
        }
        sensors[_index].Now = sensors[_index].FilterBufSum / (sizeof(sensors[_index].FilterBuf) / sizeof(sensors[_index].FilterBuf[0]));//计算出滤波后的值
        sensors[_index].Old = sensors[_index].Now;            //上一次的值更新为当前值
      }
    }
    Serial.print("Sensor ");
    Serial.print(_index);                                     //传感器编号
    Serial.print(" value is ");
    Serial.println(sensors[_index].Now);                      //打印滤波后的传感器值
  }
}
