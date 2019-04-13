/* ACADEMIC INTEGRITY PLEDGE                                              */
/*                                                                        */
/* - I have not used source code obtained from another student nor        */
/*   any other unauthorized source, either modified or unmodified.        */
/*                                                                        */
/* - All source code and documentation used in my program is either       */
/*   my original work or was derived by me from the source code           */
/*   published in the textbook for this course or presented in            */
/*   class.                                                               */
/*                                                                        */
/* - I have not discussed coding details about this project with          */
/*   anyone other than my instructor. I understand that I may discuss     */
/*   the concepts of this program with other students and that another    */
/*   student may help me debug my program so long as neither of us        */
/*   writes anything during the discussion or modifies any computer       */
/*   file during the discussion.                                          */
/*                                                                        */
/* - I have violated neither the spirit nor letter of these restrictions. */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Signed:  Sean Moran    Date:  5 Apr 19     */
/*                                                                        */
/*                                                                        */
/* 3460:4/526 BlackDOS2020 kernel, Version 1.01, Spring 2018.             */

/* Interrupt 33 function constants */
int PRINTSTR = 0;
int READSTR = 1;
int WRITEINT = 13;
int READINT = 14;
int READSECT = 2;
int WRITESECT = 6;
int CLRSCREEN = 12;
/* Interrupts */
int OP_SCREEN = 16;
int OP_PRINTER = 23;
int IP_KEY = 22;
int RW_SECT = 19;

/* Prototypes */
void handleInterrupt21(int,int,int,int);
void printLogo();
void readString(char*);
void readInt(int*);
void printString(char* c, int d);
void writeInt(int, int);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void writeSector(char*,int);
int strCmp(char*, char*);
void clearScreen(int,int);
void error(int);
void readFile(char*, char*, int*);
void writeFile(char*, char*, int);
void deleteFile(char*);
void runProgram(char*,int);
void stop();

void main()
{
 char buffer[512];
 makeInterrupt21();
 interrupt(33,2,buffer,258,0);
 interrupt(33,12,buffer[0]+1,buffer[1]+1,0);
 printLogo();
 interrupt(33,PRINTSTR,"\r\n\r\n\0",0,0);
 interrupt(33,PRINTSTR,"Press Enter to Begin: \0",0,0);
 interrupt(33,1,buffer,0,0);
 interrupt(33,12,0,0,0);
 interrupt(33,PRINTSTR,"Welcome to Blackdos \r\n\0",0,0);
 interrupt(33,4, "Shell\0" ,2,0);
 interrupt(33,0,"Bad or missing command interpreter.\r\n\0",0,0);
 while (1) ;
}
void printString(char* chArr, int printer)
{
    /* Defaults to screen output interrupt 16 */
    int opDest = OP_SCREEN;
    int i;
    int offset = 14*256;
    if(printer==1)
    {
        opDest = OP_PRINTER;
        offset = 0;
    }
    for(i=0; chArr[i] != '\0'; ++i)
    {
        interrupt(opDest, (offset + chArr[i]), 0, 0, 0);
    }
}

void printLogo()
{
   interrupt(33,0,"       ___   `._   ____  _            _    _____   ____   _____ \r\n\0",0,0);
   interrupt(33,0,"      /   \\__/__> |  _ \\| |          | |  |  __ \\ / __ \\ / ____|\r\n\0",0,0);
   interrupt(33,0,"     /_  \\  _/    | |_) | | __ _  ___| | _| |  | | |  | | (___ \r\n\0",0,0);
   interrupt(33,0,"    // \\ /./      |  _ <| |/ _` |/ __| |/ / |  | | |  | |\\___ \\ \r\n\0",0,0);
   interrupt(33,0,"   //   \\\\        | |_) | | (_| | (__|   <| |__| | |__| |____) |\r\n\0",0,0);
   interrupt(33,0,"._/'     `\\.      |____/|_|\\__,_|\\___|_|\\_\\_____/ \\____/|_____/\r\n\0",0,0);
   interrupt(33,0," BlackDOS2020 v. 1.01, c. 2018. Based on a project by M. Black. \r\n\0",0,0);
   interrupt(33,0," Author(s): Sean Moran.\r\n\r\n\0",0,0);
}

