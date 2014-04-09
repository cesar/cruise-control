#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

void setup_LCD();
void enable_LCD();
void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
void selectLineOne();
void selectLineTwo();
void changeCursorUnderscore();

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__