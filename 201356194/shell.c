#include<stdio.h>//all included libraries are listed
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<wait.h>
#include<sys/procfs.h>
#include<sys/fcntl.h>
#include<math.h>
#include<sys/stat.h>
#include<signal.h>
#include<errno.h>

int argvals;//all global varibales
int curr_proc;
int ch=0;
int back;
int p1;
char command[51][101];
char in1[1001];

struct pid_list//main structure used
{
    char process[51];
    pid_t pid;
    int back;
};

int redir_in;
int redir_out;
char hostname2[51];
char uname[51];
char homepath[101];
struct pid_list list[1001];

void findit(char *);//all functions defined
void fun();
void pid();
void execute();
void cd();
void pid1(int);
void pinfo();
void handler(int);
void jobs();
void overkill();


void cd()
{
    char path[101],curr[101];
    if(argvals==1)
	chdir(homepath);
    else
    {
	getcwd(curr,101);//get the current path in character array
	if (command[1][0]!='/')
      	    sprintf(path,"%s/%s",curr,command[1]);
	else 
	    sprintf(path,"%s",command[1]);
	if(chdir(path)==-1)
	{
	    fprintf(stderr,"No such directory found\n");
	    chdir(curr);
	}
    }
}
void findit(char *path)//function used to print relative path
{
    int i2=0,flag=1;
    int len_homepath=strlen(homepath);
    int len_path=strlen(path);
    while(i2<len_homepath)
    {
	if(homepath[i2]!=path[i2])
	{
	    flag=0;
	    break;
	}
	i2++;
    }
    if(flag)
    {
	printf("<%s@%s:~",uname,hostname2);//use ~ when in home
	i2=len_homepath;
	while(i2<len_path)
	{
	    printf("%c",path[i2]);
	    i2++;
	}
	printf(">");
    }
    else
	printf("<%s@%s:%s> ",uname,hostname2,path);//else dont use ~
}
void pid1(int num1)
{
    char pro[51],try[101],Name[101],State[101],buf[101];
    long unsigned int VmSize;
    sprintf(try,"/proc/%d/status",num1);
    FILE *fp1=fopen(try,"r");//open file to get information
    if(fp1)
    {
	fgets(buf,256,fp1);
	sscanf(buf,"Name : %s",Name);
	fgets(buf,256,fp1);
	sscanf(buf,"State : %c",State);
	fgets(buf,256,fp1);
	sscanf(buf,"VmSize : %lu",&VmSize);
    }
    printf("pid -- %d\nProcess Status -- %c\nmemory -- %lu\n",num1,State[0],VmSize);//display required information from the file
    sprintf(pro,"/proc/%d/exe",num1);
    FILE *fp2=fopen(pro,"r");
    char Path[101];
    ssize_t len=readlink(pro,Path,sizeof(Path)-1);//gives the executable path to the file
    if(len!=-1)
	Path[len]='\0';
    printf("Executable Path -- %s\n",Path);
}    

void pinfo()
{
    pid_t cur_pid=getpid();//get the pid of the current process
    int returnval=fork();//create a new process
    if(returnval)//case when returnval has value 0
    {
	if(!ch)
	    list[curr_proc++].pid=returnval;
	waitpid(returnval,&returnval,0);
    }
    else
    {
	int i2=0;
	char pro[51],try[101],Name[101],State[101],buf[101];
	long unsigned int VmSize;
	sprintf(try,"/proc/%d/status",cur_pid);
	FILE *fp1=fopen(try,"r");//open file to get information

	if(fp1)
	{
	    fgets(buf,256,fp1);
	    sscanf(buf,"Name : %s",Name);
	    fgets(buf,256,fp1);
	    sscanf(buf,"State : %c",State);
	    fgets(buf,256,fp1);
	    sscanf(buf,"VmSize : %lu",&VmSize);
	}
	printf("pid -- %d\nProcess Status -- %c\nmemory -- %lu\n",cur_pid,State[0],VmSize);//display required information from the file
	sprintf(pro,"/proc/%d/exe",cur_pid);
	FILE *fp2=fopen(pro,"r");
	char Path[101];
	ssize_t len=readlink(pro,Path,sizeof(Path)-1);//gives the executable path to the file
	if(len!=-1)
	    Path[len]='\0';
	printf("Executable Path -- %s\n",Path);
    }
}
void fun()
{
    int i2=0;
    while(in1[i2]!='\0')
    {
	if(in1[i2]==' '||in1[i2]=='\t')
	    i2++;
	else if(in1[i2]== '>')//redirection case
	{
	    redir_out=argvals;
	    sprintf(command[argvals++],">");
	    i2++;
	}
	else if(in1[i2]=='<')
	{
	    redir_in=argvals;
	    sprintf(command[argvals++],"<");
	    i2++;
	}
	else if(in1[i2]=='&')//for background processes
	{
	    back=argvals;
	    sprintf(command[argvals++],"&");
	    i2++;
	}
	else
	{
	    int p=0;
	    if(in1[i2]=='~')
	    {
		if(in1[i2+1]==' '||in1[i2+1]=='\t'||in1[i2+1]=='\0'||in1[i2+1]=='/')
		{
		    for(p=0;homepath[p]!='\0';p++)
			command[argvals][p]=homepath[p];
		    command[argvals][p++]='/';
		    i2++;
		}
	    }
	    while(in1[i2]!=' '&&in1[i2]!='\t'&&in1[i2]!='|'&&in1[i2]!='>'&&in1[i2]!='<'&&in1[i2]!='\0'&&in1[i2]!='&')
		command[argvals][p++]=in1[i2++];
	    command[argvals++][p]='\0';
	}
    }

}
void jobs()
{ 
    int i2=0,p=1;
    while(i2<curr_proc)
    {
	if(list[i2].back)
	{
	    printf("[%d] %s [%d]\n",p,list[i2].process,list[i2].pid);//prints information about all background processes
	    p++;
	}
	i2++;
    }
}

