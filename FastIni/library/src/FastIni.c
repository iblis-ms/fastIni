#include "FastIni.h"

#include <string.h>
#include <stdio.h>


#ifdef _WIN32
#define LINE_END_SIZE 2

static int newLineFirstChar(char c)
{
  return c == '\r';
}

#else

#define LINE_END_SIZE 1

static int newLineFirstChar(char c)
{
  return c == '\n';
}

#endif

static int isAlphaNumeric(char aChar)
{
  return (aChar >= '0' && aChar <= '9') || (aChar >= 'a' && aChar <= 'z') || (aChar >= 'A' && aChar <= 'Z');
}

typedef enum
{
  NONE,
  SECTION,
  KEY,
  VALUE,
  COMMENT
} TBufferType;

int parse(char *aBufferBeg, char *aEntireBufferEnd, char **aOutputBuffer, TBufferType *aOutputBufferType, DParseListener aListener)
{
  int itemSize;
  int returnValue;
  char *buffer = aBufferBeg;

  if(*aBufferBeg == '[')
  {
    *aOutputBuffer = aBufferBeg;
    *aOutputBufferType = SECTION;
  }
  if(*aOutputBufferType == SECTION)
  {
    for (; *buffer != '\n' && buffer < aEntireBufferEnd; ++buffer)
    {}
    if(buffer >= aEntireBufferEnd)
    {
      return (int)(buffer - aBufferBeg);
    }

    itemSize = (int)(buffer - *aOutputBuffer);
    returnValue = aListener(*aOutputBuffer + 1, itemSize - 2, NULL, -1, NULL, -1);
    if (returnValue < 0)
    {
      return returnValue;
    }
    *aOutputBuffer = NULL;
    *aOutputBufferType = NONE;
    return itemSize + LINE_END_SIZE;
  }

  if(*aBufferBeg == ';' && *aOutputBufferType != COMMENT)
  {
    *aOutputBuffer = aBufferBeg;
    *aOutputBufferType = COMMENT;
  }
  if(*aOutputBufferType == COMMENT)
  {
    for (; *buffer != '\n' && buffer < aEntireBufferEnd; ++buffer)
    {}
    if(buffer >= aEntireBufferEnd)
    {
      return (int)(buffer - aBufferBeg);
    }
    *aOutputBuffer = NULL;
    *aOutputBufferType = NONE;
    return (int)(buffer - aBufferBeg + LINE_END_SIZE);
  }

  if(isAlphaNumeric(*aBufferBeg) && *aOutputBufferType != VALUE && *aOutputBufferType != KEY)
  {
    *aOutputBuffer = aBufferBeg;
    *aOutputBufferType = KEY;
  }
  if(*aOutputBufferType == KEY)
  {
    for (; *buffer != '=' && buffer < aEntireBufferEnd; ++buffer)
    {}
    if(buffer >= aEntireBufferEnd)
    {
      return (int)(buffer - aBufferBeg);
    }
    itemSize = (int)(buffer - *aOutputBuffer);
    returnValue = aListener(NULL, -1, *aOutputBuffer, itemSize, NULL, -1);
    if (returnValue < 0)
    {
      return returnValue;
    }
    *aOutputBuffer = buffer + 1;
    *aOutputBufferType = VALUE;

    return itemSize + 1;

  }
  if(*aOutputBufferType == VALUE)
  {
    for (; *buffer != '\n' && buffer < aEntireBufferEnd; ++buffer)
    {}
    if(buffer >= aEntireBufferEnd)
    {

      return (int)(buffer - aBufferBeg);
    }
    itemSize = (int)(buffer - *aOutputBuffer);
    returnValue = aListener(NULL, -1, NULL, -1, *aOutputBuffer, itemSize);
    if (returnValue < 0)
    {
      return returnValue;
    }
    *aOutputBuffer = NULL;
    *aOutputBufferType = NONE;
    return itemSize + LINE_END_SIZE;
  }
  if (newLineFirstChar(*buffer))
  {
    return LINE_END_SIZE;
  }
  return 0;
}


int fastIniParse(const char *aPath, char *aBufferBeg, size_t aBufferSize, DParseListener aListener)
{
  FILE *file;
  int readBytes;
  size_t toReadBytes = aBufferSize;
  char* endBuffer;
  int moveSize = 0;
  char *itemBufferBeg = NULL;
  TBufferType aOutputBufferType = NONE;

  char *buffer = aBufferBeg;
  char *bufferEnd = aBufferBeg + aBufferSize;

  if((file = fopen(aPath, "r")) == NULL)
  {
    return -1;
  }
  size_t totalRead = 0;
  while ((readBytes = fread(buffer, sizeof(char), toReadBytes, file)) != 0u)
  {
    totalRead += readBytes;
    endBuffer = buffer + readBytes;
    buffer = aBufferBeg;
    do
    {
      moveSize = parse(buffer, endBuffer, &itemBufferBeg, &aOutputBufferType, aListener);
      buffer += moveSize;
    }
    while (buffer < endBuffer && moveSize >= 0);

    if (moveSize < 0)
    {
      // user's listener returns error message.
      fclose(file);
      return moveSize;
    }
    if (aOutputBufferType == NONE)
    {
      // when buffer ends with the end of value
      buffer = aBufferBeg;
      toReadBytes = aBufferSize;
      continue;
    }
    else if (itemBufferBeg >= bufferEnd)
    {
      // when buffer ends with the end of value
      buffer = aBufferBeg;
      toReadBytes = aBufferSize;
      itemBufferBeg = aBufferBeg;
      continue;
    }
    buffer -= moveSize;
    memcpy(aBufferBeg, buffer, (size_t)moveSize);
    buffer = aBufferBeg + moveSize;

    toReadBytes = aBufferSize - moveSize;
    itemBufferBeg = aBufferBeg;
  }

  return fclose(file);
}
