
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thông tin Wifi
const char* ssid = "Recognition Oviler";
const char* password = "*#159489@";

const char* mqtt_server = "iot.eclipse.org"; // Thông tin MQTT Broker Maker Hanoi đang deploy

// ClientID 
const char* clientID = "khanhneyiothutech";

//Thiết lập output xuất(public) và nhận(subscibe) dữ liệu với MQTT
const char* outTopic = "public/esp/outputkhanhney";
const char* inTopic = "public/esp/inputkhanhney";

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
const int threshold = 600;
// thiết lập ngưỡng do chúng ta tự setup
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int count = 0;

// Hàm kết nối Wifi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//  pinMode(D2, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
//  khai báo sử dụng D2 làm gốc test
}

// Hàm call back được gọi khi nhận được thông tin từ subcribing topic
void callback(char* topic, byte* payload, unsigned int length) {
  // xử lý dữ liệu nhận được
  /*
   * convert string to int giá trị cường độ sáng
   * if/else điều khiển bật tắt relay dựa trên ngưỡng
  */
  char* message = (char*) payload;
  int value = atoi(message);
//  hàm convert string sang number thôi ae
  Serial.println(value);
  if (value > threshold) {
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.println("HIGH");
      Serial.println(message);
  } else {
    digitalWrite(BUILTIN_LED, LOW); 
    Serial.println("LOW");
    Serial.println(message);
  }
}

// Reconnect đến MQTT Broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, clientID);
      // ... and resubscribe
      client.subscribe(inTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//vòng đời kiểm tra kết nối
void publishValue() {
  count++;
  dtostrf(count, 2, 2, msg);
  Serial.println(msg);
  dtostrf(analogRead(A0), 2, 2, msg);
  Serial.println(msg);
  client.publish(outTopic, msg);
  delay(2000);
//  nếu sử dụng public dữ liệu thì sau 2s sẽ gửi tín hiệu lên topic/server 1 lần
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
//  publishValue();
  client.loop();
}
//vòng lập chính của chương trình