void overkill()
{
    int i2=0,p=1;
    while(i2<curr_proc)
    {
	if(list[i2].back)
	    kill(list[i2].pid,SIGKILL);//kill all background processes
	i2++;
    }
}

void execute()
{
    if(!back)//perform user commands given
    {
	if(strcmp(command[0],"quit")==0)
	    exit(0);
	else if(strcmp(command[0],"cd")==0)
	    cd();
	else if(strcmp(command[0],"overkill")==0) 
	    overkill();
	else if(strcmp(command[0],"pinfo")==0)
	{
	    if(!ch)
	    {
		strcpy(list[curr_proc].process,"pinfo");
		list[curr_proc].back=0;
	    }
	    if(command[1][0]==0)//if only single argument given with pinfo command
		pinfo();
	    else
	    {
		int i=0,comm1,number=0,num1;
		int len1=strlen(command[1]);
		if(len1==4)
		    num1=10*10*10;
		else if(len1==3)
		    num1=10*10;
		else if(len1==2)
		    num1=10;
		else if(len1==1)
		    num1=1;
		while(i<len1)
		{
		    comm1=(int)(command[1][i]-'0');
		    number=num1*comm1+number;
		    num1=num1/10;
		    i++;
		}
		pid1(number);
	    }
	}
	else if(strcmp(command[0],"jobs")==0)
	{
	    if(!ch)
	    {
		strcpy(list[curr_proc].process,"jobs");
		list[curr_proc].back=0;
	    }
	    jobs();
	}    

	else if(strcmp(command[0],"exit")==0) 
	    fprintf(stderr,"The command is quit\n");
	else//for system defined commands
	{
	    if(!ch)
	    {
		strcpy(list[curr_proc].process,command[0]);
		list[curr_proc].back=0;
	    }
	    int returnval=fork();
	    if(returnval)
	    {
		list[curr_proc++].pid=returnval;
		waitpid(returnval,&returnval,0);
	    }
	    else
	    {
		int i2=0;
		char arguments[101];
		char *args[argvals+1];
		while(i2<argvals)
		{
		    args[i2]=(char*)malloc(101*sizeof(char));
		    strcpy(args[i2],command[i2]);
		    i2++;
		}
		args[i2]=NULL;
		int flag2=1;
                if(redir_in)//incase of redirection
                {
                    if(redir_in<argvals -1)
                     {  
                      freopen(command[redir_in + 1],"r",stdin);
                      args[redir_in]=NULL;
                     }  
                    else
                     {
                     fprintf(stderr,"Give filename for redirection\n");
                     exit(0);
                     }
                }
                if(redir_out)
                {
                    if(redir_out < argvals -1)
                    {
                        freopen(command[redir_out + 1],"w",stdout);
                        args[redir_out]=NULL;
                    }
                    else
                    {
                      fprintf(stderr,"Give filename for redirection\n");
                      exit(0);
                    }
                 }                
		printf("%s",*args);
		flag2=0;
		execvp(args[0],args);//execute the command in arg
		fprintf(stderr,"Not a valid shell command\n");
		exit(0);
	    }
	}
    }
    else
    {
	if(!ch)
	{
	    strcpy(list[curr_proc].process,command[0]);
	    list[curr_proc].back=1;
	}
	int returnval=fork();
	if(returnval)
	{
	    if(!ch)
		list[curr_proc++].pid=returnval;
	    printf("Command %s\t Pid %d\n",command[0],returnval);
	}
	else
	{
	    int i2=0;
	    int flag3=0;
	    char *args[argvals];
	    while(i2<back) 
	    {
		args[i2]=(char*)malloc(101*sizeof(char));
		strcpy(args[i2],command[i2]);
		i2++;
		flag3=1;
	    }
	    args[i2]=NULL;
	    execvp(args[0],args);//execute the command in args
	    fprintf(stderr,"Not a valid Shell command\n");
	    exit(0);
	}
    }
}

void handler(int sig)
{
    pid_t pid;
    pid=waitpid(WAIT_ANY,NULL,WNOHANG);
    int i2;
    if(pid>0&&!ch)
    {
	i2=0;
	while(i2<curr_proc)
	{
	    if(list[i2].pid==pid)//check if pid matches
	    {
		list[i2].back=0;
		break;
	    }
	    i2++;
	}
	char path[100];
	getcwd(path,100);
	printf("\n process : %s has exited with pid : %d\n", list[i2].process, pid);//print process name and pid if the process has terminated
	findit(path);
	fflush(stdout);
    }
    signal(SIGCHLD,handler);
}

int main ()
{
    gethostname(hostname2,51);
    int f1=0;
    sprintf(uname,"%s",getenv("USER"));
    sprintf(homepath,"%s",getenv("PWD"));
    signal(SIGCHLD,handler);
    int i2,i1=0,i3=0;
    char cur_dir[101];
    curr_proc=0;
    while(1)
    {
	getcwd(cur_dir,101);//get path of current working directory
	back=0;
	p1=0;
	in1[0]='\0';
	if(!ch)
	{
	    findit(cur_dir);
	    scanf("%[^\n]",in1);
	    getchar();
	    if(in1[0]=='\0') 
		continue;
	}
	i2=0;
	argvals=0;
	while(i2<argvals)
	{
	    command[i2][0]='\0';
	    i2++;
	    i3++;
	}
	redir_in=0;
	redir_out=0;
	fun();//call function
	execute();
	i1++;
    }
    return 0;
}













