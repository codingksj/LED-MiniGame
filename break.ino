#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <RGBmatrixPanel.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

//배선 관련
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
#define HIGH 1
#define LOW 0
#define BAUD_RATE 9600

//매트릭스 관련
#define MAT_R 32 
#define MAT_C 64
#define EDGE 2
#define DELAY_TIME 50
#define COLOR_COUNT 7

#define BALL_DELAY 200
#define PADDLE_DELAY 50

//음악 관련
#define SONG_COUNT 8
#define MUSIC_INTERVAL 6000

//함수 목록
void initializeGame();
void drawEdge();
void drawBricks();
void drawPaddle();
void PlayBackGroundMusicMP3();
void drawBall();
void movePaddle() ; 
void drawPaddle();
void drawBricks();
void gameOver();
void minigame();
void startBreak();

SoftwareSerial mega_serial(MEGA_TX, MEGA_RX); 

enum button_types {
  NONE = 0,
  LEFT = 1,
  UP,
  RIGHT,
  DOWN,
  SELECT
};

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
int matrix_colors[MAT_R][MAT_C];

// 게임 영역 크기
const int width = 64;
const int height = 32;

// 벽돌 상태
bool bricks[width][height];
int breaked_bricks[3] = {0,0,0};
int break_count = 0;
// 공의 현재 위치
int ballX = 32;
int ballY = 24;

// 공의 이동 방향
int ballSpeedX = 1;
int ballSpeedY = 1;

//패들의 현재 위치
int paddleX = 32;
int paddleY = 31;

//패들의 이동방향
int paddleSpeedX = -1;

//점수 상태
int score = 0;

//게임 상태
bool game = true ;

// 랜덤으로 특별한 벽돌 선택
int specialBrickX = 1 + random(62) ;
int specialBrickY = 1 + random(10) ;


//시드 값을 초기화해서 랜덤함수 값 바꾸는 코드 게임 재시작시 필요
/*void setup() {
  srand(time(0));
}*/   

//효과음 함수
void playSoundEffect() {
  int soundEffectFileNumber = 1; // 예시로 1번 파일 사용
  //mp3.playFile(soundEffectFileNumber) ;
  delay(1000);
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


// 초기화 함수
void initializeGame() {
  // 벽돌 초기화
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 19; j++) {
      bricks[i][j] = true;
    }
  }
  for (int i = 1; i < width - 1; i++) {
    for (int j = height - 19; j < height; j++) {
      bricks[i][j] = false;
    }
}
for (int i = 1; i < height; i++) {
  bricks[0][i] = false ;
  bricks[63][i] = false ;
}
for (int i = 0; i < width; i++) {
  bricks[i][0] = false ;
}
}

//음악 실행 함수
void PlayBackGroundMusicMP3(const int bgm_number) {
}
// 공을 움직이는 함수
void moveBall() {
  // 현재 위치에서 공을 지웁니다.
  bool is_paddle;
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

  is_paddle = (ballY + 1 == paddleY) && (ballX >= paddleX && ballX <= paddleX + 3);
  is_floor = (ballY >= height - EDGE);
  is_ceil = (ballY <= EDGE);
  is_side = (ballX <= EDGE) || (ballX >= width - EDGE - 1);

  if (is_floor) {
    game = false;
    return;
  }
  if (is_paddle) {
    ballSpeedY = -ballSpeedY;
    ballSpeedX = -ballSpeedX;
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
    bricks[nextX][ballY] = false;
    ballSpeedX = -ballSpeedX;
    playSoundEffect();
  }
  if (bricks[ballX][nextY]) {
    bricks[ballX][nextY] = false;
    ballSpeedY = -ballSpeedY;
    playSoundEffect();
  } 
  else if (bricks[nextX][nextY]) {
    bricks[nextX][nextY] = false;
    ballSpeedY = -ballSpeedY;
    ballSpeedX = -ballSpeedX;
    playSoundEffect();
  }
}

