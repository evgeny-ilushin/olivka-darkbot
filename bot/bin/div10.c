#include <stdio.h>

void timeAgo(long d, char *buf)
{
    if (d < 60)
    {
	sprintf(buf, "%ds", d);
    }
    else
    if (d < 3600)
    {
	sprintf(buf, "%dm", d/60);
    }
    else
    if (d < 86400)
    {
	sprintf(buf, "%dh", d/3600);
    }
    else
    {
	sprintf(buf, "%dd", d/86400);
    }
}

void main()
{
    char buf[1024];

    timeAgo(1, buf);
    printf("%s\n", buf);

    timeAgo(112, buf);
    printf("%s\n", buf);

    timeAgo(1123, buf);
    printf("%s\n", buf);

    timeAgo(131231, buf);
    printf("%s\n", buf);

    timeAgo(1231241, buf);
    printf("%s\n", buf);

    timeAgo(1, buf);
    printf("%s\n", buf);

    timeAgo(1, buf);
    printf("%s\n", buf);

    timeAgo(1, buf);
    printf("%s\n", buf);

    printf("%d\n", 12%10);
    printf("%d\n", 13%10);
    printf("%d\n", 14%10);
    printf("%d\n", 15%10);
    printf("%d\n", 16%10);
    printf("%d\n", 55%10);
}