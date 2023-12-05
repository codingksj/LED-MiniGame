#include <EEPROM.h>

int score = 0;

void SaveScoreToEEPROM(int score) {
  EEPROM.put(0, score);
  EEPROM.commit(); // 데이터를 저장
}

int LoadScoreFromEEPROM() {
  int savedScore;
  EEPROM.get(0, savedScore); // EEPROM에서 데이터를 불러옴
  return savedScore;
}

// 게임 종료 후에 호출하여 현재 점수를 저장
void gameOver() {
  SaveScoreToEEPROM(score);
  matrix.fillScreen(0); 
  matrix.setCursor(5, 32/ 2 - 4); 
  matrix.print(score);
  delay(5000);
}

void EatFruit(char* p1_snake_x, char* p1_snake_y, Snake* snake, Coord* food) {
  if (food->x == p1_snake_x[0] && food->y == p1_snake_y[0]) {
    // 과일을 먹었을 때의 처리
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(0, 0, 0));
    snake->length++;
    score += 1;

    // 스네이크의 길이가 증가했으므로 새로운 과일을 생성
    GenerateFood(food, p1_snake_x, p1_snake_y, snake->length);

    // 과일과 스네이크를 화면에 그림
    matrix.drawRect(food->x, food->y, 2, 2, matrix.Color333(7, 0, 0));
    matrix.drawRect(p1_snake_x[0], p1_snake_y[0], 2, 2, matrix.Color333(7, 7, 0));
  }
}
