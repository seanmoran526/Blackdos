#include "blackdos.h"

int strCmp(char*, char*);

void main()
{
    char buffer[12288];
    char dir[512];
    char command[150];
    char* str1, str2;
    int length;
    PRINTS("Welcome to Blackdos \r\n\0");
    while(1)
    {
        interrupt(33,2,dir,257,0);
        while(1)
        {
            PRINTS("^(~(oo)~)^:  \0");
            SCANS(command);
            PRINTS("\r\n\0");
            if(input[4] != ' ' || input[4] != '\0')
            {
            	break;
            }
            command[4] = '\0';
            str1 = &command[5];
            if(strCmp(command, "copy\0"))
            {
                PRINTS("Copy Works \r\n\0");   
            }
            else if(strCmp(command, "exec\0"))
            {
                PRINTS("Exec Works \r\n\0"); 
            }
            else if(strCmp(command, "prnt\0"))
            {
                PRINTS("Prnt Works \r\n\0"); 
            }      
            else if(strCmp(command, "remv\0"))
            {
                PRINTS("Remv Works \r\n\0"); 
            }
            else if(strCmp(command, "show\0"))
            {
                PRINTS("Show Works \r\n\0");    
            }
            else if(strCmp(command, "twet\0"))
            {
                PRINTS("Twet Works \r\n\0"); 
            }
            else if(strCmp(command, "boot\0"))
            {
                PRINTS("boot Works \r\n\0"); 
            }
            else if(strCmp(command, "clrs\0"))
            {
                PRINTS("Clrs Works \r\n\0"); 
            }      
            else if(strCmp(command, "ddir\0"))
            {
                PRINTS("ddir Works \r\n\0"); 
            }
            else if(strCmp(command, "echo\0"))
            {
                PRINTS("echo Works \r\n\0"); 
            }
            else if(strCmp(command, "help\0"))
            {
                PRINTS("Echo Works \r\n\0"); 
            }
            else if(strCmp(command, "senv\0"))
            {
                PRINTS("Senv Works \r\n\0"); 
            }
            else
            {
                break;
            }

        }
        PRINTS("Invalid Input \r\n\0");
    }
}

int strCmp(char* a, char* b)
{
  int i = 0;
  int same = 1;
  while(a[i] != '\0')
  {
    if(a[i] != b[i])
    {
      same = 0;
      break;
    }
    ++i;
  }
  return same;
}


