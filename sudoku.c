#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
#include "remote.h"
#include "sys.h"
#include "usart.h"

int check(u8 data[9][9]);
int check_9(u8 to_check[9]);
void click_num(u8 number, u8 i, u8 j, u8 data[9][9]);

u8 orig[9][9] = {{6, 0, 5, 0, 0, 0, 3, 0, 9}, {0, 8, 0, 9, 0, 1, 0, 2, 0},
                 {0, 3, 0, 7, 0, 6, 0, 4, 0}, {1, 0, 4, 0, 0, 0, 7, 0, 5},
                 {0, 0, 0, 0, 8, 0, 0, 0, 0}, {3, 0, 8, 0, 0, 0, 2, 0, 4},
                 {0, 5, 0, 4, 0, 3, 0, 9, 0}, {0, 6, 0, 1, 0, 8, 0, 5, 0},
                 {7, 0, 9, 0, 0, 0, 8, 0, 1}};

int check_9(u8 to_check[9]) {
  u8 mark[9];
  u8 i = 0;
  int result = 0;
  for (i = 0; i < 9; i++) {
    mark[i] = 0;
  }
  for (i = 0; i < 9; i++) {
    if (to_check[i] != 0) {
      if (mark[to_check[i] - 1] == 0) {
        mark[to_check[i] - 1] = 1;
      } else {
        result = 1;
        break;
      }
    }
  }
  return result;
}

int check(u8 data[9][9]) {
  u8 i = 0;
  u8 j = 0;
  u8 index = 0;
  u8 temp[9];
  u8 result = 0;
  for (i = 0; i < 9; i++) {
    temp[i] = 0;
  }

  for (i = 0; i < 9;) {
    for (j = 0; j < 9;) {
      for (index = 0; index < 9; index++) {
        temp[index] = data[i + (index % 3)][j + (index / 3)];
      }
      if (check_9(temp) != 0) {
        result = 1;
      }
      j += 3;
    }
    i += 3;
  }

  for (i = 0; i < 9; i++) {
    for (index = 0; index < 9; index++) {
      temp[index] = data[i][index];
    }
    if (check_9(temp) != 0) {
      result = 1;
    }
    for (index = 0; index < 9; index++) {
      temp[index] = data[index][i];
    }
    if (check_9(temp) != 0) {
      result = 1;
    }
  }
  return result;
}

void click_num(u8 number, u8 i, u8 j, u8 data[9][9]) {
  if (orig[i][j] == 0) {
    POINT_COLOR = BROWN;
    LCD_ShowChar(10 + 25 * j, 10 + 25 * i, number + 48, 16, 1);
    data[i][j] = number;
  }
}

