#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

//배선 및 통신 관련
#define UNO_TX 12
#define UNO_RX 13
#define RATE 9600

//음악 관련
#define SEC 1000UL
#define MINUTE 60000UL
#define MUSIC_NUMBER 5
#define EFFECT_NUMBER 5

SoftwareSerial mp3_serial(10, 11);  // RX, TX
SoftwareSerial dataSerial(UNO_TX, UNO_RX);
DFPlayerMini_Fast mp3;

unsigned long last_play_time, cur_play_time;
unsigned long last_interval, cur_interval;
int data;
int track_number;
int situation;
bool is_bgm;

enum situations {
  WAIT = 0,
  MENU,
  SNAKE,
  BREAK
};

unsigned long game_music_length[MUSIC_NUMBER+1] = {
  0,
  (1*MINUTE)+41*SEC,
  36*SEC,
  (2*MINUTE)+2*SEC,
  (1*MINUTE)+54*SEC,
  1*SEC
};
unsigned long effect_music_length[EFFECT_NUMBER+1] = {
  0,
  2*SEC,
  2*SEC,
  33*SEC
};

bool is_play = true;

void setup() {
  Serial.begin(RATE);
  mp3_serial.begin(RATE);
  mp3.begin(mp3_serial);

  dataSerial.begin(RATE);

  mp3.volume(16);
  delay(100);
  for (int i = 0; i <= MUSIC_NUMBER; i++) {
    Serial.print("game_music_length[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(game_music_length[i]);
  }
} 

void loop() {
  if (dataSerial.available() > 0) {
    data = dataSerial.parseInt();
    situation = (data >> 6) & 0x3F;
    track_number = (data >> 1) & 0x0F;
    is_play = data & 0x01;

    Serial.print("data received : ");
    Serial.print(data);
    Serial.print(", ");
    Serial.print(situation);
    Serial.print(", ");
    Serial.print(track_number);
    Serial.print(", ");
    Serial.print(is_play);
    Serial.println();

    cur_play_time = millis();  // 현재 시간을 밀리초로 가져옴
    cur_interval = game_music_length[track_number];
    Serial.print("play time :");
    Serial.print(last_play_time);
    Serial.print(", ");
    Serial.println(cur_play_time);
    Serial.print("interval time :");
    Serial.print(last_interval);
    Serial.print(", ");
    Serial.println(cur_interval);
    if(!is_play && data){
      cur_play_time = millis();  
      last_play_time = cur_play_time;
      last_interval = 0;
    }
    // 지정된 시간 간격이 경과했을 때만 음악을 재생
    if (is_play && (cur_play_time - last_play_time >= last_interval)) {
      mp3.playFromMP3Folder(track_number);
      last_interval = cur_interval;
      last_play_time = cur_play_time;  // 마지막으로 음악을 재생한 시간을 업데이트
    }
  }
}
