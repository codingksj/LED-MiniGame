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
#define BTN_SRC_1 A4
#define BTN_SRC_2 A5
#define BAUD_RATE 9600

#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

//매트릭스 관련
#define MAT_R 32 
#define MAT_C 64
#define EDGE 2
#define DELAY_TIME 100
#define RECT_COUNT 5
#define COLOR_COUNT 5

//음악 관련
#define SEC 1000UL
#define MINUTE 60000UL
#define MUSIC_NUMBER 10
#define EFFECT_NUMBER 10

#define SNAKE_DELAY 200
#define SNAKE_1_MAX_LENGTH 420
#define SNAKE_2_MAX_LENGTH 300

#define BALL_DELAY 200
#define PADDLE_DELAY 50
#define BRICK_NUM 20

SoftwareSerial mega_serial(MEGA_TX, MEGA_RX); 
SoftwareSerial mp3_serial(6, 7); 
DFPlayerMini_Fast mp3;

typedef struct Rectangle{
  int x;
  int y;
  int w;
  int h;
}Rectangle;

typedef struct Coord {
  int x;
  int y;
}Coord;

typedef struct Snake {
  int length;
  int cur_dir;
  int prev_dir;
}Snake;

typedef Rectangle Brick;
typedef Rectangle Paddle;

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
enum player_number {
  SINGLE = 2,
  MULTI
};

//---------- 전역변수 ----------
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
Rectangle title = {10, 4, MAT_C-2*10, 9};
Rectangle menu_1 = {15, 14, MAT_C-2*15, 7};
Rectangle menu_2 = {15, 21, MAT_C-2*15, 7};
Coord right_arrows[2] = {
  {menu_1.x - 2, menu_1.y + 3},
  {menu_2.x - 2, menu_2.y + 3}
};
Coord left_arrows[2] = {
  {menu_1.x + menu_1.w + 1, menu_1.y + 3},
  {menu_2.x + menu_2.w + 1, menu_2.y + 3}
};
Coord cursor_1 = right_arrows[0];
Coord cursor_2 = left_arrows[0];
const char str[] PROGMEM = "GAME OVER";
const char str2[] PROGMEM = "YOUR SCORE WAS:";
const char button_names[6][10] = {
  "NONE",
  "LEFT",
  "UP",
  "RIGHT",
  "DOWN",
  "SELECT"
};

int dx[] = {-1,0,1,0};
int dy[] = {0,-1,0,1};