/* MAKE FUTURE UPDATES HERE */
/* VVVVVVVVVVVVVVVVVVVVVVVV */
void readString(char* input)
{
    int i = 0;
    int offset = 14*256;
    char key;
    do
    {
        key = interrupt(IP_KEY,0,0,0,0);
        interrupt(OP_SCREEN, (offset + key), 0, 0, 0);
        if(key == '\b')
        {
            if(i > 0)
            {
                interrupt(OP_SCREEN, (offset + ' '), 0, 0, 0);
                interrupt(OP_SCREEN, (offset + '\b'), 0, 0, 0);
                --i;
            }
        }
        else
        {
            if(key!='\n' && key!='\r')
            {
                  input[i] = key;
                   ++i;
            }
    }
}while(key != '\r' );
    input[i] = '\0';
    interrupt(33,PRINTSTR,"\r\n\0",0,0);
}

void readInt(int* n)
{
    int i = 0;
    char num[6];
    readString(num);
    *n = 0;
    while(num[i] != 0 && i<6)
    {
        *n = (*n) * 10 + num[i] - '0';
        ++i;
    }
}

void writeInt(int n, int opSelection)
{
    char num[6];
    int remainder;
    int i = 4;
    num[5] = 0x0;
    while(n > 0)
    {
        remainder = mod(n,10);
        n = div(n,10);
        num[i] = remainder + '0';
        --i;
    }
    ++i;
    printString(&num[i], opSelection);
}

int mod(int a, int b)
{
    int x = a;
    while (x >= b) x = x - b;
    return x;
}

int div(int a, int b)
{
    int q = 0;
    while (q * b <= a) q++;
    return (q - 1);
}

void readSector(char* buffer, int sector)
{
    int relSec = mod(sector, 18) + 1;
    int temp = div(sector,18);
    int head = mod(temp, 2);
    int track = div(sector, 36);
    int DX = head * 256;
    int CX = track*256+relSec;
    interrupt(19,513,buffer,CX,DX);
}

void writeSector(char* buffer, int sector)
{
    int relSec = mod(sector, 18) + 1;
    int temp = div(sector,18);
    int head = mod(temp, 2);
    int track = div(sector, 36);
    int DX = head * 256;
    int CX = track*256+relSec;
    interrupt(19,769,buffer,CX,DX);
}

void clearScreen(int back,int fore)
{
    char blanks[25];
    int i;
    blanks[25] = '\0';
    for(i=0; i<24; ++i)
    {
        blanks[i] = "\r\n";
    }
    printString(blanks, 0);
    interrupt(16,512,0,0,0);
    if(back>0 && fore>0)
    {
        if(back<10 && fore<18)
        {
            interrupt(16, 1536, 4096*(back-1)+256*(fore-1), 0, 6223);
        }
    }
}

int strCmp(char* a, char* b)
{
  int i = 0;
  int boolFlag = 1;
  while(a[i] != '\0')
  {
    if(a[i] != b[i])
    {
      boolFlag = 0;
      break;
    }
    ++i;
  }
  return boolFlag;
}

void readFile(char* fname, char* buffer, char* size)
{
    char dir[512];
    int i;
    int dirIndex = 0;
    int found =1;
    int bufIndex = 0;
    int boolFlag;
    readSector(dir, 257);
    while(bufindex < 512)
    {
      BoolFlag = strCmp(fname, &dir[bufindex]);
      if(boolFlag == 1)
      {
          dirIndex = bufindex + 8;
          for(dirIndex ; dir[dirIndex] != 0; ++dirIndex)
          {
              *size += 1;
              readSector(buffer, dir[dirIndex]);
              buffer += 512;
          }
          return;
      }
      bufIndex += 32;
    }
    interrupt(33,15,0,0,0);
}

void writeFile(char* fname,char* buffer,int numSect)
{
  char dir[512];
  char map[512];
  int boolFlag;
  int dirIndex = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  int m = 0;
  int free = 0;
  readSector(dir, 257);
  readSector(map, 256);
   while(i < 17)
   {
       if(dir[m] == 0x0 )
       {
           ++free;
       }
       m += 32;
       i += 1;
   }
   if(free == 0)
   {
       interrupt(33, 15, 3, 0, 0);
       return;
   }
   while(dirIndex < 512)
   {
       boolFlag = strCmp(fname, &dir[dirIndex]);
       if(dir[dirIndex] != 0x0 && boolFlag == 1 )
       {
           interrupt(33, 15, 1, 0, 0);
           return;
       }
       else if(dir[dirIndex] == 0)
       {
           break;
       }
       dirIndex += 32;
   }
   while(fname[j] != '\0')
   {
       dir[dirIndex + j] = fname[j];
       ++j;
   }

   for(j = dirIndex; j < (dirIndex + 8); ++j)
   {
       dir[j] == 0;
   }
   dirIndex += 8;

   for(j = 0; j < numSect; ++j)
   {
         while(k <  512)
         {
           if(map[k] == 0)
           {
               map[k] = 255;
               dir[dirIndex + j] = k;

               writeSector(buffer, k);
               break;
           }
         ++k;
       }
   }
   writeSector(dir, 257);
   writeSector(map, 256);
}

