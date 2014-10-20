#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <SoftwareSerial.h>

// Bluetooth 연결할 시리얼 
SoftwareSerial BT_Serial(10,11); // RX, TX
MPU6050 accelgyro;

// 설정 값들.
const float WORKING_THRESHOLD = 0.3;

// PIN
const int LED_PIN = 13;
// 스텝 카우트를 초기화 버트
const int BT_RESET_STEPS = A0;
// 평균점 다시 계산 
const int BT_RESET_CALIBRATION = A1;

// 변수들
int16_t ax, ay, az;
int16_t gx, gy, gz;
float xavg, yavg, zavg;
int flag = 0;

// 스텝수 
int steps = 0;

boolean blinkState = false;

boolean is_working(double dist){
  return dist > WORKING_THRESHOLD;
}

// 가속도 센서 OFFSET 변경
// 영점 조정하는 과정
void adjust_accelgyro(){
    Serial.println("Updating internal sensor offsets...");
    accelgyro.setXAccelOffset(-950);
    accelgyro.setYAccelOffset(-5200);
    accelgyro.setZAccelOffset(-920);
    accelgyro.setXGyroOffset(20);
    accelgyro.setYGyroOffset(56);
    accelgyro.setZGyroOffset(-85);
}

void setup() {
    // i2C 초기화
    Wire.begin();

    // 시리얼 초기화
    Serial.begin(115200);
    
    // Bluetooth 연결 초기화
    BT_Serial.begin(9600);    
    // BT_Serial.print("AT+NAMEJINN");
    
    // 가속도센서 초기화
    accelgyro.initialize();
    
    // 자이로 250도/초, 가속도: -2.0G ~ 2.0G
    accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

    // 센서 OFFSET 초기화 
    adjust_accelgyro();

    // 아두이노 상태 LED 설정 
    pinMode(LED_PIN, OUTPUT);
    
    // 가속도센서의 중간값 구하기
    // 가속도변화를 판단하기 위해서 필요  
    calibrate();
    
    //= LED 초기화 
    for(int i=2;i<10;i++){ pinMode(i, OUTPUT); } 
    // 기본값: 0
    displayDigit(0);
    // 7-Segment의 `.' 끄기
    digitalWrite(9, HIGH);
    
    // 버튼들 초기화
    pinMode(BT_RESET_STEPS, INPUT_PULLUP);
    pinMode(BT_RESET_CALIBRATION, INPUT_PULLUP);    
}


void loop() {
    
    // check buttons
    // PULLUP 으로 설정 했기 때문에 버튼이 눌리면 LOW가 됨
    if( digitalRead(BT_RESET_STEPS) == LOW ){
      // 스텝 수를 리셋한다.
      flag = 0;
      steps = 0;
      // 것음 걸이의 1번째 수 출력 
      displayDigit(steps % 10);
      
      delay(500);
    }
    if( digitalRead(BT_RESET_CALIBRATION) == LOW ){
      flag = 0;
      calibrate();
      Serial.println("[info] recalibrated");
      delay(500);
    }
    
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // 중력가속도 단위로 변환 ( LBS --> G )
    double fax = ax / 16384.0;
    double fay = ay / 16384.0;
    double faz = az / 16384.0;
    
    // 중앙값에서 거리 값구하기.
    // 이 거리 값을 기준으로 걸었는지 판단 
    double dist = sqrt( pow(fax-xavg, 2) + pow(fay - yavg,2) + pow(faz - zavg, 2) );
    double major_factor = max(fax, max(fay, faz));
    
    // 디버깅 메시지 
    // Serial.print("dist = "); Serial.print(dist ); Serial.print("mfac = "); Serial.println(major_factor );
    
    //걸었는지 판단 
    if (is_working(dist) && flag == 0 && major_factor > 0) {
        flag = 1;
        // 걸을 때 마다 "on_step()" 호출 
        on_step();  
        delay(200);
    }
    
    if ( !is_working(dist)  && flag == 1) { 
      flag=0; 
    }
       

    // LED 블링크 
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    
    delay(20);
}


// 변화를 감지하기 위한 평균 값 찾기
// : xavg, yavg, zavg가 평균값
void calibrate() {
  
    float sumx=0;
    float sumy=0;
    float sumz=0;
    
    for (int i=0;i<100;i++) {
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        sumx += ax / 16384.0;
        sumy += ay / 16384.0;
        sumz += az / 16384.0;
    }
    
    xavg = sumx / 100.0;
    yavg = sumy / 100.0;
    zavg = sumz / 100.0;
}

// 걸을 때마다 호출 됨 
void on_step(){
    steps = steps+1;
    // 것음 걸이의 1번째 수 출력 
    displayDigit(steps % 10);
    
    // 시리얼에 출력
    Serial.print("step = ");
    Serial.println(steps);
    
    // Bluetooth에 출력 
    BT_Serial.print(",");
    BT_Serial.println(steps);
    
}

//-----------------------------------
// 7-Segment API
//-----------------------------------

// 숫자 표시를 위한 데이터
byte digits[10][7] =
{ 
  { 0,0,0,0,0,0,1 }, // 0
  { 1,0,0,1,1,1,1 }, // 1
  { 0,0,1,0,0,1,0 }, // 2
  { 0,0,0,0,1,1,0 }, // 3
  { 1,0,0,1,1,0,0 }, // 4
  { 0,1,0,0,1,0,0 }, // 5
  { 0,1,0,0,0,0,0 }, // 6
  { 0,0,0,1,1,1,1 }, // 7
  { 0,0,0,0,0,0,0 }, // 8
  { 0,0,0,1,1,0,0 }  // 9
};

// 원하슨 수를 출력
// pin : 2,3,4,5,6,7,8,9
void displayDigit(int num){
  int pin = 2;
  for(int i=0;i<7;i++){
    digitalWrite(pin+i, digits[num][i]);  
  }
}



