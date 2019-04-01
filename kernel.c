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
int findFile(char*, char*);
int openDirIndex(char*);
int openSector(char*);
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
 interrupt(33,4,"kitty1\0",2,0);
 interrupt(33,0,"Error if this executes.\r\n\0",0,0);
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

int openSector(char* map)
{
    int i;
    for(i=0; i<512; ++i)
    {
        if(map[i] == 0x00)
        {
            return i;
        }
    }
    return -1;
}


int openDirIndex(char* dir)
{
    int i, j;
    for(i=0; i<512; i+=32)
    {
        if(dir[i]=='\0')
        {
            for(j=1; j<8; ++j)
            {
                dir[i+j]= '\0';
            }
            return i;
        }
    }
    return -1;
}

int findFile(char* dir, char* fname)
{
    int i, j, found;
    for(i=0; i<512; i+=32)
    {
        found=1;
        for(j=0; j<8; ++j)
        {
            if(dir[i+j] != fname[j])
            {
                found = 0;
                break;
            }
        }
        if(found==1)
        {
            return i;
        }
    }
    return -1;
}

void readFile(char* fname, char* buffer, int* size)
{
    char dir[512];
    int dirIndex=0;
    int found=0;
    int secIndex=0;
    int bufIndex=0;
    readSector(dir, 257);
    while(dirIndex<16)
    {
        if(charComp(fname, &dir[dirIndex*32])>0)
        {
            secIndex = (dirIndex*32) + 8;
            while(dir[secIndex]!=0 && bufIndex<12288)
            {
                    readSector(&buffer[bufIndex], dir[secIndex]);
                    ++secIndex;
                    bufIndex+=512;
            }
            return;
        }
        ++dirIndex;
    }
    interrupt(33,15,0,0,0);
    return;
}

void writeFile(char* fname, char* buffer, int numSect)
{
    char dir[512];
    char map[512];
    int dirIndex=0;
    int openDir=-1;
    int k;
    int flag=1;
    int i = 0;
    int j = 0;
    int openSec=-1;
    int bufIndex=0;
    readSector(dir, 257);
    readSector(map, 256);
    while(dirIndex<16)
    {
        while(dir[dirIndex*32]==0)
        {
            ++dirIndex;
            if(openDir<0)
            {
                openDir=(dirIndex-1)*32;
                while(j<8)
                {
                    dir[openDir] = fname[j];
                    ++j;
                }
                openDir+=8;
            }
        }
        if(charComp(fname, &dir[dirIndex*32])<0)
        {
            interrupt(33,15,1,0,0);
            return;
        }
        ++dirIndex;
    }
    if(openDir<0)
    {
        interrupt(33,15,1,0,0);
        return;
    }

    bufIndex=0;
    do
    {
        while(map[i]==0 && i<511)
        {
            ++i;
        }
        if(i>511)
        {
            interrupt(33,15,2,0,0);
            flag=0;
        }
        else
        {
            dir[openDir]= i + 1;
            dir[openDir+1]= 0;
            dir[openDir+2]= 0;
            ++openDir;
            map[i]= 0xFF;
            writeSector(map, 256);
            writeSector(dir, 258);
            writeSector(&buffer[bufIndex], i+1);
            flag==0;
        }
    }while(flag==1);
    return;
}

void deleteFile(char* fname)
{
    int secIndex=0;
    int dirIndex=0;
    char dir[512];
    char map[512];
    readSector(dir, 257);
    readSector(map, 256);
    readSector(dir, 257);
    while(dirIndex<16)
    {
        if(charComp(fname, &dir[dirIndex*32])>0)
        {
            dir[0]=0;
            secIndex = (dirIndex*32) + 8;
            while(secIndex - dirIndex < 32)
            {
                    map[dir[secIndex]]=0x00;
                    dir[secIndex]=0x00;
                    ++secIndex;
            }
        }
        else
        {
            interrupt(33,15,0,0,0);
        }
    }
    return;
}

void runProgram(char* fname, int segment)
{
    char buffer[4000];
    int offset=0;
    int base = segment*4096;
    readFile(fname, buffer, 1);
    while(offset<4000)
    {
        putInMemory(base, offset, buffer[offset]);
        ++offset;
    }
    launchProgram(base);
}

void stop(){while(1);}

void error(int bx)
{
    switch(bx)
    {
        case 0: interrupt(33,PRINTSTR,"File not found. \r\n\0",0,0); break;
        case 1: interrupt(33,PRINTSTR,"Bad file name. \r\n\0",0,0); break;
        case 2: interrupt(33,PRINTSTR,"Disk full \r\n\0",0,0); break;
        default: interrupt(33,PRINTSTR,"General error \r\n\0",0,0);
    }
    stop();
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
      case 12: clearScreen(bx,cx); break;
      case 13: writeInt(bx,cx); break;
      case 14: readInt(bx); break;
      case 15: error(bx); break;
      default: printString("General BlackDOS error.\r\n\0", 0);
   }
}