const int title_pixels[] PROGMEM = {
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

const int menu_1_pixels[] PROGMEM = {
  689, 981, 982, 983, 985, 989, 992, 995, 998,
  1000, 1001, 1002, 1045, 1049, 1050, 1053, 1055, 1057,
  1059, 1061, 1064, 1109, 1110, 1111, 1113, 1115, 1117,
  1119, 1120, 1121, 1123, 1124, 1128, 1129, 1130, 1175,
  1177, 1180, 1181, 1183, 1185, 1187, 1189, 1192, 1237,
  1238, 1239, 1241, 1245, 1247, 1249, 1251, 1254, 1256,
  1257, 1258
};

const int menu_2_pixels[] PROGMEM = {
  1429, 1430, 1431, 1434, 1435, 1436, 1439,
  1440, 1441, 1444, 1447, 1450, 1493, 1496, 1498, 1501,
  1503, 1507, 1509, 1511, 1513, 1557, 1558, 1559, 1562,
  1563, 1564, 1567, 1568, 1569, 1571, 1572, 1573, 1575,
  1576, 1621, 1624, 1626, 1629, 1631, 1635, 1637, 1639,
  1641, 1685, 1686, 1687, 1690, 1693, 1695, 1696, 1697,
  1699, 1701, 1703, 1706
};
const int single_text[] PROGMEM = {
    980, 981, 982, 984, 986, 990, 993, 994, 995, 997, 
    1001, 1002, 1003, 1044, 1048, 1050, 1051, 1054, 1056, 1061, 
    1065, 1108, 1109, 1110, 1112, 1114, 1116, 1118, 1120, 1122, 
    1123, 1125, 1129, 1130, 1131, 1174, 1176, 1178, 1181, 1182, 
    1184, 1187, 1189, 1193, 1236, 1237, 1238, 1240, 1242, 1246, 
    1249, 1250, 1251, 1253, 1254, 1255, 1257, 1258, 1259, 
};
const int multi_text[] PROGMEM = {
    1430, 1434, 1436, 1439, 1441, 1445, 1446, 1447, 1449, 1494, 
    1495, 1497, 1498, 1500, 1503, 1505, 1510, 1513, 1558, 1560, 
    1562, 1564, 1567, 1569, 1574, 1577, 1622, 1626, 1628, 1631, 
    1633, 1638, 1641, 1686, 1690, 1693, 1694, 1697, 1698, 1699, 
    1702, 1705, 
};
int title_pixels_num = sizeof(title_pixels) / sizeof(int);
int menu_1_pixels_num = sizeof(menu_1_pixels) / sizeof(int);
int menu_2_pixels_num = sizeof(menu_2_pixels) / sizeof(int);
int single_text_num = sizeof(single_text) / sizeof(int);
int multi_text_num = sizeof(multi_text) / sizeof(int);

unsigned long game_music_length[MUSIC_NUMBER+1] = {
  0,
  (1*MINUTE)+41*SEC,
  36*SEC,
  (2*MINUTE)+2*SEC,
  (1*MINUTE)+54*SEC,
  (2*MINUTE)+39*SEC,
  (5*MINUTE)+43*SEC
};
unsigned long effect_music_length[EFFECT_NUMBER+1] = {
  0,
  2*SEC,
  5*SEC,
  2*SEC,
  0,
  1*SEC,
};

unsigned long bgm_prev_play_time = 0;
unsigned long bgm_cur_play_time = 0;
unsigned long bgm_prev_interval = 0;
unsigned long bgm_cur_interval = 0;

unsigned long se_prev_play_time = 0;
unsigned long se_cur_play_time = 0;
unsigned long se_prev_interval = 0;
unsigned long se_cur_interval = 0;

int track_number = 1;
int se_number = 1;
bool is_p1_selected = false;
bool is_p2_selected = false;

const int snake_title[] PROGMEM = {
    405, 406, 407, 409, 413, 416, 419, 422, 424, 425, 
    426, 469, 473, 474, 477, 479, 481, 483, 485, 488, 
    533, 534, 535, 537, 539, 541, 543, 544, 545, 547, 
    548, 552, 553, 554, 599, 601, 604, 605, 607, 609, 
    611, 613, 616, 661, 662, 663, 665, 669, 671, 673, 
    675, 678, 680, 681, 682, 
};
const int snake_title_num = sizeof(snake_title) / sizeof(int);
char p1_snake_x[SNAKE_1_MAX_LENGTH] = {0};
char p1_snake_y[SNAKE_1_MAX_LENGTH] = {0};
char p2_snake_x[SNAKE_2_MAX_LENGTH] = {0};
char p2_snake_y[SNAKE_2_MAX_LENGTH] = {0};

const int breakout_title[] PROGMEM = {
    405, 406, 407, 410, 411, 412, 415, 416, 417, 420, 
    423, 426, 469, 472, 474, 477, 479, 483, 485, 487, 
    489, 533, 534, 535, 538, 539, 540, 543, 544, 545, 
    547, 548, 549, 551, 552, 597, 600, 602, 605, 607, 
    611, 613, 615, 617, 661, 662, 663, 666, 669, 671, 
    672, 673, 675, 677, 679, 682, 
};
const int breakout_title_num = sizeof(breakout_title) / sizeof(int);
bool bricks[MAT_C][MAT_R];
int breaked_bricks[3] = {0,0,0};
int break_count = 0;
int ballX = 32;
int ballY = 24;
int ballSpeedX = 1;
int ballSpeedY = 1;
int paddleX = 32;
int paddleY = 31;
int paddleSpeedX = -1;
int breakout_score = 0;
bool is_breakout_game = true ;
int specialBrickX = 1 + random(62) ;
int specialBrickY = 1 + random(10) ;

//---------- 함수 ----------
int ProcessInputButton1();
int ProcessInputButton2();

void InitMatrixEdge(uint16_t color); 
void ClearMatrix(int start_x, int start_y, int width, int height);
void PlayWaitAnimation();
void PrintMenu();
int SelectMenu();
void DrawLeftArrow(int start_x, int start_y, uint16_t color);
void DrawRightArrow(int start_x, int start_y, uint16_t color);
int CheckP1(int btn1);
int CheckP2(int btn2);
void PrintObject(int* pixels, int pixel_num, int start_x, int start_y, uint16_t color);
void ClearObject(int* pixels, int pixel_num, int start_x, int start_y);
void UpdateMatrix();
bool RetryGame();

void PlayBGM(const int bgm_number, bool is_play_next);
void StopBGM();
void PlaySoundEffect(const int se_number, bool is_loop);
void StopSoundEffect();

void PlaySnake();
void PrintSnakeMenu();
int ChooseSnakePlayers();
void StartSnake();
void MoveSnake(char* p1_snake_x, char* p1_snake_y, Snake* snake); 
void DrawSnake1(char* p1_snake_x, char* p1_snake_y, Snake* p1_snake); 
void DrawSnake2(char* p2_snake_x, char* p2_snake_y, Snake* p2_snake); 
bool CheckCollision(char* p1_snake_x, char* p1_snake_y, Snake* snake);
bool CheckOpposite(int prev, int cur);
void GenerateFood(Coord* food, char* p1_snake_x, char* p1_snake_y, int snake_length);
void EatFruit(char* p1_snake_x, char* p1_snake_y, Snake* snake, Coord* food);
void StartSnakeMulti();
bool MultiCheckCollision(char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, Snake* snake, Snake* othersnake);
void MultiGenerateFood(Coord* food, char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, int snake_length, int P2snake_length);
void MultiEatFruit(char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, Snake* snake, Snake* othersnake, Coord* food);

void PlayBreakOut();
void PrintBreakOutMenu();
int ChooseBreakOutPlayers();
void StartBreakOut();
void InitBreakOut();
void MoveBall();
void DrawBricks();
void DrawBall();
void DrawPaddle();
void MovePaddle() ;
void DrawEdge();
void CalcBreakOutScore();
void BreakoutGameOver();
void BreakoutMinigame();

void setup() {
  //통신 세팅
  Serial.begin(9600);
  mega_serial.begin(BAUD_RATE);
  mp3_serial.begin(BAUD_RATE);
  mp3.begin(mp3_serial);
  mp3.volume(10);

  pinMode(BTN_SRC_1, INPUT);
  pinMode(BTN_SRC_2, INPUT);

  //매트릭스 세팅
  matrix.begin();
  matrix.setTextSize(1);
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  InitMatrixEdge(matrix.Color333(0, 0, 7));
}


void loop() {
  int option = WAIT;
  char snake_players = 0;
  char breakout_players = 0;

  PlayWaitAnimation();
  PrintMenu();
  option = SelectMenu();
  ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
  switch(option){
    case SNAKE:
      PlaySnake();
      break;
    case BREAK:
      PlayBreakOut();
      break;
    default:
      break;
  }

  ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
}

int ProcessInputButton1() {
  static unsigned long prev_btn_press_time = 0;
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

  if(cur_btn_press_time-prev_btn_press_time < DELAY_TIME){
    return NONE;
  }
  prev_btn_press_time = cur_btn_press_time;
  //Serial.print("Button1 Pressed: ");
  //Serial.println(button_names[cur_btn_type]);
  return cur_btn_type;
}
int ProcessInputButton2(){
  static unsigned long prev_btn_press_time = 0;
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

  if(cur_btn_press_time-prev_btn_press_time < DELAY_TIME){
    return NONE;
  }
  prev_btn_press_time = cur_btn_press_time;
  //Serial.print("Button2 Pressed: ");
  //Serial.println(button_names[cur_btn_type]);
  return cur_btn_type;
};

void InitMatrixEdge(uint16_t color) {
  matrix.drawRect(0, 0, MAT_C, MAT_R, color);
  matrix.drawRect(1, 1, MAT_C-2, MAT_R-2, color);
};
void ClearMatrix(int start_x, int start_y, int width, int height){
  matrix.fillRect(start_x, start_y, width, height, matrix.Color333(0,0,0));
};

void PlayWaitAnimation() {
  Rectangle rects[RECT_COUNT] = {
    {EDGE+2, EDGE+7, 4, 4},
    {EDGE+51, EDGE+4, 6, 6},
    {EDGE+30, EDGE+10, 8, 8},
    {EDGE+10, EDGE+15, 10, 10},
    {EDGE+1, EDGE+1, 12, 12}, 
  };
  int dirs[RECT_COUNT] = {0,1,2,3,3};
  int cur_color = 0;
  int prev_color = 0;
  int btn1 = NONE;
  int btn2 = NONE;
  int data;

  unsigned long prev_time = 0;
  unsigned long cur_time = 0;
  uint16_t colors[] = {
    matrix.Color333(0, 0, 7),
    matrix.Color333(0, 7, 0),
    matrix.Color333(2, 7, 7),
    matrix.Color333(0, 7, 7),
    matrix.Color333(0, 7, 2),
  };
  track_number = 2;
  se_number = 3;
  while (!(btn1 == SELECT || btn2 == SELECT)) {
    btn1 = ProcessInputButton1();
    btn2 = ProcessInputButton2();
    PlayBGM(track_number, false);
    cur_time = millis();
    if(cur_time-prev_time >= 5 * SEC){
      InitMatrixEdge(colors[cur_color]);
      prev_color = cur_color;
      cur_color = (cur_color+1) % COLOR_COUNT;
      prev_time = cur_time;
    }

    for(int i=0;i<RECT_COUNT;i++){
      rects[i].x+=dx[dirs[i]];
      rects[i].y+=dy[dirs[i]];
      matrix.drawRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, colors[prev_color]);
      if ((rects[i].x + rects[i].w) >= MAT_C - EDGE || rects[i].x <= EDGE || (rects[i].y + rects[i].h) >= MAT_R - EDGE || rects[i].y <= EDGE) {
        dirs[i] = (dirs[i] + 1) % 4;
      }
    }
    delay(50);
    for(int i=0;i<RECT_COUNT;i++){
      matrix.drawRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, matrix.Color333(0, 0, 0));
    }
  }
  PlaySoundEffect(1, true);
  StopBGM();
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
    PlayBGM(track_number, false);
    option1 = CheckP1(btn1);
    option2 = CheckP2(btn2);
    if(option1 == option2 && option1>0){
      break;
    }
  }
  game_number = option1;
  Serial.print("you choose : ");
  Serial.println(game_number);
  PlaySoundEffect(1, true);
  delay(1500);
  StopSoundEffect();
  StopBGM();

  return game_number;
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

