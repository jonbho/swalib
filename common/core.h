#ifndef _CORE_H_
#define _CORE_H_

//-----------------------------------------------------------------------------
typedef unsigned       char byte;
typedef unsigned short int  word;
typedef unsigned       int  dword;
typedef                int  sdword;

typedef unsigned       int  uint;

//-----------------------------------------------------------------------------
struct rgba_t { byte a, r, g, b; };
inline rgba_t rgbamake(byte r, byte g, byte b, byte a)
  { rgba_t c; c.r = r; c.g = g; c.b = b; c.a = a; return c; }
#define RGBA(rrr,ggg,bbb,aaa) rgbamake(rrr,ggg,bbb,aaa)
#define GETA(_rgba_) _rgba_.a
#define GETR(_rgba_) _rgba_.r
#define GETG(_rgba_) _rgba_.g
#define GETB(_rgba_) _rgba_.b

//-----------------------------------------------------------------------------
#define SIZE_ARRAY(__a__) (sizeof(__a__)/sizeof(__a__[0]))

//-----------------------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PIf 3.14159265358979323846f

//-----------------------------------------------------------------------------

#define RAD2DEG(_rad_) (360.f * (_rad_) / (2.f * M_PIf))
#define DEG2RAD(_deg_) ((2.f * M_PIf) * (_deg_) / 360.f)

//-----------------------------------------------------------------------------
struct vec2 { float x, y; };
inline vec2  vmake (float x, float y) { vec2 r; r.x = x; r.y = y; return r; }
inline vec2  vadd  (vec2 v1, vec2 v2) { return vmake(v1.x + v2.x, v1.y + v2.y); }
inline vec2  vsub  (vec2 v1, vec2 v2) { return vmake(v1.x - v2.x, v1.y - v2.y); }
inline vec2  vscale(vec2 v, float f)  { return vmake(v.x * f, v.y * f); }
inline float vlen2 (vec2 v)           { return v.x * v.x + v.y * v.y; }
inline float vlen  (vec2 v)           { return (float)sqrt(vlen2(v)); }
inline float vdot  (vec2 v1, vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }
inline vec2  vunit (float angle)      { return vmake((float)cos(angle), (float)sin(angle)); }

//-----------------------------------------------------------------------------
struct vec3 { float x, y, z; };
inline vec3  v3make (float x, float y, float z) { vec3 r; r.x = x; r.y = y; r.z = z; return r; }
inline vec3  v3add  (vec3 v1, vec3 v2) { return v3make(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline vec3  v3sub  (vec3 v1, vec3 v2) { return v3make(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline vec3  v3scale(vec3 v, float f)  { return v3make(v.x * f, v.y * f, v.z * f); }
inline float v3len2 (vec3 v)           { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline float v3len  (vec3 v)           { return (float)sqrt(v3len2(v)); }
inline float v3dot  (vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

//-----------------------------------------------------------------------------
float  CORE_FRand     (float from, float to);

GLuint CORE_LoadBmp   (const char filename[], bool wrap);
GLuint CORE_LoadPNG   (const char filename[], bool wrap);
void   CORE_UnloadBmp (GLuint texid);
void   CORE_UnloadPNG (GLuint texid);
void   CORE_RenderSprite(vec2 p0, vec2 p1, GLuint texid);
void   CORE_RenderCenteredSprite(vec2 pos, vec2 size, GLuint texid);
void   CORE_RenderCenteredRotatedSprite(vec2 pos, vec2 size, float angle, GLuint texid, rgba_t color = RGBA(255,255,255,255));

#ifdef SWALIB_SOUND
bool CORE_InitSound();
void CORE_EndSound();
uint CORE_LoadWav     (const char filename[]);
void CORE_PlaySound (uint snd, float volume, float pitch);
void CORE_UnloadWav (uint snd);
void CORE_PlayMusic (uint snd, float volume = 1.f);
void CORE_StopMusic ();
#endif //SWALIB_SOUND

#endif
