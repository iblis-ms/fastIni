#include <stdio.h>
#include <string.h>

#include "FastIni.h"

void displayInt(const char* aFormat, char *begin, int size)
{
  char test[64] = {'\0'};
  memcpy(test, begin, size);
  printf(aFormat, test);
}

int listener(char* aSectionBeg, int aSectionSize, char* aKeyBeg, int aKeySize, char* aValueBeg, int aValueSize)
{
  if (aSectionBeg)
  {
    displayInt("Section: %s\n", aSectionBeg, aSectionSize);
  }
  else if (aKeyBeg)
  {
    displayInt("Key: %s\n", aKeyBeg, aKeySize);
  }
  else if(aValueBeg)
  {
    displayInt("Value: %s\n", aValueBeg, aValueSize);
  }
  else
  {
    printf("End");
  }
  return 0;
}

int main(int aArgc, char **aArgv)
{
  const int size = 3200;
  char buffer[3200];
  fastIniParse("test.ini", buffer, size, listener);
  return 0;
}