int CheckP1(int btn1) {
  switch (btn1) {
    case UP:
      DrawRightArrow(right_arrows[1].x, right_arrows[1].y, matrix.Color333(0, 0, 0));
      cursor_1 = right_arrows[0];
      DrawRightArrow(cursor_1.x, cursor_1.y, matrix.Color333(0, 7, 0));
      is_p1_selected = false;
      break;
    case DOWN:
      DrawRightArrow(right_arrows[0].x, right_arrows[0].y, matrix.Color333(0, 0, 0));
      cursor_1 = right_arrows[1];
      DrawRightArrow(cursor_1.x, cursor_1.y, matrix.Color333(0, 7, 0));
      is_p1_selected = false;
      break;
    case SELECT:
      DrawRightArrow(right_arrows[0].x, right_arrows[0].y, matrix.Color333(0, 0, 0));
      DrawRightArrow(right_arrows[1].x, right_arrows[1].y, matrix.Color333(0, 0, 0));
      DrawRightArrow(cursor_1.x, cursor_1.y, matrix.Color333(7, 0, 0));
      PlaySoundEffect(1, true);
      is_p1_selected = true;
      break;
    default:
      break;
  }
  if (!is_p1_selected) {
    return WAIT;
  }
  return (cursor_1.y == right_arrows[0].y ? SNAKE : BREAK);
};

int CheckP2(int btn2) {
  switch (btn2) {
    case UP:
      DrawLeftArrow(left_arrows[1].x, left_arrows[1].y, matrix.Color333(0, 0, 0));
      cursor_2 = left_arrows[0];
      DrawLeftArrow(cursor_2.x, cursor_2.y, matrix.Color333(0, 7, 0));
      is_p2_selected = false;
      break;
    case DOWN:
      DrawLeftArrow(left_arrows[0].x, left_arrows[0].y, matrix.Color333(0, 0, 0));
      cursor_2 = left_arrows[1];
      DrawLeftArrow(cursor_2.x, cursor_2.y, matrix.Color333(0, 7, 0));
      is_p2_selected = false;
      break;
    case SELECT:
      DrawLeftArrow(left_arrows[0].x, left_arrows[0].y, matrix.Color333(0, 0, 0));
      DrawLeftArrow(left_arrows[1].x, left_arrows[1].y, matrix.Color333(0, 0, 0));
      DrawLeftArrow(cursor_2.x, cursor_2.y, matrix.Color333(7, 0, 0));
      PlaySoundEffect(1, true);
      is_p2_selected = true;
      break;
    default:
      break;
  }
  if (!is_p2_selected) {
    return WAIT;
  }
  return (cursor_2.y == left_arrows[0].y ? SNAKE : BREAK);
};
void PrintObject(const int* pixels, int pixel_num, int start_x, int start_y, uint16_t color) {
  for (int i = 0; i < pixel_num; i++) {
    int pixel = pgm_read_word(&pixels[i]);
    matrix.drawPixel(start_x + pixel % MAT_C, start_y + pixel / MAT_C, color);
  }
}

