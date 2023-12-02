#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <RGBmatrixPanel.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

//배선 및 통신 관련
#define CLK 11 
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define MEGA_TX 14
#define MEGA_RX 15
#define BTN_SRC_1 A7
#define BAUD_RATE 9600

//매트릭스 관련
#define MAT_R 32 
#define MAT_C 64
#define EDGE 1
#define DELAY_TIME 125
#define COLOR_COUNT 6

//음악 관련
#define SEC 1000UL
#define MINUTE 60000UL
#define MUSIC_NUMBER 5

SoftwareSerial mega_serial(MEGA_TX, MEGA_RX); 

typedef struct Rectangle{
  int x;
  int y;
  int w;
  int h;
}Rectangle;

enum button_types {
  NONE = 0,
  LEFT,
  UP,
  RIGHT,
  DOWN,
  SELECT,
};
enum situations {
  WAIT = 0,
  MENU,
  SNAKE,
  BREAK
};
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
Rectangle title = {10, 4, MAT_C-2*10, 9};
Rectangle menu_1 = {15, 14, MAT_C-2*15, 7};
Rectangle menu_2 = {15, 21, MAT_C-2*15, 7};
const char button_names[6][10] = {
  "NONE",
  "LEFT",
  "UP",
  "RIGHT",
  "DOWN",
  "SELECT"
};
int matrix_colors[MAT_R][MAT_C];
int dx[] = {1,0,-1,0};
int dy[] = {0,1,0,-1};

int title_pixels[] = {
  398, 402, 404, 406, 410, 412, 417, 418, 419, 422,
  425, 429, 431, 432, 433, 462, 463, 465, 466, 468,
  470, 471, 474, 476, 480, 485, 487, 489, 490, 492,
  493, 495, 526, 528, 530, 532, 534, 536, 538, 540,
  544, 546, 547, 549, 550, 551, 553, 555, 557, 559,
  560, 561, 590, 594, 596, 598, 601, 602, 604, 608,
  611, 613, 615, 617, 621, 623, 654, 658, 660, 662,
  666, 668, 673, 674, 675, 677, 679, 681, 685, 687,
  688, 689
};
int menu_1_pixels[] = {
  689, 981, 982, 983, 985, 989, 992, 995, 998,
  1000, 1001, 1002, 1045, 1049, 1050, 1053, 1055, 1057,
  1059, 1061, 1064, 1109, 1110, 1111, 1113, 1115, 1117,
  1119, 1120, 1121, 1123, 1124, 1128, 1129, 1130, 1175,
  1177, 1180, 1181, 1183, 1185, 1187, 1189, 1192, 1237,
  1238, 1239, 1241, 1245, 1247, 1249, 1251, 1254, 1256,
  1257, 1258
};
int menu_2_pixels[] = {
  1429, 1430, 1431, 1434, 1435, 1436, 1439,
  1440, 1441, 1444, 1447, 1450, 1493, 1496, 1498, 1501,
  1503, 1507, 1509, 1511, 1513, 1557, 1558, 1559, 1562,
  1563, 1564, 1567, 1568, 1569, 1571, 1572, 1573, 1575,
  1576, 1621, 1624, 1626, 1629, 1631, 1635, 1637, 1639,
  1641, 1685, 1686, 1687, 1690, 1693, 1695, 1696, 1697,
  1699, 1701, 1703, 1706
};
int title_pixels_num = sizeof(title_pixels) / sizeof(int);
int menu_1_pixels_num = sizeof(menu_1_pixels) / sizeof(int);
int menu_2_pixels_num = sizeof(menu_2_pixels) / sizeof(int);

unsigned long game_music_length[MUSIC_NUMBER+1] = {
  0,
  (1*MINUTE)+41*SEC,
  36*SEC,
  (2*MINUTE)+2*SEC,
  (1*MINUTE)+54*SEC,
  1*SEC
};
unsigned long last_play_time, cur_play_time;
unsigned long last_interval, cur_interval;

int ProcessInputButton(const int button_source);

void InitMatrixEdge(uint16_t color); 
void ClearMatrix(int start_x, int start_y, int width, int height);
void PlayWaitAnimation();
void PrintMenu();
int SelectMenu();
void ClearMenu();
void PrintObject(int* pixels, int pixel_num, int start_x, int start_y, uint16_t color);
void ClearObject(int* pixels, int pixel_num, int start_x, int start_y);
void UpdateMatrix();

void PlayBackGroundMusicMP3(const int bgm_number, bool is_play_next);
void StopBackGroundMusic();
void PlaySoundEffectMP3(const int se_number);

void Snake();
void BreakOut();

