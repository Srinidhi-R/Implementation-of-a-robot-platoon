#define TRIG_PIN 16
#define ECHO_PIN 17

#define M_1A 22
#define M_1B 23
#define M_2A 21
#define M_2B 19

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define STOP_DIS 0.1


#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;



hw_timer_t *timerE = NULL;
long g_f64_duration = 0;
float g_f32_distance = 0;
unsigned int g_u16_turn_count = 0, g_u16_drn = 0, g_u16_interrupt_countE = 0, g_u16_countE = 10, g_u16_stop = 0;
bool stateE = 0, stateOB =0;
unsigned long int prevMillis = 0, writeD = 0;
unsigned int start = 0;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.println();
  start = 1;
  g_u16_drn = myData.b;
}

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
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(M_1A, OUTPUT);
  pinMode(M_1B, OUTPUT);
  pinMode(M_2A, OUTPUT);
  pinMode(M_2B, OUTPUT);
  digitalWrite(M_1A, LOW);
  digitalWrite(M_1B, LOW);
  digitalWrite(M_2A, LOW);
  digitalWrite(M_2B, LOW);
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
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

}
 
void loop() {
      if(g_f32_distance <= STOP_DIS)
      {
        digitalWrite(M_1A, LOW);
        digitalWrite(M_1B, LOW);
        digitalWrite(M_2A, LOW);
        digitalWrite(M_2B, LOW);
      }
      else
      {
      if(g_u16_drn == 1)
      {
        digitalWrite(M_1A, LOW);
        digitalWrite(M_1B, LOW);
        digitalWrite(M_2A, stateE);
        digitalWrite(M_2B, LOW);
      }
      else if(g_u16_drn == 2)
      {
        digitalWrite(M_1A, stateE);
        digitalWrite(M_1B, LOW);
        digitalWrite(M_2A, stateE);
        digitalWrite(M_2B, LOW);
      }
     else if(g_u16_drn == 3)
      {
      
        digitalWrite(M_1A, LOW);
        digitalWrite(M_1B, LOW);
        digitalWrite(M_2A, LOW);
        digitalWrite(M_2B, LOW);
      }
      }

  if(millis() - prevMillis > 500)
  {
    getDistance();
    if(g_f32_distance < 0.1)
      g_u16_stop = 1;
    Serial.print("\nDistance: ");
    Serial.print(g_f32_distance);
    Serial.print(" m");
    Serial.print("\nDirection: ");
    Serial.print(g_u16_drn);
    Serial.print("\nCount: ");
    Serial.print(g_u16_turn_count);
    Serial.print("\nState obstacle: ");
    Serial.print(stateOB);
    prevMillis = millis();
    if(g_u16_drn == 2)
       g_u16_turn_count++;
    
//    if(g_f32_distance <= STOP_DIS)
//  {
//    digitalWrite(M_1A, LOW);
//    digitalWrite(M_1B, LOW);
//    digitalWrite(M_2A, LOW);
//    digitalWrite(M_2B, LOW);
//  }
//  else if(g_u16_drn == 2)
//  {
//    
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
//      g_u16_turn_count = 0;
//    }  
//  }
//  else if(g_u16_drn == 3)
//  {
//    
//    
//    digitalWrite(M_1A, stateE);
//    digitalWrite(M_1B, LOW);
//    digitalWrite(M_2A, stateE);
//    digitalWrite(M_2B, LOW);
//  }  
  
  }
}