void ClearObject(const int* pixels, int pixel_num, int start_x, int start_y) {
  PrintObject(pixels, pixel_num, start_x, start_y, matrix.Color333(0, 0, 0));
}

bool RetryGame() {
  ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
  InitMatrixEdge(matrix.Color333(0,7,7));
  matrix.setCursor(15, 5);
  matrix.print("Retry?");

  while (true) {
    int btn1 = ProcessInputButton1();
    int btn2 = ProcessInputButton2();
    if (btn1 == LEFT || btn2 == LEFT){
      return false;
    }
    if (btn1 == SELECT || btn2 == SELECT) {
      return true;
    }
  }
  return false;
}


void PlayBGM(const int bgm_number, bool is_play_next) {
  int data;

  bgm_cur_play_time = millis();
  bgm_cur_interval = game_music_length[track_number];

  if(bgm_cur_play_time-bgm_prev_play_time >= bgm_prev_interval){
    data = (track_number<<1) + 1;
    mega_serial.print(data);
    mega_serial.print(',');
    Serial.print("sent: ");
    Serial.println(data);
    Serial.print("play time :");
    Serial.print(bgm_prev_play_time);
    Serial.print(", ");
    Serial.println(bgm_cur_play_time);
    Serial.print("interval time :");
    Serial.print(bgm_prev_interval);
    Serial.print(", ");
    Serial.println(bgm_cur_interval);

    track_number = is_play_next ? ((track_number % MUSIC_NUMBER) + 1) : track_number;
    bgm_prev_play_time = bgm_cur_play_time;
    bgm_prev_interval = bgm_cur_interval;
  }
};

void StopBGM(){
  mega_serial.print(2);
  mega_serial.print(",");
  Serial.println("STOP BGM!");
  bgm_cur_play_time = millis();  
  bgm_prev_play_time = bgm_cur_play_time;
  bgm_prev_interval = 0;
};

void PlaySoundEffect(const int se_number, bool is_loop){
  mp3.playFromMP3Folder(se_number);
}

void StopSoundEffect(){
  mp3.stop();
};

void PlaySnake(){
  bool is_snake_over = false;
  char snake_players;

  cursor_1 = right_arrows[0];
  cursor_2 = left_arrows[0];

  while(!is_snake_over){
    PrintSnakeMenu();
    snake_players = ChooseSnakePlayers();
    ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
    switch(snake_players){
      case SINGLE:
        StartSnake();
        break;
      case MULTI:
        StartSnakeMulti();
        break;
    }
    is_snake_over = !RetryGame();
    ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
  }
};

void PrintSnakeMenu(){
  matrix.drawRect(title.x, title.y, title.w, title.h, matrix.Color333(3,0,0));
  matrix.drawRect(menu_1.x, menu_1.y, menu_1.w, menu_1.h, matrix.Color333(1,3,0));
  matrix.drawRect(menu_2.x, menu_2.y, menu_2.w, menu_2.h, matrix.Color333(0,7,0));
  PrintObject(snake_title, snake_title_num, 0, 0, matrix.Color333(3,0,0));
  PrintObject(single_text, single_text_num, 0, 0, matrix.Color333(1,3,0));
  PrintObject(multi_text, multi_text_num, 0, 0, matrix.Color333(0,7,0));
};

int ChooseSnakePlayers(){   //2 single,  3 multi
  int game_number = 0;
  int btn1 = NONE;
  int btn2 = NONE;
  int option1 = 0;
  int option2 = 0;
  int data;

  track_number = 1;
  is_p1_selected = is_p2_selected = false;

  while (true) {
    btn1 = ProcessInputButton1();
    btn2 = ProcessInputButton2();
    PlayBGM(track_number, false);
    option1 = CheckP1(btn1);
    option2 = CheckP2(btn2);
    if(option1 == option2 && option1>0){
      break;
    }
  }
  game_number = option1;
  Serial.print("you choose : ");
  Serial.println(game_number);
  PlaySoundEffect(1, true);
  delay(1500);
  StopSoundEffect();
  StopBGM();

  return game_number;
};

