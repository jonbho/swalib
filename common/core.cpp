#include "stdafx.h"
#include "core.h"

//#############################################################################
// General
float CORE_FRand(float from, float to)
{
  return from + (to-from)*(rand()/(float)RAND_MAX);
}

//-----------------------------------------------------------------------------
void CORE_RenderSprite(vec2 p0, vec2 p1, GLuint texid)
{
  glBindTexture( GL_TEXTURE_2D, texid );
  glBegin( GL_QUADS );
  glTexCoord2d(0.0,0.0); glVertex2f(p0.x, p0.y);
  glTexCoord2d(1.0,0.0); glVertex2f(p1.x, p0.y);
  glTexCoord2d(1.0,1.0); glVertex2f(p1.x, p1.y);
  glTexCoord2d(0.0,1.0); glVertex2f(p0.x, p1.y);
  glEnd();
}

//-----------------------------------------------------------------------------
void CORE_RenderCenteredSprite(vec2 pos, vec2 size, GLuint texid)
{
  vec2 p0 = vsub(pos, vscale(size, .5f));
  vec2 p1 = vadd(pos, vscale(size, .5f));

  glBindTexture( GL_TEXTURE_2D, texid );
  glBegin( GL_QUADS );
  glTexCoord2d(0.0,0.0); glVertex2f(p0.x, p0.y);
  glTexCoord2d(1.0,0.0); glVertex2f(p1.x, p0.y);
  glTexCoord2d(1.0,1.0); glVertex2f(p1.x, p1.y);
  glTexCoord2d(0.0,1.0); glVertex2f(p0.x, p1.y);
  glEnd();
}

//-----------------------------------------------------------------------------
void CORE_RenderCenteredRotatedSprite(vec2 pos, vec2 size, float angle, GLuint texid, rgba_t color)
{
#if 0 // MANUAL
  // Rotated axes
  vec2 u = vscale(vunit(angle), .5f * size.x);
  vec2 v = vscale(vunit(angle + M_PIf/2.f), .5f * size.y);

  vec2 p0 = vsub(pos, vadd(u, v));
  vec2 p1 = vadd(pos, vsub(u, v));
  vec2 p2 = vadd(pos, vadd(u, v));
  vec2 p3 = vadd(pos, vsub(v, u));

  glBindTexture( GL_TEXTURE_2D, texid );
  glBegin( GL_QUADS );
  glColor4f(GETR(color)/255.f, GETG(color)/255.f, GETB(color)/255.f, GETA(color)/255.f);
  glTexCoord2d(0.0,0.0); glVertex2f(p0.x, p0.y);
  glTexCoord2d(1.0,0.0); glVertex2f(p1.x, p1.y);
  glTexCoord2d(1.0,1.0); glVertex2f(p2.x, p2.y);
  glTexCoord2d(0.0,1.0); glVertex2f(p3.x, p3.y);
  glEnd();

  glColor4f(1.f, 1.f, 1.f, 1.f);
#else
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(pos.x, pos.y, 0.f);
  glScalef(size.x, size.y, 1.f);
  glRotatef(RAD2DEG(angle), 0.f, 0.f, 1.f);

  glBindTexture( GL_TEXTURE_2D, texid );

  glBegin( GL_QUADS );
  glColor4f(GETR(color)/255.f, GETG(color)/255.f, GETB(color)/255.f, GETA(color)/255.f);
  glTexCoord2d(0.0,0.0); glVertex2f(-0.5f, +.5f);
  glTexCoord2d(1.0,0.0); glVertex2f(+0.5f, +.5f);
  glTexCoord2d(1.0,1.0); glVertex2f(+0.5f, -.5f);
  glTexCoord2d(0.0,1.0); glVertex2f(-0.5f, -.5f);
  glEnd();

  glPopMatrix();
  glColor4f(1.f, 1.f, 1.f, 1.f);
#endif
}

//#############################################################################
// BMP files
struct CORE_BMPFileHeader
{
  byte  mark[2];
  byte  filesize[4];
  byte  reserved[4];
  byte  pixdataoffset[4];

  byte  hdrsize[4];
  byte  width[4];
  byte  height[4];
  byte  colorplanes[2];
  byte  bpp[2];
  byte  compression[4];
  byte  pixdatasize[4];
  byte  hres[4];
  byte  vres[4];
  byte  numcol[4];
  byte  numimportantcolors[4];
};

//-----------------------------------------------------------------------------
#define READ_LE_WORD(a)  (a[0] + a[1]*0x100)
#define READ_LE_DWORD(a) (a[0] + a[1]*0x100 + a[2]*0x10000 + a[3]*0x1000000)

