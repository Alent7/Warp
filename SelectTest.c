#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include "wrap.h"

int main()
{
    int lfd = tcp4bind(8888,NULL);
    Listen(lfd,128);
    int maxfd = lfd;
    fd_set oldset,rset;
    FD_ZERO(&oldset);
    FD_ZERO(&rset);
    FD_SET(lfd, &oldset);
    while(1)
    {
        rset = oldset;
        int n = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(n < 0)
        {
            perror("");
            break;
        }
        else if(n == 0)
        {
            continue;
        }
        else
        {
            if(FD_ISSET(lfd, &rset))
            {
                int cfd = Accept(lfd, NULL, NULL);
                printf("xinlj\n");
                FD_SET(cfd, &oldset);
                if(cfd > maxfd)
                {
                    maxfd = cfd;
                }
                if(--n == 0)
                {
                    continue;
                }
            }
            for(int i = lfd + 1; i <= maxfd; i++)
            {
                if(FD_ISSET(i, &rset))
                {
                    char buf[1000] = "";
                    int ret = Read(i, buf, sizeof(buf));
                    if(ret < 0)
                    {
                        perror("");
                        close(i);
                        FD_CLR(i, &oldset);
                    }
                    else if(ret == 0)
                    {   
                        printf("client close\n");
                        close(i);
                        FD_CLR(i, &oldset);

                    }
                    else
                    {
                        printf("%s\n", buf);
                        Write(i, buf, ret);
                    }

                }
            }
        }
    }
}