void StartSnake() {
  Snake snake = {4, RIGHT, RIGHT};
  Snake* p_snake = &snake;
  Coord snake_init_coords[4] = {
    {12,14},
    {10,14},
    {8,14},
    {6,14}
  };
  Coord food = {10,10};
  Coord head;
  Coord* p_food = &food;
  unsigned long prev_time = 0;
  unsigned long cur_time = 0;
  int btn1 = NONE;
  int btn2 = NONE;
  bool is_food = false;
  bool is_game = true;
  bool is_collide = false;
  bool* p_is_food = &is_food;
  track_number = 5;

  for(int i=0;i<snake.length;i++){
    p1_snake_x[i] = snake_init_coords[i].x;
    p1_snake_y[i] = snake_init_coords[i].y;
  }
  GenerateFood(p_food, p1_snake_x, p1_snake_y, snake.length);
  matrix.drawRect(food.x, food.y, 2, 2, matrix.Color333(7, 0, 0));

  while(is_game){
    PlayBGM(track_number, true);
    btn1 = ProcessInputButton1();
    if(!(btn1 == NONE || btn1 == SELECT || CheckOpposite(btn1, snake.prev_dir))){
      snake.cur_dir = btn1;
    }
    Serial.print("dir : ");
    Serial.print(button_names[snake.cur_dir]);
    Serial.print(" ");
    Serial.print(p1_snake_x[0], DEC);
    Serial.print(" ");
    Serial.println(p1_snake_y[0], DEC);
    
    cur_time = millis();
    if(cur_time - prev_time >= SNAKE_DELAY){
      matrix.drawRect(p1_snake_x[snake.length - 1], p1_snake_y[snake.length - 1], 2, 2, matrix.Color333(0, 0, 0));
      if(!CheckCollision(p1_snake_x, p1_snake_y, p_snake)){
        MoveSnake(p1_snake_x, p1_snake_y, p_snake);
        p1_snake_x[0] += (2*dx[snake.cur_dir-1]);
        p1_snake_y[0] += (2*dy[snake.cur_dir-1]);
        EatFruit(p1_snake_x, p1_snake_y, p_snake, p_food);
      }
      snake.prev_dir = snake.cur_dir;
      prev_time = cur_time;
    }
    if(CheckCollision(p1_snake_x, p1_snake_y, p_snake)){
      InitMatrixEdge(matrix.Color333(7,0,0));
      StopBGM();
      PlaySoundEffect(3, true);
      for (int i = 1; i <= snake.length; i++) {
        matrix.drawRect(p1_snake_x[i], p1_snake_y[i], 2, 2, matrix.Color333(7, 0, 0)); 
      }
      delay(3000);
      break;
    }
    else{
      DrawSnake1(p1_snake_x, p1_snake_y, p_snake);
    }
  }
};

void MoveSnake(char* snake_x, char* snake_y, Snake* snake) {
  for (int i = snake->length; i > 0; i--) {
    snake_x[i] = snake_x[i - 1];
    snake_y[i] = snake_y[i - 1];
  }
}

void DrawSnake1(char* snake_x, char* snake_y, Snake* snake){
  int length = snake->length;
  for (int i = 0; i < length; i++) {
    Serial.print("length : ");
    Serial.println(length);
    matrix.drawRect(snake_x[i], snake_y[i], 2, 2, (i == 0) ? matrix.Color333(7, 7, 0) : matrix.Color333(0, 7, 0)); 
  }
}; 

void DrawSnake2(char* p2_snake_x, char* p2_snake_y, Snake* p2_snake){
  int length = p2_snake->length;
  for (int i = 0; i < length; i++) {
    Serial.print("length : ");
    Serial.println(length);
    matrix.drawRect(p2_snake_x[i], p2_snake_y[i], 2, 2, (i == 0) ? matrix.Color333(0, 7, 0) : matrix.Color333(7, 7, 0)); 
  }
}; 

bool CheckCollision(char* snake_x, char* snake_y, Snake* snake){
  Coord head = {snake_x[0], snake_y[0]};
  int length = snake->length;
  if( head.x < EDGE || head.x >= MAT_C-1-EDGE || head.y < EDGE || head.y >= MAT_R-1-EDGE){
    return true;
  };

  for(int i=1;i<length;i++){
    if(head.x == snake_x[i] && head.y == snake_y[i]){
      return true;
    }
  }
  return false;
};
bool CheckOpposite(int prev, int cur){
  return ((abs(prev-cur) == 2) && !(prev == NONE || cur == NONE));
};
void GenerateFood(Coord* food, char* snake_x, char* snake_y, int snake_length) {
  randomSeed(analogRead(A15));
  while (true) {
    food->x = 2 * (1+random(30));
    food->y = 2 * (1+random(14));

    // 스네이크와 겹치는지 확인
    bool overlap = false;
    for (int i = 0; i < snake_length; i++) {
      if (food->x == snake_x[i] && food->y == snake_y[i]) {
        overlap = true;
        break;
      }
    }

    if (!overlap) {
      break;
    }
  }
}

void EatFruit(char* snake_x, char* snake_y, Snake* snake, Coord* food) {
  if (food->x == snake_x[0] && food->y == snake_y[0]) {
    // 과일을 먹었을 때의 처리
    PlaySoundEffect(5, true);
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(0, 0, 0));
    snake->length++;

    // 스네이크의 길이가 증가했으므로 새로운 과일을 생성
    GenerateFood(food, snake_x, snake_y, snake->length);

    // 과일과 스네이크를 화면에 그림
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(7, 0, 0));
    matrix.drawRect(snake_x[0], snake_y[0], 2, 2, matrix.Color333(7, 7, 0));
  }
}


