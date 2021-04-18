#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED D4
// Update these with values suitable for your network.
#define ADpin A0
#define Sensor_DO D7
#define Sensor D6
const char* ssid = "HUAWEI-3ACVUZ";
const char* password = "GMR13720935401";
const char* mqtt_server = "www.dwhwulian.top";
WiFiClient espClient;
PubSubClient client(espClient);
int count=0;
bool WIFI_Status=true;
long lastMsg = 0;
char msg[50];
int value = 0;
unsigned int SensorValue = 0;
unsigned int DOValue=0;
unsigned int Ss=0;
int ADBuffer = 0;
void setup() {
   pinMode(LED, OUTPUT);
  pinMode(Sensor_DO,INPUT);
  pinMode(Sensor,INPUT);
   pinMode(ADpin, INPUT);//火焰输入引脚
  pinMode(BUILTIN_LED, OUTPUT);    
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //WiFi.begin(ssid, password);

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
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void reconnect() {
    ADBuffer = analogRead(ADpin);   //读AD值
    Ss= digitalRead(Sensor_DO);
   delay(100);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
       if(ADBuffer <1024){
      client.publish("/dwh003/flame", "have");
       }else if(Ss==0){
        client.publish("/dwh003/flame", "smoke");
        }else{
          client.publish("/dwh003/flame", "no");
          }
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
  ADBuffer = analogRead(ADpin);   //都AD值
  delay(100);
   Serial.print("PIR value: ");
     Serial.print(ADBuffer);
     Serial.print('\n');
//  SensorValue = analogRead(Sensor);    //读取Sensor引脚的模拟值，该值大小0-1023
//  DOValue = analogRead(Sensor_DO);
  Ss= digitalRead(Sensor_DO);
  delay(300);
     Serial.print(" Ss = ");
     Serial.println(Ss);  
     delay(100);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    if(ADBuffer <1024){
      snprintf (msg, 75,  "have");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/dwh003/flame", msg);
    delay(1000);
    }
   else if(Ss==0){
       snprintf (msg, 75,  "smoke");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/dwh003/flame", msg);
    delay(1000);
      }
      else{
      snprintf (msg, 75,  "no");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/dwh003/flame", msg);
    delay(1000);
      }
  }
}
