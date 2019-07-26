#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#define DUMMYCMD 0
int main()
{
	char buf[50];
	int fd = open("/dev/scull0", O_RDWR);
	
	printf("fd: %d.\n", fd);

	if(fd == -1) {
		printf("invaild fd.\n");
		return 1;
	}

	read(fd, buf, sizeof(buf));
	write(fd, buf, sizeof(buf));
	ioctl(fd, DUMMYCMD, 0);
	lseek(fd, 10, 0);

	close(fd);

	return 0;
}
