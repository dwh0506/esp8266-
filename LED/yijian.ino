
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //使用ArduinoJson库
#define LED D4
const char* ssid = "HUAWEI-3ACVUZ";
const char* password = "GMR137209354";
const char* mqtt_server = "www.dwhwulian.top";
//const char* host = "api.seniverse.com";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int val = D4;
int val1;
int count=0;
bool WIFI_Status=true;
void setup() {
  pinMode(LED, OUTPUT);
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
    digitalWrite(LED, LOW); 
  }else if((char)payload[0] == '3'){
    weather();
    }
  else {
    digitalWrite(LED, HIGH);  
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      if (val == 0) {
        client.publish("/open", "{\"msg\":\"open\"}");
      }
      else {
        client.publish("/open", "{\"msg\":\"off\"}");
      }
      client.subscribe("/open/2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void weather(){
   //天气预报
   const char* host = "api.seniverse.com";
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/v3/weather/now.json?key=SN-XdVLX1rHoGmAM4&location=yuncheng&language=zh-Hans&unit=c";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);
  String weather_data;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    weather_data += line;
  }

  client.stop();
  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  String json_weather_data;
  int jsonIndex;

  for (int i = 0; i < weather_data.length(); i++) {
    if (weather_data[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  json_weather_data = weather_data.substring(jsonIndex);
  Serial.println();
  Serial.println("json_weather_data: ");
  Serial.println(json_weather_data);


  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 210;
  DynamicJsonBuffer jsonBuffer(capacity);

  const char* json = "{\"results\":[{\"location\":{\"id\":\"WQNZJRJNM3JV\",\"name\":\"运城\",\"country\":\"CN\",\"path\":\"运城,运城,山西,中国\",\"timezone\":\"Asia/Shanghai\",\"timezone_offset\":\"+08:00\"},\"now\":{\"text\":\"晴\",\"code\":\"0\",\"temperature\":\"13\"},\"last_update\":\"2020-04-12T10:19:00+08:00\"}]}";

  JsonObject& root = jsonBuffer.parseObject(json);

  JsonObject& results_0 = root["results"][0];

  JsonObject& results_0_location = results_0["location"];
  const char* results_0_location_id = results_0_location["id"]; // "WQNZJRJNM3JV"
  const char* results_0_location_name = results_0_location["name"]; // "运城"
  const char* results_0_location_country = results_0_location["country"]; // "CN"
  const char* results_0_location_path = results_0_location["path"]; // "运城,运城,山西,中国"
  const char* results_0_location_timezone = results_0_location["timezone"]; // "Asia/Shanghai"
  const char* results_0_location_timezone_offset = results_0_location["timezone_offset"]; // "+08:00"

  JsonObject& results_0_now = results_0["now"];
  const char* results_0_now_text = results_0_now["text"]; // "晴"
  const char* results_0_now_code = results_0_now["code"]; // "0"
  const char* results_0_now_temperature = results_0_now["temperature"]; // "13"

  const char* results_0_last_update = results_0["last_update"]; // "2020-04-12T10:19:00+08:00"
  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  Serial.print("运城今天天气：　");
  Serial.println(results_0_now_text);
  String change=String()+results_0_now_text;
   if(change=="晴"){
    digitalWrite(LED,LOW);
    Serial.print("开灯成功：　");
    }else{
        digitalWrite(LED,HIGH);
      Serial.print("光灯成功：　");
      }
  delay(1000);

  }
void loop() {
 //订阅信息部分
 
  if (!client.connected()) {
    reconnect();
  }
   client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
     val1 = digitalRead(val);
    lastMsg = now;
    if (val1== 0) {
      snprintf (msg, 75, "{\"msg\":\"open\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      Serial.println(val1);
      client.publish("/open", msg);
        client.subscribe("/open/2");
    }
    else {
      snprintf (msg, 75, "{\"msg\":\"off\"}");
      Serial.print("Publish message: ");
      Serial.println(msg);
      Serial.println(val1);
      client.publish("/open", msg);
   client.subscribe("/open/2");
    }
  }
  


 
}
