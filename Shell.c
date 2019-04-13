#include "blackdos.h"

int strCmp(char*, char*);
int isUpper(char*);
int getLength(char*);
int getSize(char*);
void main()
{
    char buffer[12288];
    char dir[512];
    char command[180];
    int getLength(char*);
    char* str1, str2, sectIndex;
    int length, i, size, total;
    PRINTS("Welcome to Blackdos \r\n\0");
    while(1)
    {
        interrupt(33,2,dir,257,0);
        while(1)
        {
            PRINTS("^(~(oo)~)^:  \0");
            SCANS(command);
            PRINTS("\r\n\0");
            if(command[4] != ' ' || command[4] != '\0')
            {
            	break;
            }
            command[4] = '\0';
            str1 = &command[5];
            if(strCmp(command, "copy\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                str2 = &str1[length+1];
                length = getLength(str2);
                if(isUpper(str2) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str2[length] = '\0';
                interrupt(33,3,str1,buffer,&length);
                interrupt(33,8,str2,buffer,length);
            }
            else if(strCmp(command, "exec\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                interrupt(33,4,str1,4,0);
                 
            }
            else if(strCmp(command, "prnt\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                interrupt(33,3,str1,buffer,&length);
                LPPRINTS(buffer);
            }
            else if(strCmp(command, "remv\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                interrupt(33,7,str1,0,0);
            }
            else if(strCmp(command, "show\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                interrupt(33,3,str1,buffer,&length);
                PRINTS(buffer); 
            }
            else if(strCmp(command, "twet\0"))
            {
                length = getLength(str1);
                if(isUpper(str1) || length > 8 || length < 2)
                {
                    interrupt(33,15,1,0,0);
                }
                str1[length] = '\0';
                str2 = &str1[length+1];
                for(i=0; str2[i] != '\0' && i < 140; ++i)
                {
                    buffer[i] = str2[i];
                }
                interrupt(33,8,str1,buffer,1);
                
            }
            else if(strCmp(command, "boot\0"))
            {
                interrupt(33,11,0,0,0);
            }
            else if(strCmp(command, "clrs\0"))
            {
                interrupt(33,12,0,0,0); 
            }      
            else if(strCmp(command, "ddir\0"))
            {
                total = 0;
                i = 0;
                PRINTS("Blackdos User File Directory\r\n\r\n\0");
                while(i < 512)
                {
                    sectIndex = &dir[i] + 8;   
                    if(dir[i] > 0)
                    {
                        size = getSize(sectIndex);
                        if(isUpper(&dir[i])==0)
                        {
                            dir[i+8]='\0';
                            PRINTS(&dir[i]);
                            PRINTS("    size: \0");
                            PRINTN(size);
                            PRINTS("\r\n\0");
                        }
                        total += size;
                    }
                    i += 32;
                }
            }
            else if(strCmp(command, "echo\0"))
            {
                PRINTS(str1);
                PRINTS("\r\n\0"); 
            }
            else if(strCmp(command, "help\0"))
            {
		        PRINTS("    User Manual \r\n\r\n\0");
		        PRINTS("boot: reboots system \r\n\0");
		        PRINTS("clrs: clears screen \r\n\0");
		        PRINTS("copy file1 file2: creates file2 and copies the content of file1 to it \r\n\0");
		        PRINTS("ddir: prints user file names and sizes, total space used, and free space \r\n\0");
		        PRINTS("echo comment: prints comment to display \r\n\0");
		        PRINTS("exec filename: executes filename  \r\n\0");
		        PRINTS("help: displays user manual \r\n\0");
		        PRINTS("prnt filename: prints content of filename to printer \r\n\0");
		        PRINTS("remv filename: deletes file \r\n\0");
		        PRINTS("senv: allows user to change set enviroment colors \r\n\0");
		        PRINTS("show filename: displays file content on screen \r\n\0");
		        PRINTS("twet filename: creates text file of 140 characters of less \r\n\0");
		        PRINTS("Press Enter to continue \r\n\0");
		        SCANS(buffer);
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
    int i;
    for(i=0; a[i]!='\0'; ++i)
    {
        if(a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

int isUpper(char* c)
{
    if (*c >= 'A' && *c <= 'Z') 
    {
        return 1;
    }
    return 0;
}

int getLength(char* str)
{
    int length=0;
    while(str[i] != ' ' || str[i] != '\0')
    {
        ++length;
    }
    return length;
}

int getSize(char* sect)
{
    i=0;
    while(sect[i]!=0 && i < 24)
    {
        ++i;
    }
    return i;
}




