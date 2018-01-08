#ifndef _FONT_H_
#define _FONT_H_

void FONT_Init();
void FONT_End();
void FONT_DrawChar(vec2 p0, vec2 p1, unsigned char ch);
void FONT_DrawString(vec2 p0, const char string[]);

#endif
