/*
  程序目的：
    1、批量化操作LED。
    2、无阻塞实现呼吸灯。
    一、硬件材料
    1*Arduino uno开发板
    10*LED
  二、电路接线
    Arduino UNO   的   GND   接   所有LED  的   负极
    Arduino UNO   的    2    接   1号LED   的   正极  
    Arduino UNO   的    3    接   2号LED   的   正极
    Arduino UNO   的    4    接   3号LED   的   正极
    Arduino UNO   的    5    接   4号LED   的   正极
    Arduino UNO   的    6    接   5号LED   的   正极
    Arduino UNO   的    7    接   6号LED   的   正极
    Arduino UNO   的    8    接   7号LED   的   正极
    Arduino UNO   的    9    接   8号LED   的   正极
    Arduino UNO   的    10   接   9号LED   的   正极
    Arduino UNO   的    11   接  10号LED   的   正极
    详见接线图
  三、程序上传
    第1步：打开Arduino软件，依次点击 -> 工具 -> 开发板选择"Arduino Mega"
    第2步：依次点击 -> 工具 -> 端口选择你自己电脑Arduino的COM口，一般是com3开始，如COM3，COM7
    第3步：点击上传（Arduino软件左上角对勾旁边的图标）
  四、程序原理
    采用非阻塞式实现呼吸灯
  五、预期效果
    1、支持多个LED同时呼吸灯
  六、可能的问题
    1、接线不对
    仔细核对接线图接线
  七、参考的资料
    File → Examples → 01.Basics → Fade
    或
    文件 → 示例 → 01.Basics → Fade
    Arduino教程
    https://space.bilibili.com/103589285/video?tid=0&page=4&keyword=&order=pubdate
    https://www.geek-workshop.com/thread-2719-1-1.html
    https://www.yiboard.com/forum-70-1.html
  八、特别注意
    1、因为采用正逻辑，高电平点亮，低电平熄灭。如果电路是负逻辑，需要改动相对应的代码。
    2、结合实际项目情况，选用合适的写法。一种是整体法，另一种是分块法。
*/

#define ledPin1     2   //1号LED
#define ledPin2     3   //2号LED
#define ledPin3     4   //3号LED
#define ledPin4     5   //4号LED
#define ledPin5     6   //5号LED
#define ledPin6     7   //6号LED
#define ledPin7     8   //7号LED
#define ledPin8     9   //8号LED
#define ledPin9     10  //9号LED
#define ledPin10    11  //10号LED
                        //所有LED存入数组以便于批量化操作
const int ledPins[13] = {ledPin1, ledPin2, 
                         ledPin3, ledPin4,
                         ledPin5, ledPin6, 
                         ledPin7, ledPin8,
                         ledPin9, ledPin10};                       

#define LEDON       HIGH//点亮LED灯
#define LEDOFF      LOW //熄灭LED灯

struct Led {
 long previousMillis;   //存储事件发生的最后时间。
 int  brightness = 0;   //定义整数型变量brightness与其初始值，此变量用来表示LED的亮度。
 bool flag;             //判断标志
};
Led led[2];             //新建2个led

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);         //初始化串口，波特率为9600
  initLEDs();                 //初始化所有LED
}

// the loop function runs over and over again forever
void loop() {
  //下面3个子函数采用整体的思维，也就是亮度计算和输出亮度是在同一个函数实现
  ledBreath(ledPin2, 1, 100); //led2以每100ms变化亮度1的速度呼吸
  ledBreath(ledPin4, 5, 10);  //led4以每10ms变化亮度5的速度呼吸
  ledBreath(ledPin5, 10, 200);//led5以每200ms变化亮度10的速度呼吸
  //下面2个if采用分块的思维，也就是亮度计算和输出亮度是2个维度
  if (ledBreath(ledPin8, 0, 1, 50) == true) {
    analogWrite(ledPin8 , led[0].brightness);
  }
  if (ledBreath(ledPin9, 1, 5, 100) == true) {
    analogWrite(ledPin9, led[1].brightness);
  }
}
/*********************************************************
 * 函数功能：LED初始化函数
 * 入口参数：无
 * 返回  值：无
*********************************************************/
void initLEDs() {
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);                //所有LED引脚设置为输出
    digitalWrite(ledPins[i], LEDOFF);           //所有LED熄灭
  }
}
/*********************************************************
 * 函数功能：LED呼吸灯效果，采用非阻塞式按键消抖，适用于多个LED
 * 入口参数：int LED引脚，int LED数组编号（0 1 2）、int 亮度变化亮、int 变化间隔
 * 返回  值：无
*********************************************************/
void ledBreath(int _ledPin, int _fadeAmount, int _period) {
  static unsigned long breathPeriod = millis(); //亮度变化的时间片刻
  static int brightness = 0;                    //亮度值
  if (millis() - breathPeriod >= _period) {     //是否已经过去了"_period" ms。
    breathPeriod = millis();                    //记录亮度变化的时间片刻
    brightness += _fadeAmount;                  //亮度值增加或减少
    if (brightness <= 0 || brightness >= 255) { //是否达到临界点
      _fadeAmount = -_fadeAmount;               //达到临界点，亮度变化量翻转
    }
    analogWrite(_ledPin, brightness);           //输出LED亮度
  }
}
/*********************************************************
 * 函数功能：LED呼吸灯效果，采用非阻塞式按键消抖，适用于多个LED
 * 入口参数：int LED数组编号（0 1 2）、int 亮度变化亮、int 变化间隔
 * 返回  值：无
*********************************************************/
bool ledBreath(int _ledPin, int _index, int _fadeAmount, int _period) { //_fadeAmount为每次led变化的亮度值，_period为led每次变化的间隔时间
  if (millis() - led[_index].previousMillis >= _period){                //是否已经过去了"_period" ms。
    led[_index].previousMillis = millis();                              //记录亮度变化的时间片刻
    led[_index].flag = true;                                            //将状态标志记录为true
    led[_index].brightness += _fadeAmount;                              //每次led的亮度变化_fadeAmount
    if (led[_index].brightness <= 0 || led[_index].brightness >= 255) { //达到临界点，亮度变化量翻转
      _fadeAmount = -_fadeAmount;
    }                                       
    Serial.print(_index + 1);
    Serial.print("号led的亮度为:");
    Serial.println(led[_index].brightness);                             //打印出led的亮度值
  }
  else {                                                                //如果未达到"_period" ms，那么就把状态记为false
    led[_index].flag = false;
  }
  return led[_index].flag;                                              //返回flag值
}
