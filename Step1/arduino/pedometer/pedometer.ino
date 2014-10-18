#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"

// MPU6050 체어 객체 생성 
MPU6050 accelgyro;

// 중안 사용할 변수들 ( 가속도, 자이로, 평균 가속도 )
int16_t ax, ay, az;
int16_t gx, gy, gz;

// 동작 중인지 표시할 LED
#define LED_PIN 13
bool blinkState = false;

void setup() {
    Wire.begin();

    Serial.begin(115200);

    // MPU6050 초기화 
    accelgyro.initialize(); 
    accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

    // 연결 테스트 
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // 동작 표시 LED 초기화 
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);
    
    // 시리얼로 출력 
    print_data_using_string(ax, ay, az, gx, gy, gz);
    // print_data_for_processing(ax, ay, az, gx, gy, gz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    
    delay(20);
}

// 시리얼로 사람이 보기 쉽게 출력
void print_data_using_string(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz){
    Serial.print("Accelerator => (");
    Serial.print(ax); Serial.print(",\t");
    Serial.print(ay); Serial.print(",\t");
    Serial.print(az); Serial.print(")\t");
    
    Serial.print("Gyro => (");
    Serial.print(gx); Serial.print(",\t");
    Serial.print(gy); Serial.print(",\t");
    Serial.print(gz); Serial.print(")\n");  
}

// 프로세싱으로 처리하기위해서 출력 
void print_data_for_processing(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz){
    // display tab-separated accel/gyro x/y/z values
    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.print(az); Serial.print(",");
    Serial.print(gx); Serial.print(",");
    Serial.print(gy); Serial.print(",");
    Serial.print(gz); Serial.print("\n");
}