//-----------------------------------------------------------------------------
static GLuint CreateOpenGLTextureFromData(byte *pixdata, int width, int height, bool wrap, bool bgra = false)
{
  GLuint texid = 1;

  glGenTextures( 1, &texid );
  glBindTexture( GL_TEXTURE_2D, texid );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); // GL_LINEAR_MIPMAP_NEAREST
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // GL_LINEAR);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

  //gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA8, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixloadbuffer );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, abs((int)height), 0, (bgra ? GL_BGRA_EXT : GL_RGBA), GL_UNSIGNED_BYTE, pixdata);
  return texid;
}

//-----------------------------------------------------------------------------
static byte pixloadbuffer[512 * 512 * 4];
GLuint CORE_LoadBmp(const char filename[], bool wrap)
{
  GLint              retval = -1;
  CORE_BMPFileHeader hdr;
  
  int fd = open(filename, O_RDONLY);
  if (fd != -1)
  {
    read(fd, &hdr, sizeof(hdr));

    if (hdr.mark[0] == 'B' && hdr.mark[1] == 'M')
    {
      dword width  = READ_LE_DWORD(hdr.width);
      sdword height = READ_LE_DWORD(hdr.height);
      dword offset = READ_LE_DWORD(hdr.pixdataoffset);
        
      dword pixdatasize = READ_LE_DWORD(hdr.pixdatasize);
      if (!pixdatasize)
          pixdatasize = (width * abs(height) * READ_LE_WORD(hdr.bpp) / 8);

      lseek(fd, offset, SEEK_SET);
      if (height > 0)
        read(fd, pixloadbuffer, pixdatasize);
      else
      {
        // Reverse while loading
        int nrows = -height;
        for (int i = 0; i < nrows; i++)
          read(fd, pixloadbuffer + (nrows-i-1) * width * 4, (pixdatasize / nrows));
      }
        
      retval = CreateOpenGLTextureFromData(pixloadbuffer, width, height, wrap, true);
    }
    close(fd);
  }

  return retval;
}

//-----------------------------------------------------------------------------
GLuint CORE_LoadPNG(const char filename[], bool wrap)
{
  // Load and decode image.
  std::vector<unsigned char> image_data;
  unsigned width, height;
  unsigned error = lodepng::decode(image_data, width, height, filename, LCT_RGBA);

  // Blank texture if error
  if(error != 0)
    return 0;
    
  // Make image upside down to follow original convention
  std::vector<unsigned char> reversed_image_data;
  reversed_image_data.resize(image_data.size());
  for (unsigned i = 0; i < height; i++)
  {
    memcpy(reversed_image_data.data() + i * width * 4, image_data.data() + (height-i-1) * width * 4, width * 4);
  }

  // Create OpenGL texture
  return CreateOpenGLTextureFromData(reversed_image_data.data(), width, height, wrap);
}

//-----------------------------------------------------------------------------
void CORE_UnloadBmp(GLuint texid)
{
  glDeleteTextures( 1, &texid );
}
      
//-----------------------------------------------------------------------------
void CORE_UnloadPNG(GLuint texid)
{
  glDeleteTextures( 1, &texid );
}
//#############################################################################
#ifdef SWALIB_SOUND
// Sound (OpenAL, WAV files), leave last one for music!
#define MAX_SND_SOURCES 4
ALuint sndsources[MAX_SND_SOURCES];
int    nextsource = 0;

//-----------------------------------------------------------------------------
bool CORE_InitSound()
{
  ALCcontext *context;
  ALCdevice *device;
 
  device = alcOpenDevice(NULL);
  if (device)
  {
    context = alcCreateContext(device,NULL);
    alcMakeContextCurrent(context);
    alGenSources(MAX_SND_SOURCES, sndsources); 
    alGetError();
    return true;
  }
  else
    return false;
 
}

//-----------------------------------------------------------------------------
void CORE_EndSound()
{
  ALCdevice *device;
  ALCcontext *context;

  context = alcGetCurrentContext();
  device = alcGetContextsDevice(context);

  alDeleteSources(MAX_SND_SOURCES, sndsources);
   
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
}

//-----------------------------------------------------------------------------
void CORE_PlaySound(ALuint snd, float volume, float pitch)
{
  // Stop and play!
  alSourceStop(sndsources[nextsource]);
  alSourcei(sndsources[nextsource], AL_BUFFER, snd);
  alSourcef(sndsources[nextsource], AL_GAIN,   volume);
  alSourcef(sndsources[nextsource], AL_PITCH,  pitch);
  alSourcePlay(sndsources[nextsource]);

  nextsource++;
  if (nextsource == MAX_SND_SOURCES-1)
    nextsource = 0;
}

