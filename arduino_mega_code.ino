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
#define BTN_SRC_1 A15
#define BTN_SRC_2 A4
#define BAUD_RATE 9600

//매트릭스 관련
#define MAT_R 32 
#define MAT_C 64
#define EDGE 1
#define DELAY_TIME 125
#define RECT_COUNT 5
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

typedef struct Coord{
  int x;
  int y;
}Coord;

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
unsigned long last_play_time = 0;
unsigned long cur_play_time = 0;
unsigned long last_interval = 0;
unsigned long cur_interval = 0;
int track_number = 1;
bool is_p1_selected = false;
bool is_p2_selected = false;

int ProcessInputButton1();
int ProcessInputButton2();

void InitMatrixEdge(uint16_t color); 
void ClearMatrix(int start_x, int start_y, int width, int height);
void PlayWaitAnimation();
void PrintMenu();
int SelectMenu();
void DrawLeftArrow(int start_x, int start_y, uint16_t color);
void DrawRightArrow(int start_x, int start_y, uint16_t color);
int CheckOption(int btn1, int btn2);
int CheckP1(int btn1);
int CheckP2(int btn2);
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
  pinMode(BTN_SRC_2, INPUT);

  //매트릭스 세팅
  matrix.begin();
  matrix.setTextSize(1);
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  InitMatrixEdge(matrix.Color333(7, 0, 0));
}


void loop() {
  int option = WAIT;

  PlayWaitAnimation();
  PrintMenu();
  option = SelectMenu();
  switch(option){
    case SNAKE:
      Snake();
      break;
    case BREAK:
      BreakOut();
      break;
    default:
      break;
  }
  ClearMatrix(1, 1, MAT_C-2, MAT_R-2);
}

int ProcessInputButton1() {
  static unsigned long last_btn_press_time = 0;
  static unsigned long cur_btn_press_time = 0;
  int cur_btn_type = NONE;
  int voltage;

  // 현재 버튼 값 읽기
  voltage = analogRead(BTN_SRC_1);
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
  //Serial.print("Button1 Pressed: ");
  //Serial.println(button_names[cur_btn_type]);
  return cur_btn_type;
}
int ProcessInputButton2(){
  static unsigned long last_btn_press_time = 0;
  static unsigned long cur_btn_press_time = 0;
  int cur_btn_type = NONE;
  int voltage;

  voltage = analogRead(BTN_SRC_2);
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
  //Serial.print("Button2 Pressed: ");
  //Serial.println(button_names[cur_btn_type]);
  return cur_btn_type;
};

void InitMatrixEdge(uint16_t color) {
  matrix.drawRect(0, 0, MAT_C, MAT_R, color);
};
void ClearMatrix(int start_x, int start_y, int width, int height){
  matrix.fillRect(start_x, start_y, width, height, matrix.Color333(0,0,0));
};

void PlayWaitAnimation() {
  Rectangle rects[RECT_COUNT] = {
    {EDGE, EDGE, 2, 2}, 
    {EDGE+2, EDGE, 4, 4},
    {EDGE+11, EDGE+4, 6, 6},
    {EDGE+30, EDGE+10, 8, 8},
    {EDGE+40, EDGE+19, 10, 10},
  };
  int dirs[RECT_COUNT] = {0,1,2,3,0};
  int cur_color = 0;
  int prev_color = 0;
  int btn1 = NONE;
  int btn2 = NONE;
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
  track_number = 2;
  while (!(btn1 == SELECT || btn2 == SELECT)) {
    btn1 = ProcessInputButton1();
    btn2 = ProcessInputButton2();
    PlayBackGroundMusicMP3(track_number, false);
    cur_time = millis();
    if(cur_time-last_time >= 5 * SEC){
      InitMatrixEdge(colors[cur_color]);
      prev_color = cur_color;
      cur_color = (cur_color+1) % COLOR_COUNT;
      last_time = cur_time;
    }

    for(int i=0;i<RECT_COUNT;i++){
      rects[i].x+=dx[dirs[i]];
      rects[i].y+=dy[dirs[i]];
      matrix.drawRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, colors[prev_color]);
      if ((rects[i].x + rects[i].w) >= MAT_C - EDGE || rects[i].x <= EDGE || (rects[i].y + rects[i].h) >= MAT_R - EDGE || rects[i].y <= EDGE) {
        dirs[i] = (dirs[i] + 1) % 4;
      }
    }
    delay(75);
    for(int i=0;i<RECT_COUNT;i++){
      matrix.drawRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, matrix.Color333(0, 0, 0));
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
  int game_number = WAIT;
  int btn1 = NONE;
  int btn2 = NONE;
  int option1 = WAIT;
  int option2 = WAIT;
  int data;

  track_number = 1;
  is_p1_selected = is_p2_selected = false;

  while (true) {
    btn1 = ProcessInputButton1();
    btn2 = ProcessInputButton2();
    PlayBackGroundMusicMP3(track_number, false);
    option1 = CheckP1(btn1);
    option2 = CheckP2(btn2);
    if(option1 == option2 && option1>0){
      break;
    }
  }
  game_number = option1;
  Serial.print("you choose : ");
  Serial.println(game_number);
  delay(1500);
  StopBackGroundMusic();
};

