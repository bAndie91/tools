/* Begin C code. Put this in a file called cdstatus.c and do "gcc -o
* cdstatus cdstatus.c" . No error checking, no looping, no
* documentation, if it breaks, you get to keep both pieces. */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/mtio.h>
#include <linux/types.h>
#include <linux/cdrom.h>

int main(int argc, char **argv) {
	char buff[256];
	/* ioctl() needs a 3rd arg but it doesn't really use it here. */
	int fd,status;
	int ec=1;

	if(argc!=2) {
		fprintf(stderr,"Usage: cdstatus <CD-ROM device name>\n");
		return 1;
	}

	fd=open(argv[1], O_RDONLY | O_NONBLOCK); /* need NONBLOCK! */
	if(fd < 0) {
		snprintf(buff,64,"%s cannot be opened\n",argv[1]);
		perror(buff);
		return 1;
	}
	status=ioctl(fd,CDROM_DRIVE_STATUS,buff);
	close(fd);

	switch(status) {
	case CDS_NO_INFO:
		printf("Whoa. Huh?");
		ec=2;
		break;
	case CDS_NO_DISC:
		printf("No disc is in the drive.");
		ec=3;
		break;
	case CDS_TRAY_OPEN:
		printf("Tray is open, or tray is closed and no CD is present.");
		ec=4;
		break;
	case CDS_DRIVE_NOT_READY:
		printf("Drive is not ready.");
		ec=5;
		break;
	case CDS_DISC_OK:
		printf("A disc is in the drive.");
		ec=0;
		break;
	default:
		printf("Um, we shouldn't be here!");
		ec=1;
	}
        printf("\n");
	
	return ec;
}

/* end of C code */