void StartSnakeMulti() {
  Snake snake = {4, RIGHT, RIGHT};
  Snake* p_snake = &snake;
  Snake P2snake = {4, LEFT, LEFT};
  Snake* p_P2snake = &P2snake;
  Coord P1_snake_init_coords[4] = {
    {12,14},
    {10,14},
    {8,14},
    {6,14}
  };
  Coord P2_snake_init_coords[4] = {
    {50,14},
    {52,14},
    {54,14},
    {56,14}
  };
  Coord food = {10,10};
  Coord p1_head;
  Coord p2_head;
  Coord* p_food = &food;
  unsigned long prev_time = 0;
  unsigned long cur_time = 0;
  int btn1 = NONE;
  int btn2 = NONE;
  bool is_food = false;
  bool is_game = true;
  bool* p_is_food = &is_food;

  for(int i=0;i<snake.length;i++){
    p1_snake_x[i] = P1_snake_init_coords[i].x;
    p1_snake_y[i] = P1_snake_init_coords[i].y;
  }
  for(int i=0;i<P2snake.length;i++){
    p2_snake_x[i] = P2_snake_init_coords[i].x;
    p2_snake_y[i] = P2_snake_init_coords[i].y;
  }

  MultiGenerateFood(p_food, p1_snake_x, p1_snake_y, p2_snake_x, p2_snake_y, snake.length, P2snake.length);
  matrix.drawRect(food.x, food.y, 2, 2, matrix.Color333(7, 0, 0));

  while(is_game){
    PlayBGM(3, false);
    btn1 = ProcessInputButton1();
    if(!(btn1 == NONE || btn1 == SELECT || CheckOpposite(btn1, snake.prev_dir))){
      snake.cur_dir = btn1;
    }
    btn2 = ProcessInputButton2();
    if(!(btn2 == NONE || btn2 == SELECT || CheckOpposite(btn2, P2snake.prev_dir))){
      P2snake.cur_dir = btn2;
    }
    Serial.print("dir : ");
    Serial.print(button_names[snake.cur_dir]);
    Serial.print(" ");
    Serial.print(p1_snake_x[0], DEC);
    Serial.print(" ");
    Serial.println(p1_snake_y[0], DEC);
    
    cur_time = millis();
    if(cur_time - prev_time >= SNAKE_DELAY){
      matrix.drawRect(p1_snake_x[snake.length - 1], p1_snake_y[snake.length - 1], 2, 2, matrix.Color333(0, 0, 0));
      matrix.drawRect(p2_snake_x[P2snake.length - 1], p2_snake_y[P2snake.length - 1], 2, 2, matrix.Color333(0, 0, 0));
      if(!MultiCheckCollision(p1_snake_x, p1_snake_y, p2_snake_x, p2_snake_y, p_snake, p_P2snake)){
        MoveSnake(p1_snake_x, p1_snake_y, p_snake);
        MoveSnake(p2_snake_x, p2_snake_y, p_P2snake);
        p1_snake_x[0] += (2*dx[snake.cur_dir-1]);
        p1_snake_y[0] += (2*dy[snake.cur_dir-1]);
        p2_snake_x[0] += (2*dx[P2snake.cur_dir-1]);
        p2_snake_y[0] += (2*dy[P2snake.cur_dir-1]);
      }
      MultiEatFruit(p2_snake_x, p2_snake_y, p1_snake_x, p1_snake_y, p_P2snake, p_snake, p_food);
      snake.prev_dir = snake.cur_dir;
      P2snake.prev_dir = P2snake.cur_dir;
      prev_time = cur_time;
    }
    if(MultiCheckCollision(p2_snake_x, p2_snake_y, p1_snake_x, p1_snake_y, p_P2snake, p_snake)){
      for (int i = 1; i <= P2snake.length; i++) {
        matrix.drawRect(p2_snake_x[i], p2_snake_y[i], 2, 2, matrix.Color333(7, 0, 0)); 
      }
      for (int i = 1; i <= snake.length; i++) {
        matrix.drawRect(p1_snake_x[i], p1_snake_y[i], 2, 2, matrix.Color333(7, 0, 0)); 
      }
      break;
    }
    else{
      DrawSnake1(p1_snake_x, p1_snake_y, p_snake);
      DrawSnake2(p2_snake_x, p2_snake_y, p_P2snake);
    }
  }
  delay(1500);
  StopBGM();
};

bool MultiCheckCollision(char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, Snake* snake, Snake* othersnake){
  Coord P1head = {snake_x[0], snake_y[0]};
  Coord P2head = {othersnake_x[0], othersnake_y[0]};
  int length = snake->length;
  int otherlength = othersnake->length;
  if( P1head.x < EDGE || P1head.x >= MAT_C-1-EDGE || P1head.y < EDGE || P1head.y >= MAT_R-1-EDGE){
    return true;
  };
  if( P2head.x < EDGE || P2head.x >= MAT_C-1-EDGE || P2head.y < EDGE || P2head.y >= MAT_R-1-EDGE){
    return true;
  };

  for(int i=1;i<length;i++){
    if(P1head.x == snake_x[i] && P1head.y == snake_y[i]){
      return true;
    }
  }
  for(int i=0;i<otherlength;i++){
    if(P1head.x == othersnake_x[i] && P1head.y == othersnake_y[i]){
      return true;
    }
  }
  for(int i=1;i<otherlength;i++){
    if(P2head.x == othersnake_x[i] && P2head.y == othersnake_y[i]){
      return true;
    }
  }
  for(int i=0;i<length;i++){
    if(P2head.x == snake_x[i] && P2head.y == snake_y[i]){
      return true;
    }
  }
  return false;
};

void MultiGenerateFood(Coord* food, char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, int snake_length, int P2snake_length) {
  randomSeed(analogRead(A15));
  while (true) {
    food->x = 2 * (1+random(30));
    food->y = 2 * (1+random(14));

    // 스네이크와 겹치는지 확인
    bool overlap = false;
    for (int i = 0; i < snake_length; i++) {
      if (food->x == snake_x[i] && food->y == snake_y[i]) {
        overlap = true;
        break;
      }
    }
    for (int i = 0; i < P2snake_length; i++) {
      if (food->x == othersnake_x[i] && food->y == othersnake_y[i]) {
        overlap = true;
        break;
      }
    }

    if (!overlap) {
      break;
    }
  }
}

