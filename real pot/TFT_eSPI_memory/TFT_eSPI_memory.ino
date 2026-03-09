#include <SPI.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <AnimatedGIF.h>

// DHT11 핀
#define DHTPIN 26   
#define DHTTYPE DHT11   

DHT dht(26, DHT11);

// TFT 설정
TFT_eSPI tft = TFT_eSPI();

// GIF 이미지 파일 경로
#include "C:/Users/FORYOUCOM/Desktop/real pot/TFT_eSPI_memory/test_images/gif_happy.h"
#include "C:/Users/FORYOUCOM/Desktop/real pot/TFT_eSPI_memory/test_images/gif_default.h"
#include "C:/Users/FORYOUCOM/Desktop/real pot/TFT_eSPI_memory/test_images/gif_sad.h"

AnimatedGIF gif;

const uint8_t* GIF_IMAGE = gif_happy;  // 기본적으로 happy.h를 표시

void setup() {
  Serial.begin(115200);
  
  // DHT11 초기화
  dht.begin();
  delay(2000); // 센서 안정화를 위한 2초 대기
  
  // TFT 화면 초기화
  tft.begin();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  
  // GIF 라이브러리 초기화
  gif.begin(BIG_ENDIAN_PIXELS); 
}

void loop() {
  // 온도 습도 측정
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // 섭씨 온도 읽기

  // 값이 유효하지 않으면 출력하지 않음
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 온도 습도 출력
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  // 온도 습도를 조건
  if (humidity >= 60.0 && temperature >= 25.0) {
    // 온도 습도가 모두 기준을 초과 happy.h
    GIF_IMAGE = gif_happy;
  }
  else if (humidity < 60.0 && temperature >= 25.0) {
    // 온도는 기준 이상, 습도는 기준 이하default.h
    GIF_IMAGE = gif_default;
  }
  else {
    // 온도 습도가 모두 기준 이하 sad.h
    GIF_IMAGE = gif_sad;
  }

  // GIF 이미지 표시
  if (gif.open(const_cast<uint8_t*>(GIF_IMAGE), sizeof(gif_happy), GIFDraw)) {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // TFT chip select를 잠그고
    while (gif.playFrame(true, NULL)) {
        yield();
    }
    gif.close();
    tft.endWrite(); // TFT chip select를 풀어줌
  }

  delay(1000);  // 1초마다 측정 및 이미지 갱신
}
