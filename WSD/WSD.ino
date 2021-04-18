#include <DHT.h>//包含DHT头文件
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define DHTPIN 5      //定义DHT11模块连接管脚io5
#define LED D4
#define DHTTYPE DHT11   // 使用温度湿度模块的类型为DHT11
const char* ssid = "HUAWEI-3ACVUZ";
const char* password = "GMR13720935401";
const char* mqtt_server = "www.dwhwulian.top";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int count=0;
bool WIFI_Status=true;
DHT dht(DHTPIN, DHTTYPE);    //生成DHT对象，参数是引脚和DHT的类型
void setup()//将设置代码放在此处，运行一次；
{
    //初始化端口
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    dht.begin();//初始化DHT传感器
    setup_wifi();
    client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    if(WIFI_Status){
      Serial.print(".");
      digitalWrite(LED,HIGH);
      delay(500);
      digitalWrite(LED,LOW);
      delay(500);
      count++;
      if(count>=5){
        WIFI_Status=false;
        Serial.println("wifi连接失败，请用手机配网");
        }
      }
      else{
        smartConfig();
        }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//一键配网函数
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    digitalWrite(LED, 0);
    delay(500);
    digitalWrite(LED, 1);
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      break;
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
    Serial.println((char)payload[5]);
  

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[5] == 'N') {
    digitalWrite(LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else if((char)payload[5] == 'F'){
    digitalWrite(LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
   float h = dht.readHumidity();//读取DHT11传感器的湿度 并赋值给h
    float t = dht.readTemperature();//读取传感器的温度   并赋值给t
   int h1=int(h);
   int t1=int(t);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("flameClient")) {
      Serial.println("connected");
      snprintf (msg, 75,  "{\"reported\":{\"Temperature\":%d,\"Humidity\":%d}}",t1,h1);
      client.publish("/WSD",  msg);
      client.subscribe("/WSD/1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//通过循环不断读取温湿度传感器获取的数据
void loop() //把主代码放在这里，重复运行：
{
     if (!client.connected()) {
    reconnect();
  }
  client.loop();
    float h = dht.readHumidity();//读取DHT11传感器的湿度 并赋值给h
    float t = dht.readTemperature();//读取传感器的温度   并赋值给t
   int h1=int(h);
   int t1=int(t);
    if (isnan(h) || isnan(t))//判断是否成功读取到温湿度数据
    {
        Serial.print("失败！");//读取温湿度失败！
    }
    else//成功读取到数据
    {   
       Serial.print("温度为：");
       Serial.print(t);
      Serial.print("湿度为：");
       Serial.print(h);
  
    }
    delay(500);//延时函数
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    snprintf (msg, 75,  "{\"reported\":{\"Temperature\":%d,\"Humidity\":%d}}",t1,h1);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/WSD", msg);
    client.subscribe("/WSD/1");
    delay(1000);
  }
}