void MultiEatFruit(char* snake_x, char* snake_y, char* othersnake_x, char* othersnake_y, Snake* snake, Snake* othersnake, Coord* food) {
  if (food->x == snake_x[0] && food->y == snake_y[0]) {
    // 과일을 먹었을 때의 처리
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(0, 0, 0));
    snake->length++;

    // 스네이크의 길이가 증가했으므로 새로운 과일을 생성
    MultiGenerateFood(food, snake_x, snake_y, othersnake_x, othersnake_y, snake->length, othersnake->length);

    // 과일과 스네이크를 화면에 그림
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(7, 0, 0));
    matrix.drawRect(snake_x[0], snake_y[0], 2, 2, matrix.Color333(7, 7, 0));
  }
  if (food->x == othersnake_x[0] && food->y == othersnake_y[0]) {
    // 과일을 먹었을 때의 처리
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(0, 0, 0));
    othersnake->length++;

    // 스네이크의 길이가 증가했으므로 새로운 과일을 생성
    MultiGenerateFood(food, othersnake_x, othersnake_y, snake_x, snake_y, othersnake->length, snake->length);

    // 과일과 스네이크를 화면에 그림
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(7, 0, 0));
    matrix.drawRect(othersnake_x[0], othersnake_y[0], 2, 2, matrix.Color333(7, 7, 0));
  }
}

void PlayBreakOut(){
	char breakout_players;

  cursor_1 = right_arrows[0];
  cursor_2 = left_arrows[0];

  PrintBreakOutMenu();
  breakout_players = ChooseBreakOutPlayers();
  ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
  switch(breakout_players){
    case SINGLE:
      StartBreakOut();
      break;
    case MULTI:
      StartBreakOut();
      break;
  }
};

void PrintBreakOutMenu(){
  matrix.drawRect(title.x, title.y, title.w, title.h, matrix.Color333(3,0,0));
  matrix.drawRect(menu_1.x, menu_1.y, menu_1.w, menu_1.h, matrix.Color333(1,3,0));
  matrix.drawRect(menu_2.x, menu_2.y, menu_2.w, menu_2.h, matrix.Color333(0,7,0));
  PrintObject(breakout_title, breakout_title_num, 0, 0, matrix.Color333(3,0,0));
  PrintObject(single_text, single_text_num, 0, 0, matrix.Color333(1,3,0));
  PrintObject(multi_text, multi_text_num, 0, 0, matrix.Color333(0,7,0));
};

int ChooseBreakOutPlayers(){
  int game_number = 0;
  int btn1 = NONE;
  int btn2 = NONE;
  int option1 = 0;
  int option2 = 0;
  int data;

  track_number = 1;
  is_p1_selected = is_p2_selected = false;

  while (true) {
    btn1 = ProcessInputButton1();
    btn2 = ProcessInputButton2();
    PlayBGM(track_number, false);
    option1 = CheckP1(btn1);
    option2 = CheckP2(btn2);
    if(option1 == option2 && option1>0){
      break;
    }
  }
  game_number = option1;
  Serial.print("you choose : ");
  Serial.println(game_number);
  PlaySoundEffect(1, true);
  delay(1500);
  StopSoundEffect();
  StopBGM();

  return game_number;
};

