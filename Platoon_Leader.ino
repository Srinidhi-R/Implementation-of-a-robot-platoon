#define TRIG_PIN 16
#define ECHO_PIN 17

#define M_1A 25
#define M_1B 12
#define M_2A 26
#define M_2B 4


//25,12,26,4
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define STOP_DIS 0.1
#define TURN_DIS 0.3

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x8C,0xAA,0xB5, 0x94, 0x4B, 0xC0};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;
bool g_ub_turn = 0;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


hw_timer_t *timerE = NULL;
long g_f64_duration = 0;
float g_f32_distance = 0;
unsigned int g_u16_turn_count = 0, g_u16_drn = 0, g_u16_interrupt_countE = 0, g_u16_countE = 10;
bool stateE = 0, stateF = 0;
unsigned long int prevMillis = 0;
void IRAM_ATTR extTimer()        //100uS timer to generate extruder pulses
{

    if (g_u16_interrupt_countE < 9*g_u16_countE-1)
    {
      stateE=HIGH;
    }
    else if (g_u16_interrupt_countE >= 9*g_u16_countE-1 && g_u16_interrupt_countE < 10*g_u16_countE-1)
    {
      stateE=LOW;
    }
    else if (g_u16_interrupt_countE == 10*g_u16_countE-1)
    {
      g_u16_interrupt_countE = 0;
    }
    g_u16_interrupt_countE++; 


     
}
void getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    g_f64_duration = pulseIn(ECHO_PIN, HIGH);
    g_f32_distance = g_f64_duration * SOUND_SPEED/200;
    
}
void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(M_1A, OUTPUT);
  pinMode(M_1B, OUTPUT);
  pinMode(M_2A, OUTPUT);
  pinMode(M_2B, OUTPUT);
  timerE = timerBegin(1,80,true);               //80Mhz prescalar - 100us timer for extruder
  timerAttachInterrupt(timerE,&extTimer,true);
  timerAlarmWrite(timerE,100,true);
  timerAlarmEnable(timerE);
    // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  delay(10);
}

void loop() {
  myData.b = g_u16_drn;
  if(g_f32_distance <= STOP_DIS and g_ub_turn!=1)
    {
      g_u16_drn = 3; //stop
      digitalWrite(M_1A, LOW);
      digitalWrite(M_1B, LOW);
      digitalWrite(M_2A, LOW);
      digitalWrite(M_2B, LOW);
    }
    else if(g_f32_distance <= TURN_DIS or g_ub_turn == 1)
    {
      g_u16_drn = 1; //turn
      digitalWrite(M_1A, stateE);
      digitalWrite(M_1B, LOW);
      digitalWrite(M_2A, LOW);
      digitalWrite(M_2B, LOW);
      if(g_u16_turn_count <= 5)
        g_ub_turn = 1;
      else
      {
        g_ub_turn = 0;  
        g_u16_turn_count = 0;
      }  
        
    }
    else if(g_ub_turn != 1)
    {
      g_u16_drn = 2; //run
      digitalWrite(M_1A, stateE);
      digitalWrite(M_1B, LOW);
      digitalWrite(M_2A, HIGH);
      digitalWrite(M_2B, LOW); 
    }
  
  if(millis() - prevMillis > 200)
  {
    getDistance();
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    Serial.print("\nDistance: ");
    Serial.print(g_f32_distance);
    Serial.print(" m");
    Serial.print("\nDirection: ");
    Serial.print(g_u16_drn);
    Serial.print("\nCount: ");
    Serial.print(g_u16_turn_count);
    prevMillis = millis();
    if(g_u16_drn == 1)
       g_u16_turn_count++;
    
//    if(g_f32_distance <= STOP_DIS and g_u16_drn != 2)
//  {
//    g_u16_drn = 1; //stop
//    digitalWrite(M_1A, LOW);
//    digitalWrite(M_1B, LOW);
//    digitalWrite(M_2A, LOW);
//    digitalWrite(M_2B, LOW);
//  }
//  else if(g_f32_distance <= TURN_DIS or g_u16_drn == 2)
//  {
//    g_u16_drn = 2; //turn
//    if(g_u16_turn_count <= 1)
//    {
//      digitalWrite(M_1A, LOW);
//      digitalWrite(M_1B, LOW);
//      digitalWrite(M_2A, LOW);
//      digitalWrite(M_2B, LOW);
//    }
//    else if (g_u16_turn_count <= 3)
//    {
//      digitalWrite(M_1A, LOW);
//      digitalWrite(M_1B, stateE);
//      digitalWrite(M_2A, LOW);
//      digitalWrite(M_2B, stateE);
//    }
//    else if(g_u16_turn_count <= 4)
//    {
//      digitalWrite(M_1A, stateE);
//      digitalWrite(M_1B, LOW);
//      digitalWrite(M_2A, LOW);
//      digitalWrite(M_2B, LOW);
//    }
//    else
//    {
//      g_u16_drn = 0;
//      g_u16_turn_count = 0;
//    }  
//  }
//  else
//  {
//    
//    g_u16_drn = 3; //forward
//    digitalWrite(M_1A, stateE);
//    digitalWrite(M_1B, LOW);
//    digitalWrite(M_2A, stateE);
//    digitalWrite(M_2B, LOW);
//  }
  }
  
  
 

}
    