void setup() {
  //통신 세팅
  Serial.begin(9600);
  mega_serial.begin(BAUD_RATE);
  
  pinMode(BTN_SRC_1, INPUT);

  //매트릭스 세팅
  matrix.begin();
  matrix.setTextSize(1);
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  InitMatrixEdge(matrix.Color333(7, 0, 0));
}


void loop() {
  bool is_game_over = false;

  PlayWaitAnimation();
  PrintMenu();
  SelectMenu();
  ClearMenu();
}

int ProcessInputButton(const int button_src) {
  static unsigned long last_btn_press_time = 0;
  static unsigned long cur_btn_press_time = 0;
  int cur_btn_type = NONE;
  int voltage;

  // 현재 버튼 값 읽기
  voltage = analogRead(button_src);
  cur_btn_press_time = millis();

  if (voltage >= 100 && voltage <= 200) {
    cur_btn_type = UP;
  } 
  else if (voltage <= 50) {
    cur_btn_type = LEFT;
  } 
  else if (voltage >= 450 && voltage <= 550) {
    cur_btn_type = RIGHT;
  } 
  else if (voltage >= 300 && voltage <= 400) {
    cur_btn_type = DOWN;
  } 
  else if (voltage >= 700 && voltage <= 800) {
    cur_btn_type = SELECT;
  } 
  else {
    cur_btn_type = NONE;
  }

  if(cur_btn_press_time-last_btn_press_time < DELAY_TIME){
    return NONE;
  }
  last_btn_press_time = cur_btn_press_time;
  Serial.print("Button Pressed: ");
  Serial.println(button_names[cur_btn_type]);
  return cur_btn_type;
}
void InitMatrixEdge(uint16_t color) {
  matrix.drawRect(0, 0, MAT_C, MAT_R, color);
};
void ClearMatrix(int start_x, int start_y, int width, int height){
  matrix.fillRect(start_x, start_y, width, height, matrix.Color333(0,0,0));
};

/*void PlayWaitAnimation() {
  int xPos = EDGE; // 시작 위치
  int yPos = EDGE; // 시작 위치
  int direction = 0;
  int track_number = 1;
  int cur_color = 0;
  int prev_color = 0;
  int btn = NONE;
  int data;

  uint16_t colors[] = {
    matrix.Color333(7, 0, 0),
    matrix.Color333(0, 0, 7),
    matrix.Color333(0, 7, 0),
    matrix.Color333(7, 7, 0),
    matrix.Color333(0, 7, 7),
    matrix.Color333(7, 7, 7),
  };

  while ((btn = ProcessInputButton()) != SELECT) {
    cur_play_time = millis();
    cur_interval = game_music_length[track_number];
    if(cur_play_time-last_play_time >= last_interval){
      InitMatrixEdge(colors[cur_color]);
      data = (xPos<<6) + (track_number<<1) + 1;
      mega_serial.print(data);
      mega_serial.print(',');
      Serial.print("sent: ");
      Serial.println(data);
      Serial.print("play time :");
      Serial.print(last_play_time);
      Serial.print(", ");
      Serial.println(cur_play_time);
      Serial.print("interval time :");
      Serial.print(last_interval);
      Serial.print(", ");
      Serial.println(cur_interval);

      track_number = (track_number % MUSIC_NUMBER) + 1;
      last_play_time = cur_play_time;
      last_interval = cur_interval;
      prev_color = cur_color;
      cur_color = (cur_color+1) % COLOR_COUNT;
    }
    matrix.drawRect(xPos, yPos, 2, 2, colors[prev_color]);
    delay(DELAY_TIME);
    matrix.drawRect(xPos, yPos, 2, 2, matrix.Color333(0, 0, 0));

    xPos += dx[direction];
    yPos += dy[direction];

    if (xPos + 2 >= MAT_C - EDGE || xPos <= EDGE || yPos + 2 >= MAT_R - EDGE || yPos <= EDGE) {
      // Change direction when hitting the edges
      direction = (direction + 1) % 4;
    }
  }
};*/
void PlayWaitAnimation() {
  int xPos = EDGE; // 시작 위치
  int yPos = EDGE; // 시작 위치
  int direction = 0;
  int track_number = 2;
  int cur_color = 0;
  int prev_color = 0;
  int btn = NONE;
  int data;
  unsigned long last_time = 0;
  unsigned long cur_time = 0;
  uint16_t colors[] = {
    matrix.Color333(7, 0, 0),
    matrix.Color333(0, 0, 7),
    matrix.Color333(0, 7, 0),
    matrix.Color333(7, 7, 0),
    matrix.Color333(0, 7, 7),
    matrix.Color333(7, 7, 7),
  };

  while ((btn = ProcessInputButton(BTN_SRC_1)) != SELECT) {
    PlayBackGroundMusicMP3(track_number, true);
    cur_time = millis();
    if(cur_time-last_time >= 10 * SEC){
      InitMatrixEdge(colors[cur_color]);
      prev_color = cur_color;
      cur_color = (cur_color+1) % COLOR_COUNT;
      last_time = cur_time;
    }
    matrix.drawRect(xPos, yPos, 2, 2, colors[prev_color]);
    delay(100);
    matrix.drawRect(xPos, yPos, 2, 2, matrix.Color333(0, 0, 0));

    xPos += dx[direction];
    yPos += dy[direction];

    if (xPos + 2 >= MAT_C - EDGE || xPos <= EDGE || yPos + 2 >= MAT_R - EDGE || yPos <= EDGE) {
      // Change direction when hitting the edges
      direction = (direction + 1) % 4;
    }
  }
  StopBackGroundMusic();
};
void PrintMenu(){
  matrix.drawRect(title.x, title.y, title.w, title.h, matrix.Color333(3,0,0));
  matrix.drawRect(menu_1.x, menu_1.y, menu_1.w, menu_1.h, matrix.Color333(1,3,0));
  matrix.drawRect(menu_2.x, menu_2.y, menu_2.w, menu_2.h, matrix.Color333(0,7,0));
  PrintObject(title_pixels, title_pixels_num, 0, 0, matrix.Color333(3,0,0));
  PrintObject(menu_1_pixels, menu_1_pixels_num, 0, 0, matrix.Color333(1,3,0));
  PrintObject(menu_2_pixels, menu_2_pixels_num, 0, 0, matrix.Color333(0,7,0));
};

