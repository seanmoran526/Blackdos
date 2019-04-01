
int commandCheck(char*, char**);
int fileCheck(char*);
void copyFile(char*, char*);
void printDir(char*, char*);
void echo(char*);
void exec(char*);
void help(); 
void print(char*);
void remove(char*);
void show(char*, char*, int*);
void tweet(char*, char*); 

void main()
{
    char buffer[12288];
    char dir[512];
    char input[200]={0};
    char* commandLog[]={"boot", "clrs", "copy", "ddir", "echo", "exec",
                        "help", "prnt", "remv", "senv", "show", "twet"};
    int comNum;
    int size[17]={0};
    interrupt(33,0,"    Welcome to BlackDos Command Shell \r\n\0",0,0);
    while(1)
    {
        interrupt(33,2,dir,257,0);
        do
        {
            interrupt(33,0,"^(~(oo)~)^:  \0",0,0);
            interrupt(33,1,command,0,0);
            comNum = commandCheck(command, commandLog)
        }while(comNum<0);
        switch(comNum)
        {
            case 0: interrupt(33,11,0,0,0); break;
            case 1: interrupt(33,12,0,0,0); break;
            case 2: copyFile(input, buffer); break;
            case 3: printDir(dir,buffer); break;
            case 4: echo(input); break;
            case 5: exec(input); break;
            case 6: help(); break;
            case 7: print(input); break;
            case 8: remove(input); break;
            case 9: interrupt(33,4,"Stenv\0",4,0); break;
            case 10: show(input, buffer,size); break;
            case 11: tweet(input, buffer); break;
            default: (33,0, "Command Interpretter Error \r\n\0",0,0);   
        }
    }
}

int commandCheck(char* command, char** log)
{
    int i,j;
    int found;
    char* entry;
    if(input[4]!=' ' && input[4]!='\0')
    {
        interrupt(33,0,"    Invalid Command \r\n\0",0,0);
        return -1;
    }
    for(i=0; i<12; ++i)
    {
        found=1;
        entry = log[i];
        for(j=0; j<4; ++j)
        {
            if(command[j]!=entry[j])
            {
                found=-1;
                break;
            }
        }
        if(found==1)
        {
            return i;
        }
    }
    interrupt(33,0,"Invalid Command \r\n\0",0,0);
    return -1;
}
int fileCheck(char* input)
{
    if(input[5] < 'a')
    {
        interrupt(33,15,1,0,0);
    }
    
    for(i=5; i<13 || input[i]==' '; ++i)
    {
        fname[i]=input[i];
    }
    if(input[i]!= ' ')
    {
       interrupt(33,15,1,0,0);
    }
    return i;
}

void copyFile(char* input, char* buffer)
{
    int i,j;
    char fname[8]={0};
    char cname[8]={0};
    i = fileCheck(input);
    ++i
    if(input[i]<'a')
    {
       interrupt(33,15,1,0,0);
    }
    for(j=i; j< i+8 || input[j]=='\0'; ++j)
    {
        cname[j]=input[j];
    }
    interrupt(33,3,fname,buffer,1);
    interrupt(33,13,cname,buffer,1);
}

void printDir(char* dir, char* buffer)
{
    int i, j;
    for(i=0; i<512; i+=32)   
    {
        j=i+8;
        for(j; j<i+32; ++j)
        {
            
        }
    }
}
void echo(char*);
void exec(char*);
void help(); 
void print(char*);
void remove(char*);
void show(char*, char*, int*);
void tweet(char*, char*); 
