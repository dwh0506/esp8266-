
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED D4


const char* ssid = "HUAWEI-3ACVUZ";
const char* password = "GMR13720935401";
const char* mqtt_server = "www.dwhwulian.top";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int pirPin = A0;
int val;
int count=0;
bool WIFI_Status=true;
void setup() {
  pinMode(pirPin, INPUT);
  pinMode(LED,OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
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
        
//    delay(500);
//    Serial.print(".");
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

void reconnect() {
   val=analogRead(pirPin);
   delay(100);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
       if(!val){
      client.publish("dwh004/people", "no");
       }else{
        client.publish("dwh004/people", "have");
        }
  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
  digitalWrite(LED, HIGH);
  val=analogRead(pirPin);
      Serial.print("PIR value: ");
     Serial.print(val);
     Serial.print('\n');
     delay(100);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    if(val<500){
       digitalWrite(LED,HIGH);
      snprintf (msg, 75,  "no");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/dwh004/people", msg);
   
    delay(1000);
    }else {
       digitalWrite(LED,LOW);
      snprintf (msg, 75,  "have");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/dwh004/people", msg);
   
    delay(1000);
      }
    
  }
}