//-----------------------------------------------------------------------------
void CORE_PlayMusic (uint snd, float volume /*= 1.f*/)
{
  alSourceStop(sndsources[MAX_SND_SOURCES-1]);
  alSourcei(sndsources[MAX_SND_SOURCES-1], AL_BUFFER, snd);
  alSourcef(sndsources[MAX_SND_SOURCES-1], AL_GAIN,   volume);
  alSourcei(sndsources[MAX_SND_SOURCES-1], AL_LOOPING, AL_TRUE);
  alSourcePlay(sndsources[MAX_SND_SOURCES-1]);
}
  
//-----------------------------------------------------------------------------
void CORE_StopMusic()
{
  alSourceStop(sndsources[MAX_SND_SOURCES-1]);
}

//-----------------------------------------------------------------------------
struct CORE_RIFFHeader
{
  byte  chunkID[4];   // 'RIFF'
  byte  chunkSize[4];
  byte  format[4];    // 'WAVE'
};

struct CORE_RIFFChunkHeader
{
  byte  subChunkID[4];
  byte  subChunkSize[4];
};

struct CORE_WAVEFormatChunk
{
  byte  audioFormat[2];
  byte  numChannels[2];
  byte  sampleRate[4];
  byte  byteRate[4];
  byte  blockAlignp[2];
  byte  bitsPerSample[2];
};

//-----------------------------------------------------------------------------
#define MAX_WAV_SIZE 32*1024*1024 // Max 32Mb sound!
static byte soundloadbuffer[MAX_WAV_SIZE];
ALuint CORE_LoadWav(const char filename[])
{
  ALuint          retval = UINT_MAX;
  CORE_RIFFHeader hdr;
  
  int fd = open(filename, O_RDONLY);
  if (fd != -1)
  {
    read(fd, &hdr, sizeof(hdr));

    if (  hdr.chunkID[0] == 'R' && hdr.chunkID[1] == 'I' && hdr.chunkID[2] == 'F' && hdr.chunkID[3] == 'F'
       && hdr.format [0] == 'W' && hdr.format [1] == 'A' && hdr.format [2] == 'V' && hdr.format [3] == 'E' )
    {
      CORE_WAVEFormatChunk fmt;
      memset(&fmt, 0, sizeof(fmt));

      for (;;)
      {
        CORE_RIFFChunkHeader chunkhdr;
        if (read(fd, &chunkhdr, sizeof(chunkhdr)) < sizeof(chunkhdr))
          break;

        dword chunkdatasize = READ_LE_DWORD(chunkhdr.subChunkSize);

        if (chunkhdr.subChunkID[0] == 'f' && chunkhdr.subChunkID[1] == 'm' && chunkhdr.subChunkID[2] == 't' && chunkhdr.subChunkID[3] == ' ')
        {
          read(fd, &fmt, sizeof(fmt));
          lseek(fd, ((1 + chunkdatasize) & -2) - sizeof(fmt), SEEK_CUR); // Skip to next chunk
        }
        else if (chunkhdr.subChunkID[0] == 'd' && chunkhdr.subChunkID[1] == 'a' && chunkhdr.subChunkID[2] == 't' && chunkhdr.subChunkID[3] == 'a')
        {
          dword contentsize = chunkdatasize;
          if (contentsize > sizeof(soundloadbuffer))
            contentsize = sizeof(soundloadbuffer);

          read(fd, soundloadbuffer, contentsize);

          bool valid = true;
          ALsizei al_size = contentsize;
          ALsizei al_frequency = READ_LE_DWORD(fmt.sampleRate);
          ALenum al_format = (ALenum)-1;
          if (READ_LE_WORD(fmt.numChannels) == 1)
          {
            if      (READ_LE_WORD(fmt.bitsPerSample) == 8 ) al_format = AL_FORMAT_MONO8;
            else if (READ_LE_WORD(fmt.bitsPerSample) == 16) al_format = AL_FORMAT_MONO16;
            else                                            valid = false;
          }
          else if (READ_LE_WORD(fmt.numChannels) == 2)
          {
            if      (READ_LE_WORD(fmt.bitsPerSample) == 8 ) al_format = AL_FORMAT_STEREO8;
            else if (READ_LE_WORD(fmt.bitsPerSample) == 16) al_format = AL_FORMAT_STEREO16;
            else                                            valid = false;
          }

          if (valid)
          {
            ALuint al_buffer = UINT_MAX;
            alGenBuffers(1, &al_buffer);

            alBufferData(al_buffer, al_format, soundloadbuffer, al_size, al_frequency);
            retval = al_buffer;
          }
          break;
        }
        else
          lseek(fd, ((1 + READ_LE_DWORD(chunkhdr.subChunkSize)) & -2), SEEK_CUR); // Skip to next chunk
      }
    }
    close(fd);
  }

  return retval;
}

//-----------------------------------------------------------------------------
void CORE_UnloadWav(ALuint snd)
{
  alDeleteBuffers(1, &snd);
}

#endif //SWALIB_SOUND