int SelectMenu(){
  bool is_game = false;
  int track_number = 1;
  int cur_color = 0;
  int prev_color = 0;
  int btn = NONE;
  int data;
  while ((btn = ProcessInputButton(BTN_SRC_1)) != SELECT) {
    PlayBackGroundMusicMP3(track_number, false);
    switch(btn){
      case LEFT:
        Serial.println("left");
        break;
      case RIGHT:
        Serial.println("right");
        break;
      case UP:
        Serial.println("up");
        break;
      case DOWN:
        Serial.println("down");
        break;
      case SELECT:
        Serial.println("select");
        break;
      default:
        Serial.println("None");
        break;
    }
  }
  StopBackGroundMusic();
};

void ClearMenu(){
  matrix.drawRect(title.x, title.y, title.w, title.h, matrix.Color333(0,0,0));
  matrix.drawRect(menu_1.x, menu_1.y, menu_1.w, menu_1.h, matrix.Color333(0,0,0));
  matrix.drawRect(menu_2.x, menu_2.y, menu_2.w, menu_2.h, matrix.Color333(0,0,0));
  ClearObject(title_pixels, title_pixels_num, 0, 0);
  ClearObject(menu_1_pixels, menu_1_pixels_num, 0, 0);
  ClearObject(menu_2_pixels, menu_2_pixels_num, 0, 0);
};
void PrintObject(int* pixels, int pixel_num, int start_x, int start_y, uint16_t color){
  for(int i=0;i<pixel_num;i++){
    matrix.drawPixel(start_x+pixels[i]%MAT_C, start_y+pixels[i]/MAT_C, color);
  }
};
void ClearObject(int* pixels, int pixel_num, int start_x, int start_y){
  PrintObject(pixels, pixel_num, start_x, start_y, matrix.Color333(0,0,0));
};
void PlayBackGroundMusicMP3(const int bgm_number, bool is_play_next) {
  static unsigned long last_play_time = 0;
  static unsigned long cur_play_time = 0;
  static unsigned long last_interval = 0;
  static unsigned long cur_interval = 0;
  static int track_number = bgm_number;
  int data;

  cur_play_time = millis();
  cur_interval = game_music_length[track_number];

  if(cur_play_time-last_play_time >= last_interval){
    data = (track_number<<1) + 1;
    mega_serial.print(data);
    mega_serial.print(',');
    Serial.print("sent: ");
    Serial.println(data);
    Serial.print("play time :");
    Serial.print(last_play_time);
    Serial.print(", ");
    Serial.println(cur_play_time);
    Serial.print("interval time :");
    Serial.print(last_interval);
    Serial.print(", ");
    Serial.println(cur_interval);

    track_number = is_play_next ? ((track_number % MUSIC_NUMBER) + 1) : track_number;
    last_play_time = cur_play_time;
    last_interval = cur_interval;
  }
};
void StopBackGroundMusic(){
  mega_serial.print(1<<10);
};
void PlaySoundEffectMP3(const int se_number);




void Snake() {

};
void BreakOut();