void deleteFile(char* fname)
{
   char dir[512];
   char map[512];
   int boolFlag;
   int i = 0;
   int j = 0;
   int bufIndex = 0;
   int mapIndex = 0;
   readSector(dir, 257);
   readSector(map, 256);
   while(i<16)
   {
     boolFlag = strCmp(fname, &dir[bufIndex]);
     if(boolFlag == 1)
     {
         for(j; j < 8; ++j)
         {
             dir[bufIndex + j] = 0;
         }
         for(j = bufIndex + 8; dir[j] != 0 ;++j)
         {
             mapIndex = dir[j];
             dir[j] = 0;
             map[mapIndex] = 0;
         }

         writeSector(dir, 257);
         writeSector(map, 256);
         return;
     }
     ++i;
   }
   interrupt(33, 15,0, 0, 0);
}

  void runProgram(char* fname, int segment)
  {
    char buffer[4000];
    int offset=0;
    readFile(fname, buffer, 1);
    for(offset = 0 ; offset<4000 ; ++offset)
    {
        putInMemory(segment*4096, offset, buffer[offset]);
    }
    launchProgram(segment*4096);
  }


void stop(){launchProgram(8192);}

void error(int bx)
{
   char errMsg0[18], errMsg1[17], errMsg2[13], errMsg3[17];

   errMsg0[0] = 70; errMsg0[1] = 105; errMsg0[2] = 108; errMsg0[3] = 101;
   errMsg0[4] = 32; errMsg0[5] = 110; errMsg0[6] = 111; errMsg0[7] = 116;
   errMsg0[8] = 32; errMsg0[9] = 102; errMsg0[10] = 111; errMsg0[11] = 117;
   errMsg0[12] = 110; errMsg0[13] = 100; errMsg0[14] = 46; errMsg0[15] = 13;
   errMsg0[16] = 10; errMsg0[17] = 0;
   errMsg1[0] = 66; errMsg1[1] = 97; errMsg1[2] = 100; errMsg1[3] = 32;
   errMsg1[4] = 102; errMsg1[5] = 105; errMsg1[6] = 108; errMsg1[7] = 101;
   errMsg1[8] = 32; errMsg1[9] = 110; errMsg1[10] = 97; errMsg1[11] = 109;
   errMsg1[12] = 101; errMsg1[13] = 46; errMsg1[14] = 13; errMsg1[15] = 10;
   errMsg1[16] = 0;
   errMsg2[0] = 68; errMsg2[1] = 105; errMsg2[2] = 115; errMsg2[3] = 107;
   errMsg2[4] = 32; errMsg2[5] = 102; errMsg2[6] = 117; errMsg2[7] = 108;
   errMsg2[8] = 108; errMsg2[9] = 46; errMsg2[10] = 13; errMsg2[11] = 10;
   errMsg2[12] = 0;
   errMsg3[0] = 71; errMsg3[1] = 101; errMsg3[2] = 110; errMsg3[3] = 101;
   errMsg3[4] = 114; errMsg3[5] = 97; errMsg3[6] = 108; errMsg3[7] = 32;
   errMsg3[8] = 101; errMsg3[9] = 114; errMsg3[10] = 114; errMsg3[11] = 111;
   errMsg3[12] = 114; errMsg3[13] = 46; errMsg3[14] = 13; errMsg3[15] = 10;
   errMsg3[16] = 0;

   switch(bx) {
   case 0: printString(errMsg0); break;
   case 1: printString(errMsg1); break;
   case 2: printString(errMsg2); break;
   default: printString(errMsg3);
   }
}


void handleInterrupt21(int ax, int bx, int cx, int dx)
{
   switch(ax)
   {
      case 0: printString(bx,cx); break;
      case 1: readString(bx); break;
      case 2: readSector(bx,cx); break;
      case 3: readFile(bx,cx,dx); break;
      case 4: runProgram(bx,cx); break;
      case 5: stop(); break;
      case 6: writeSector(bx,cx); break;
      case 7: deleteFile(bx); break;
      case 8: writeFile(bx,cx,dx); break;
      case 11: interrupt(25,0,0,0,0); break;
      case 12: clearScreen(bx,cx); break;
      case 13: writeInt(bx,cx); break;
      case 14: readInt(bx); break;
      case 15: error(bx); break;
      default: printString("General BlackDOS error.\r\n\0", 0);
   }
   stop();
}