void StartBreakOut() {
  unsigned int prev_ball_move = 0;
  unsigned int cur_ball_move = 0;
  unsigned int prev_paddle_move = 0;
  unsigned int cur_paddle_move = 0;

  ballX = 24;
  ballY = 22;

  // 공의 이동 방향
  ballSpeedX = 1;
  ballSpeedY = 1;

  //패들의 현재 위치
  paddleX = 32;
  paddleY = 31;

  //패들의 이동방향
  paddleSpeedX = -1;

  //점수 상태
  breakout_score = 0;
  track_number = 6;
  is_breakout_game = true ;

  InitBreakOut();
  DrawEdge();
  DrawBricks();
  DrawBall();
  DrawPaddle();

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7, 7, 7));
  while (true) {
    PlayBGM(track_number, true);
    cur_ball_move = cur_paddle_move = millis();
    if(cur_ball_move - prev_ball_move >= BALL_DELAY){
      MoveBall();
      DrawBall();
      DrawEdge();
      prev_ball_move = cur_ball_move;
    }

    Serial.print("paddle : ");
    Serial.println(paddleX);
    /*Serial.print("time : ");
    Serial.print(prev_paddle_move);
    Serial.print(",");
    Serial.println(cur_paddle_move);*/
    MovePaddle();
    DrawPaddle(); 
    prev_paddle_move = cur_paddle_move;

    if (!is_breakout_game) {
      BreakoutGameOver();
      StopBGM();
      ClearMatrix(EDGE, EDGE, MAT_C-2*EDGE, MAT_R-2*EDGE);
      break ;
    }
  }
};
void InitBreakOut() {
  // 벽돌 초기화
  for (int i = 2; i < MAT_C - 2; i++) {
    for (int j = 2; j < MAT_R - 18; j++) {
      bricks[i][j] = true;
    }
  }
  for (int i = 2; i < MAT_C - 2; i++) {
    for (int j = MAT_R - 18; j < MAT_R; j++) {
      bricks[i][j] = false;
    }
  }
  for (int i = 1; i < MAT_R; i++) {
    bricks[0][i] = false ;
    bricks[63][i] = false ;
    bricks[1][i] = false ;
    bricks[62][i] = false ;
  }
  for (int i = 0; i < MAT_C; i++) {
    bricks[i][0] = false ;
    bricks[i][1] = false ;
  }
}
// 공을 움직이는 함수
// 공을 움직이는 함수
void MoveBall() {
  // 현재 위치에서 공을 지웁니다.
  bool is_paddle_L;
  bool is_paddle_M;
  bool is_paddle_R;
  bool is_side;
  bool is_ceil;
  bool is_floor;
  matrix.drawPixel(ballX, ballY, matrix.Color333(0, 0, 0));

  // 새로운 위치로 공을 이동합니다.
  ballX += ballSpeedX;
  ballY += ballSpeedY;
  Serial.print("ball : ");
  Serial.print(ballX);
  Serial.print(",");
  Serial.print(ballY);
  Serial.println();

  is_paddle_L = (ballY + 1 == paddleY) && (ballX >= paddleX - 2 && ballX <= paddleX);
  is_paddle_M = (ballY + 1 == paddleY) && (ballX == paddleX);
  is_paddle_R = (ballY + 1 == paddleY) && (ballX >= paddleX && ballX <= paddleX + 2);
  is_floor = (ballY > MAT_R - EDGE);
  is_ceil = (ballY <= EDGE);
  is_side = (ballX <= EDGE) || (ballX >= MAT_C - EDGE - 1);

  if (is_floor) {
    is_breakout_game = false;
    return;
  }
  if (is_paddle_L) {
    ballSpeedY = -ballSpeedY;
    ballSpeedX = -1;
    return;
  }
  if (is_paddle_M) {
    ballSpeedY = -ballSpeedY;
    ballSpeedX = 0;
    return;
  }
  if (is_paddle_R) {
    ballSpeedY = -ballSpeedY;
    ballSpeedX = 1;
    return;
  }
  // 벽에 부딪혔다면 방향을 변경합니다.
  if (is_side) {
    ballSpeedX = -ballSpeedX;
  }
  if (is_ceil) {
    ballSpeedY = -ballSpeedY;
  }

  // 벽돌과의 충돌 검사
  int nextX = ballX + ballSpeedX;
  int nextY = ballY + ballSpeedY;

  if (bricks[nextX][ballY]) {
    matrix.drawRect(nextX + ballSpeedX, ballY - 1, 2, 2, matrix.Color333(0, 0, 0));
    if (ballSpeedX < 0) {
      for (int i = 0; i > -2; i--) {
        for (int j = 0; j < 2; j++) {
          bricks[nextX + i][ballY - 1 + j] = false;
        }
      }
    } else {
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          bricks[nextX + i][ballY - 1 + j] = false;
        }
      }
    }
    ballSpeedX = -ballSpeedX;
    PlaySoundEffect(4, true);
  } else if (bricks[ballX][nextY]) {
    matrix.drawRect(ballX - 1, nextY - 1, 3, 2, matrix.Color333(0, 0, 0));
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 2; j++) {
        bricks[ballX - 1 + i][nextY - 1 + j] = false;
      }
    }
    ballSpeedY = -ballSpeedY;
    PlaySoundEffect(4, true);
  } else if (bricks[nextX][nextY]) {
    matrix.drawRect(nextX + ballSpeedX, nextY - 1, 2, 2, matrix.Color333(0, 0, 0));
    if (ballSpeedX < 0) {
      for (int i = 0; i > -2; i--) {
        for (int j = 0; j < 2; j++) {
          bricks[nextX + i][nextY - 1 + j] = false;
        }
      }
    } else {
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          bricks[nextX + i][nextY - 1 + j] = false;
        }
      }
    }
    ballSpeedY = -ballSpeedY;
    ballSpeedX = -ballSpeedX;
    PlaySoundEffect(4, true);
  }
}


// 벽돌 그리기 함수
void DrawBricks() {
  for (int i=2;i<14;i++) {
    matrix.drawLine(2,i,61,i,matrix.Color333(0,0,7));
}
  if (bricks[specialBrickX][specialBrickY]) {
    matrix.drawPixel(specialBrickX, specialBrickY, matrix.Color333(3, 5, 3)); //특별한 벽돌 색상변경
  }
}


//공 그리는 함수 
void DrawBall() { 
  matrix.drawPixel(ballX, ballY, matrix.Color333(7, 7, 7));
}

//패들 그리는 함수
void DrawPaddle() {
    matrix.drawLine(paddleX-2, paddleY, paddleX+2, paddleY,  matrix.Color333(2, 7, 7));
}
//패들 움직이는 함수
void MovePaddle() {
  int btn = ProcessInputButton1();
  if ( paddleX != EDGE + 2 && btn == LEFT) {
    Serial.println("left");
    matrix.drawLine(paddleX-2, paddleY, paddleX+2, paddleY,  matrix.Color333(0, 0, 0));
    paddleX+=paddleSpeedX;
  }
  else if ( paddleX != MAT_C-EDGE-3 && btn == RIGHT) {
    Serial.println("right");
    matrix.drawLine(paddleX-2, paddleY, paddleX+2, paddleY,  matrix.Color333(0, 0, 0));
    paddleX-=paddleSpeedX;
  }
 }
//모서리 벽 그리기 함수
void DrawEdge() {
  matrix.drawLine(0,0,0,31,matrix.Color333(0,7,0));
  matrix.drawLine(2,0,61,0,matrix.Color333(0,7,0));
  matrix.drawLine(63,0,63,31,matrix.Color333(0,7,0));
  matrix.drawLine(1,0,1,31,matrix.Color333(0,7,0));
  matrix.drawLine(2,1,61,1,matrix.Color333(0,7,0));
  matrix.drawLine(62,0,62,31,matrix.Color333(0,7,0));
  matrix.drawLine(2,31,61,31,matrix.Color333(0,0,0));
  matrix.drawLine(2,30,61,30,matrix.Color333(0,0,0));
}

//점수 계산 함수
void CalcBreakOutScore() {
  for (int i = 1; i < MAT_C - 1; i++) {
    for (int j = 1; j < MAT_R - 19; j++) {
      if (bricks[i][j] == false) {
        breakout_score++;
      }
    }
  }
}

//게임오버 출력 함수
void BreakoutGameOver() {
  matrix.fillScreen(0); 
  matrix.setCursor(5, 32/ 2 - 4); 
  matrix.print("GAME OVER");
  delay(5000);
}

//미니게임 함수
void BreakoutMinigame() {
  matrix.fillScreen(0);
}
