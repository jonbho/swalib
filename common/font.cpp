#include "stdafx.h"
#include "sys.h"
#include "core.h"

//=============================================================================
// Font support
#define FONT_ROWS 8
#define FONT_COLS 8
#define MAX_FONT_CHARDEFS 256
#define FONT_CHAR_WIDTH (16.f/128.f)
#define FONT_CHAR_HEIGHT (16.f/128.f)

GLuint FONT_Tex;
char FONT_Def[FONT_ROWS][FONT_COLS+1] =
{
  { " !\"~*%-'" },
  { "   +, ./" },
  { "01234567" },
  { "89:;aib?" },
  { "*ABCDEFG" },
  { "HIJKLMNO" },
  { "PQRSTUVW" },
  { "XYZ     " },
};

struct FontCharDef
{
  char ch;
  vec2 p0;
};

FontCharDef fontchardefs[MAX_FONT_CHARDEFS] = { 0 };

//-----------------------------------------------------------------------------
void FONT_Init()
{
  // Load texture
  FONT_Tex = CORE_LoadPNG("data/kromasky.png", false);

  // Preapre coords
  for (int i = 0; i < FONT_ROWS; i++) // One iteration per row
  {
    for (int j = 0; j < FONT_COLS; j++) // Inside row
    {
      unsigned char ch = FONT_Def[i][j];
      fontchardefs[ch].ch = ch;
      fontchardefs[ch].p0 = vmake(j * FONT_CHAR_WIDTH, (FONT_ROWS - i - 1) * FONT_CHAR_HEIGHT);
    }
  }
}

//-----------------------------------------------------------------------------
void FONT_End()
{
  CORE_UnloadPNG(FONT_Tex);
}

//-----------------------------------------------------------------------------
void FONT_DrawChar(vec2 p0, vec2 p1, unsigned char ch)
{
  if ((int)ch < MAX_FONT_CHARDEFS && fontchardefs[ch].ch == ch)
  {
    glBindTexture( GL_TEXTURE_2D, FONT_Tex );
    glBegin( GL_QUADS );
    glTexCoord2d(fontchardefs[ch].p0.x                  , fontchardefs[ch].p0.y                   ); glVertex2f(p0.x, p0.y);
    glTexCoord2d(fontchardefs[ch].p0.x + FONT_CHAR_WIDTH, fontchardefs[ch].p0.y                   ); glVertex2f(p1.x, p0.y);
    glTexCoord2d(fontchardefs[ch].p0.x + FONT_CHAR_WIDTH, fontchardefs[ch].p0.y + FONT_CHAR_HEIGHT); glVertex2f(p1.x, p1.y);
    glTexCoord2d(fontchardefs[ch].p0.x                  , fontchardefs[ch].p0.y + FONT_CHAR_HEIGHT); glVertex2f(p0.x, p1.y);
    glEnd();
  }
}

//-----------------------------------------------------------------------------
void FONT_DrawString(vec2 p0, const char string[])
{
  int n = (int)strlen(string);
  for (int i = 0; i < n; i++)
  {
    FONT_DrawChar(p0, vadd(p0, vmake(16.f, 16.f)), string[i]);
    p0 = vadd(p0, vmake(16.f, 0.f));
  }
}
