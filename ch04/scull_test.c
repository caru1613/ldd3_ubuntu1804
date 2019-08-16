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
	int ret;
	int fd = open("/dev/scull0", O_RDWR);
	
	printf("fd: %d.\n", fd);

	if(fd == -1) {
		printf("invaild fd.\n");
		return 1;
	}

	ret = read(fd, buf, sizeof(buf));
	ret = write(fd, buf, sizeof(buf));
	ret = ioctl(fd, DUMMYCMD, 0);
	ret = lseek(fd, 10, 0);

	close(fd);

	return 0;
}
