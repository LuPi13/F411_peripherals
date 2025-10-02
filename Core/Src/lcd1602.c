#include "lcd1602.h"

// 내부 함수
static void LCD_Write(uint8_t control, uint8_t data)
{
    // control: 0x00=command, 0x40=data
    uint8_t buf[2];
    buf[0] = control;
    buf[1] = data;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR<<1, buf, 2, HAL_MAX_DELAY);
}

void LCD_Init(void)
{
    HAL_Delay(50); // LCD 안정화 대기

    // function set
    LCD_Write(0x00, 0x38);
    HAL_Delay(2);

    // display on/off
    LCD_Write(0x00, 0x0C);
    HAL_Delay(2);

    // clear display
    LCD_Write(0x00, 0x01);
    HAL_Delay(2);

    // entry mode
    LCD_Write(0x00, 0x06);
    HAL_Delay(2);
}

void LCD_SendCommand(uint8_t cmd)
{
    LCD_Write(0x00, cmd);
    HAL_Delay(2);
}

void LCD_SendData(uint8_t data)
{
    LCD_Write(0x40, data);
}

void LCD_DispCGRAM(void)
{
    // 첫 CGRAM
    LCD_SendCommand(0x80);
    LCD_SendCommand(0x40);
    for (int i = 0; i < 16; i++)
    {
        LCD_SendData(0x01);
    }

    // 두번째 CGRAM
    LCD_SendCommand(0xC0);
    LCD_SendCommand(0x40);
    for (int i = 0; i < 16; i++)
    {
        LCD_SendData(0x01);
    }
}

void LCD_DispChar(int line, int column, char *dp)
{
    // DDRAM 주소 설정
    LCD_SendCommand(0x80 + (line - 1) * 0x40 + (column - 1));
    // 데이터 전송
    int i;
    for (i = 0; i < 16 && dp[i] != '\0'; i++)
    {
        LCD_SendData((uint8_t)dp[i]);
    }
    // 나머지 공간을 공백으로 채움
    for (; i < 16; i++)
    {
        LCD_SendData(' ');
    }
}