// 벽돌 그리기 함수
void drawBricks() {
  for (int i = 1; i < width - 1 ; i++) {
    for (int j = 1; j < height - 19 ; j++) {
      matrix.drawPixel(i, j, bricks[i][j] ? matrix.Color333(0, 7, 0) : matrix.Color333(0, 0, 0)); // 벽돌 색상 설정
    }
  }
  if (bricks[specialBrickX][specialBrickY]) {
    matrix.drawPixel(specialBrickX, specialBrickY, matrix.Color333(3, 5, 3)); //특별한 벽돌 색상변경
  }
}


//공 그리는 함수 
void drawBall() { 
  matrix.drawPixel(ballX, ballY, matrix.Color333(7, 7, 7));
}

//패들 그리는 함수
void drawPaddle() {
    matrix.drawLine(paddleX-1, paddleY, paddleX+2, paddleY,  matrix.Color333(2, 7, 7));
}
//패들 움직이는 함수
void movePaddle() {
  int btn = ProcessInputButton1();
  if ( paddleX != EDGE && btn == LEFT) {
    Serial.println("left");
    matrix.drawLine(paddleX-1, paddleY, paddleX+2, paddleY,  matrix.Color333(0, 0, 0));
    paddleX+=paddleSpeedX;
  }
  else if ( paddleX != MAT_C-EDGE-1 && btn == RIGHT) {
    Serial.println("right");
    matrix.drawLine(paddleX-1, paddleY, paddleX+2, paddleY,  matrix.Color333(0, 0, 0));
    paddleX-=paddleSpeedX;
  }
 }



//모서리 벽 그리기 함수
void drawEdge() {
  matrix.drawLine(0,0,0,31,matrix.Color333(0,0,7));
  matrix.drawLine(1,0,62,0,matrix.Color333(0,0,7));
  matrix.drawLine(63,0,63,31,matrix.Color333(0,0,7));
}

//점수 계산 함수
void ScoreCount() {
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 19; j++) {
      if (bricks[i][j] == false) {
        score++;
      }
    }
  }
}

//게임오버 출력 함수
void gameOver() {
  matrix.fillScreen(0); 
  matrix.setCursor(5, 32/ 2 - 4); 
  matrix.print("GAME OVER");
  delay(5000);
}

//미니게임 함수
void minigame() {
  matrix.fillScreen(0);
}

//벽돌깨기 게임 실행
void startBreak() {
  unsigned int prev_ball_move = 0;
  unsigned int cur_ball_move = 0;
  unsigned int prev_paddle_move = 0;
  unsigned int cur_paddle_move = 0;

  ballX = 32;
  ballY = 24;

  // 공의 이동 방향
  ballSpeedX = 1;
  ballSpeedY = 1;

  //패들의 현재 위치
  paddleX = 32;
  paddleY = 31;

  //패들의 이동방향
  paddleSpeedX = -1;

  //점수 상태
  score = 0;

  //게임 상태
  game = true ;
  initializeGame();
  drawEdge();
  drawBricks();
//PlayBackGroundMusicMP3(); 음악 정해서 ()안에 넣기
  drawBall();
  drawPaddle();
//텍스트 세팅
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7, 7, 7));
  while (true) {
    cur_ball_move = millis();
    if(cur_ball_move - prev_ball_move >= BALL_DELAY){
      moveBall();
      drawBall();
      prev_ball_move = cur_ball_move;
    }
    cur_paddle_move = millis();
    if(cur_paddle_move - prev_paddle_move >= PADDLE_DELAY){
      Serial.print("paddle : ");
      Serial.println(paddleX);
      movePaddle();
      drawPaddle(); 
      prev_paddle_move = cur_paddle_move;
    }
    drawBricks();
    if (!game) {
      gameOver();
      matrix.fillScreen(0);
      break ;
    }
  }
}
  

// 초기 설정 함수
void setup() {
//통신 세팅
  Serial.begin(9600);
  mega_serial.begin(BAUD_RATE);
//매트릭스 세팅
  matrix.begin(); // RGB 매트릭스 초기화
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7, 7, 7)); 

}

// 메인 루프 함수
void loop() {
   startBreak() ;
}