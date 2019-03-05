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
/* Signed:  Sean Moran    Date:  5 Feb 19     */
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
void readString(char*);
void writeSector(char*,int);
void clearScreen(int,int);
void error(int);
void readFile(char*, char*, int*);
void writeFile(char*, char*, int);
void deleteFile(char*);
void main()
{
    char buffer[512]; int i;
    makeInterrupt21();

    /* Step 0 –config file */
    interrupt(33,2,buffer,258,0);
    interrupt(33,12,buffer[0]+1,buffer[1]+1,0);
    printLogo();

    while (1);
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
    interrupt(RW_SECT,513,buffer,(track*256+relSec),(head*256));
}

void writeSector(char* buffer, int sector)
{
    int relSec = mod(sector, 18) + 1;
    int temp = div(sector,18);
    int head = mod(temp, 2);
    int track = div(sector, 36);
    interrupt(RW_SECT,769,buffer,(track*256+relSec),(head*256));
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
    interrupt(OP_SCREEN,512,0,0,0);
    if(back>0 && fore>0)
    {
        if(back<10 && fore<18)
        {
            interrupt(OP_SCREEN, 1536, 4096*(back-1)+256*(fore-1), 0, 6223);
        }
    }
}

void readFile(char* fname, char* buffer, int* size)
{
    char dir[512];
    readSector(dir, 257);
    int i=0;
    int foundIndex=-1;
    while(foundIndex<0 && i<512)
    {    
        int j=0;
        while(j<8 && dir[i++] != fname[j++]){}
        if(j==8)
            foundIndex = i;
        else
            i = i - j + 32;
    }
    if(foundIndex>0)
    {
        while(dir[foundIndex]>0 && foundIndex%32 != 0)
        {
            readSector(buffer, dir[foundIndex]);
            ++foundIndex;
            buffer += 512;
        }
    else
        interrupt(33, 15, 0, 0, 0);
    size = buffer/512;
    return;
}

void writeFile(char* fname, char* buffer, int numSect)
{
    char dir[512];
    char map[512];
    readSector(dir, 257);
    readSector(map, 256);
    int i=0;
    int j=0;
    int k;
    int openSec;
    while(j<16 && map[i++]==0xFF && dir[j++]==fname[0]){j+=31;}
    if(j%2 == 0)
    {
        if(j<16)
        {
            k=1;
            ++j;
            while(j<8 && dir[j] != fname[k]){++j; ++k;}
            if(k==8)
            {
                interrupt(33, 15, 1, 0, 0);
                return;
            }
        }
        else
        {
             while(i<512 && map[i]==0xFF){++i;}
                if(i==512)
                {
                    interrupt(33,15,2,0,0);
                    return;
                }
        }
    }
    
}

void deleteFile(char* fname)
{

}

void error(int bx)
{
    switch(bx)
    {
        case 0: interrupt(33,PRINTSTR,"File not found. \r\n\0",0,0); break;
        case 1: interrupt(33,PRINTSTR,"Bad file name. \r\n\0",0,0); break;
        case 2: interrupt(33,PRINTSTR,"Disk full \r\n\0",0,0); break;
        default: interrupt(33,PRINTSTR,"General error \r\n\0",0,0);
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
      case 6: writeSector(bx,cx); break;
      case 7: deleteFile(bx); break;
      case 8: writeFile(bx,cx,dx); break;
      case 12: clearScreen(bx,cx); break;
      case 13: writeInt(bx,cx); break;
      case 14: readInt(bx); break;
      case 15: error(bx); break;
      default: printString("General BlackDOS error.\r\n\0", 0);
   }
}
