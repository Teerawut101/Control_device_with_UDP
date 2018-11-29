#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Ticker.h>     
WiFiUDP udp;

// Include ESP8266 library
//---------------------------------------------------------------------------------
const char* ssid = "Electronics";           // SSID is set
const char* password = "Electronics";   // Password is set
#define ALIAS  "room1" //กำหนดชื่ออุปกรณี่ต้องการ
unsigned int localPort = 8888;      // local port to listen for UDP packets
//---------------------------------------------------------------------------------

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
//------------------------------Status LED---------------------------
Ticker ST_LED;
Ticker ST_STOP;
char Flag_ST = 0x00;   //  RUN =1 STOP =0
//-------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);                   // Enable UART
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");         // Print title message to the serial monitor
  Serial.println(ssid);                   // Print SSID name
  WiFi.disconnect();
  WiFi.begin(ssid, password);            // Send password
  ST_begin(); // เริ่ม LED แสดงสะถานะการเชื่อมต่อ Wi-Fi
  while (WiFi.status() != WL_CONNECTED)   // Check WiFi status
  {
    delay(500);
    Serial.print(".");                    // Print dot for showing the progress status
  }
  Serial.println("");
  Serial.println("WiFi connected");       // Print the connected message
  Serial.println("IP address: ");         // Print IP address
  Serial.println(WiFi.localIP());
  udp.begin(localPort);  //กำหนดหมายเลข port ให้กับอุปกรณ์
  _RUN(); // LED แสดงสะถานะปกติ
}
void loop()
{
  if (WiFi.status() != WL_CONNECTED)  // Check WiFi status
  {
    delay(500);
    _STOP(); // LED แสดงสะถานะเชื่อมต่อ WiFi ไม่ได้
  } else {
    _RUN(); // LED แสดงสะถานะปกติ
  }
  int cb = udp.parsePacket(); 
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    udp.read(packetBuffer, NTP_PACKET_SIZE); // บันทึกข้อมูลลงในตัวแปร packetBuffer
    String _packet = String((char *)packetBuffer);  // เปลี่ยนชนิดตัวแปร packetBuffer ให้เป็นชนิด String
    _packet.trim(); // ตัดช่องว่างหัว-ท้ายข้อความออก
    Serial.print("msg packet: ");
    Serial.println(_packet); // แสดง ข้อความที่ส่งเข้ามา
    String _Alias = getValue(_packet, ',', 0); // แยกข้อความที่ส่งเข้ามาเอาเฉพาะชื่ออุปกรณ์
    String _msg = getValue(_packet, ',', 1); // แยกข้อความที่ส่งเข้ามาเอาเฉพาะข้อความ
    if (_Alias.equals(ALIAS)) { // ตรวจสอบชื่อข้อความที่ส่งเข้ามา
      _msg.trim(); // ตัดช่องว่างหัว-ท้ายข้อความออก
      Serial.print("_msg: ");
      int pin = _msg.toInt(); //เปรี่ยนข้อความ ให้เป็น ค่าตัวเลข
      Serial.println(pin);  // แสดงข้อความ
      pinMode(pin, OUTPUT); // กำหนดขาให้เป็น OUTPUT
      digitalWrite(pin, !digitalRead(pin)); // กำหนดสะถานะ โดยกลับสะถานะจากเดิม
    }
  }
}
String getValue(String data, char separator, int index) // ฟังก์ชันแยกข้อคาวม
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void _RUN (void) {
  if (Flag_ST != 1) {
    ST_LED.detach();
    Flag_ST = 1;
    ST_LED.attach(1, toggle_led);
  }

}
void _STOP(void) {
  if (Flag_ST != 0) {
    ST_LED.detach();
    Flag_ST = 0;
    ST_LED.attach(0.05, toggle_led);
  }
}
void toggle_led(void)
{
  //toggle state
  int state = digitalRead(2);  //
  digitalWrite(2, !state);     // set pin to the opposite state
}

void ST_begin(void) {
  pinMode(2, OUTPUT);
  pinMode(2, HIGH);
  ST_LED.attach(0.05, toggle_led);
  Flag_ST = 0;
}