int main(void) {
  u8 key;
  u8 k0 = 0;
  u8 t = 0;
  u8 i = 0;
  u8 j = 0;
  u8 row_index = 7;
  u8 col_index = 7;
  u8 data[9][9] = {{6, 0, 5, 0, 0, 0, 3, 0, 9}, {0, 8, 0, 9, 0, 1, 0, 2, 0},
                   {0, 3, 0, 7, 0, 6, 0, 4, 0}, {1, 0, 4, 0, 0, 0, 7, 0, 5},
                   {0, 0, 0, 0, 8, 0, 0, 0, 0}, {3, 0, 8, 0, 0, 0, 2, 0, 4},
                   {0, 5, 0, 4, 0, 3, 0, 9, 0}, {0, 6, 0, 1, 0, 8, 0, 5, 0},
                   {7, 0, 9, 0, 0, 0, 8, 0, 1}};

  delay_init();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  uart_init(9600);
  LCD_Init();
  KEY_Init();
  Remote_Init();
  POINT_COLOR = BLACK;

  LCD_ShowString(97, 242, 200, 16, 16, "Sudoku");

  // draw rows
  for (i = 0; i < 10; i++) {
    LCD_DrawLine(7, row_index, 232, row_index);
    row_index += 25;
  }
  // draw columns
  for (i = 0; i < 10; i++) {
    LCD_DrawLine(col_index, 7, col_index, 232);
    col_index += 25;
  }

  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      if (data[i][j] != 0) {
        LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1); //
      }
    }
  }

  i = 0;
  j = 0;

  while (1) {
    t = KEY_Scan(0);
    switch (t) {
    case WKUP_PRES:
      LCD_Fill(90, 262, 200, 278, WHITE);
      for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
          LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
                   WHITE);
          data[i][j] = orig[i][j];
          if (data[i][j] != 0) {
            LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
          }
        }
      }
      i = 0;
      j = 0;
      break;
    case KEY0_PRES:
      if (k0 == 0) {
        k0 = 1;
      } else {
        k0 = 0;
      }
      break;
    }
    key = Remote_Scan();
    if (key && RmtCnt == 1) {
      switch (key) {
      case 162:
        POINT_COLOR = RED;
        if (check(data) == 1) {
          LCD_Fill(90, 262, 200, 278, WHITE);
          LCD_ShowString(90, 262, 200, 16, 16, "Check: Illegal!");
        } else {
          LCD_Fill(90, 262, 200, 278, WHITE);
          LCD_ShowString(90, 262, 200, 16, 16, "Check: Legal!");
        }
        POINT_COLOR = BLACK;
        break;
      case 98:
        LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
                 WHITE);
        if (data[i][j] != 0) {
          if (orig[i][j] == 0) {
            POINT_COLOR = BROWN;
          }
          LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
        }
        if (k0 == 0) {
          if (i > 0) {
            i -= 1;
          }
        } else {
          if (i == 0) {
            i = 8;
          } else {
            i -= 1;
          }
        }
        break;
      case 168:
        LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
                 WHITE);
        if (data[i][j] != 0) {
          if (orig[i][j] == 0) {
            POINT_COLOR = BROWN;
          }
          LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
        }
        if (k0 == 0) {
          if (i < 8) {
            i += 1;
          }
        } else {
          if (i == 8) {
            i = 0;
          } else {
            i += 1;
          }
        }
        break;
      case 194:
        LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
                 WHITE);
        if (data[i][j] != 0) {
          if (orig[i][j] == 0) {
            POINT_COLOR = BROWN;
          }
          LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
        }
        if (k0 == 0) {
          if (j < 8) {
            j += 1;
          }
        } else {
          if (j == 8) {
            j = 0;
          } else {
            j += 1;
          }
        }
        break;
      case 34:
        LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
                 WHITE);
        if (data[i][j] != 0) {
          if (orig[i][j] == 0) {
            POINT_COLOR = BROWN;
          }
          LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
        }
        if (k0 == 0) {
          if (j > 0) {
            j -= 1;
          }
        } else {
          if (j == 0) {
            j = 8;
          } else {
            j -= 1;
          }
        }
        break;
      case 104:
        click_num(1, i, j, data);
        break;
      case 152:
        click_num(2, i, j, data);
        break;
      case 176:
        click_num(3, i, j, data);
        break;
      case 48:
        click_num(4, i, j, data);
        break;
      case 24:
        click_num(5, i, j, data);
        break;
      case 122:
        click_num(6, i, j, data);
        break;
      case 16:
        click_num(7, i, j, data);
        break;
      case 56:
        click_num(8, i, j, data);
        break;
      case 90:
        click_num(9, i, j, data);
        break;
      }
      LCD_Fill(8 + 25 * j, 8 + 25 * i, 6 + 25 * (j + 1), 6 + 25 * (i + 1),
               LIGHTBLUE);
      if (data[i][j] > 0) {
        if (orig[i][j] == 0) {
          POINT_COLOR = BROWN;
        }
        LCD_ShowChar(16 + 25 * j, 12 + 25 * i, data[i][j] + 48, 16, 1);
      }
      POINT_COLOR = BLACK;
    }
    delay_ms(100);
  }
}