void DrawLeftArrow(int start_x, int start_y, uint16_t color){
  matrix.drawPixel(start_x, start_y, color);
  matrix.drawLine(start_x+1, start_y+1, start_x+1, start_y-1, color);
  matrix.drawLine(start_x+2, start_y+2, start_x+2, start_y-2, color);
};

void DrawRightArrow(int start_x, int start_y, uint16_t color){
  matrix.drawPixel(start_x, start_y, color);
  matrix.drawLine(start_x-1, start_y+1, start_x-1, start_y-1, color);
  matrix.drawLine(start_x-2, start_y+2, start_x-2, start_y-2, color);
};

int CheckP1(int btn1){
  Coord left_arrows[2] = {
    {menu_1.x + menu_1.w + 1, menu_1.y + 3},
    {menu_2.x + menu_2.w + 1, menu_2.y + 3}
  };
  static Coord cursor_1 = left_arrows[0];

  switch (btn1) {
    case UP:
      DrawLeftArrow(left_arrows[1].x, left_arrows[1].y, matrix.Color333(0,0,0));
      cursor_1 = left_arrows[0];
      DrawLeftArrow(cursor_1.x, cursor_1.y, matrix.Color333(0,7,0));
      is_p1_selected = false;
      break;
    case DOWN:
      DrawLeftArrow(left_arrows[0].x, left_arrows[0].y, matrix.Color333(0,0,0));
      cursor_1 = left_arrows[1];
      DrawLeftArrow(cursor_1.x, cursor_1.y, matrix.Color333(0,7,0));
      is_p1_selected = false;
      break;
    case SELECT:
      DrawLeftArrow(left_arrows[0].x, left_arrows[0].y, matrix.Color333(0,0,0));
      DrawLeftArrow(left_arrows[1].x, left_arrows[1].y, matrix.Color333(0,0,0));
      DrawLeftArrow(cursor_1.x, cursor_1.y, matrix.Color333(7,0,0));
      is_p1_selected = true;
      break;
    default:
      break;
  }
  if(!is_p1_selected){
    return WAIT;
  }
  return (cursor_1.y == left_arrows[0].y ? SNAKE : BREAK);
};
int CheckP2(int btn2){
  Coord right_arrows[2] = {
    {menu_1.x - 2, menu_1.y + 3},
    {menu_2.x - 2, menu_2.y + 3}
  };
  static Coord cursor_2 = right_arrows[0];

  switch (btn2) {
    case UP:
      DrawRightArrow(right_arrows[1].x, right_arrows[1].y, matrix.Color333(0, 0, 0));
      cursor_2 = right_arrows[0];
      DrawRightArrow(cursor_2.x, cursor_2.y, matrix.Color333(0, 7, 0));
      is_p2_selected = false;
      break;
    case DOWN:
      DrawRightArrow(right_arrows[0].x, right_arrows[0].y, matrix.Color333(0, 0, 0));
      cursor_2 = right_arrows[1];
      DrawRightArrow(cursor_2.x, cursor_2.y, matrix.Color333(0, 7, 0));
      is_p2_selected = false;
      break;
    case SELECT:
      DrawRightArrow(right_arrows[0].x, right_arrows[0].y, matrix.Color333(0, 0, 0));
      DrawRightArrow(right_arrows[1].x, right_arrows[1].y, matrix.Color333(0, 0, 0));
      DrawRightArrow(cursor_2.x, cursor_2.y, matrix.Color333(7, 0, 0));
      is_p2_selected = true;
      break;
    default:
      break;
  }
  if(!is_p2_selected){
    return WAIT;
  }
  return (cursor_2.y == right_arrows[0].y ? SNAKE : BREAK);
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
  int data;

  cur_play_time = millis();
  cur_interval = game_music_length[track_number];
  /*Serial.print(last_play_time);
  Serial.print(",");
  Serial.print(cur_play_time);
  Serial.print(",");
  Serial.println(last_interval);*/
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
  mega_serial.print(2);
  mega_serial.print(",");
  Serial.println("STOP!");
  cur_play_time = millis();  
  last_play_time = cur_play_time;
  last_interval = 0;
};

void PlaySoundEffectMP3(const int se_number);




void Snake() {

};
void BreakOut();


