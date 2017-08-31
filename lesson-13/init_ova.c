#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/resource.h>

void daemonize(const char *cmd)
{
    int                 i,fd0,fd1,fd2;
    pid_t               pid;
    struct rlimit       r1;
    struct sigaction    sa;
    umask(0);
    //��ȡ�ļ����������ֵ
    getrlimit(RLIMIT_NOFILE,&r1);
    //�����ӽ���
    if((pid = fork()) < 0)
    {
         perror("fork() error");
         exit(0);
    }
    else if(pid > 0)  //ʹ�������˳�
        exit(0);
    setsid();  //�����Ự
   //�����ӽ��̱����ȡ�ն�
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGHUP,&sa,NULL);
    if((pid = fork()) < 0)
    {
         perror("fork() error");
         exit(0);
    }
    else if(pid > 0)
        exit(0);
   //�޸�Ŀ¼
    chdir("/");
    //�رղ���Ҫ���ļ�������
    if(r1.rlim_max == RLIM_INFINITY)
        r1.rlim_max = 1024;
    for(i=0;i<r1.rlim_max;++i)
        close(i);
    //���ļ�������
    fd0 = open("/dev/null",O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    openlog(cmd,LOG_CONS,LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR,"unexpected file descriptors %d %d %d",fd0,fd1,fd2);
        exit(1);
    }
}

int main()
{
    daemonize("ls");
    sleep(30);  //���������ߣ��Ա�鿴�ػ�����״̬
    exit(0);
}