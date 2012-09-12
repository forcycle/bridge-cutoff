/*
* pnx.c
* Copyright (C) 2011 NXP Semiconductors
*/

/*
* Android.mk //TODO:Here is makefile reference
* LOCAL_PATH:= $(call my-dir)

* include $(CLEAR_VARS)
* LOCAL_SRC_FILES:= pnx.c

* LOCAL_MODULE:= pnx

* include $(BUILD_EXECUTABLE)
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>  
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <pnx.h>

#include <unistd.h>   
#include <getopt.h> 

#define _read_sample_test_size	0x4

struct hci_reset {
	unsigned char length;
	unsigned char RSET;
	unsigned char Window_size;
	unsigned char Endpoint_capabilities;
	unsigned char crc1;
	unsigned char crc2;
};

int main(int argc, char *argv[]) {

	int ret = 0;
	int i = 0;
	int fp = 0;
	unsigned char buf[40];
	struct hci_reset msg = { 
		.length = 0x05,
		.RSET = 0xF9,
		.Window_size = 0x04,
		.Endpoint_capabilities = 0x00,
		.crc1 = 0xc3,
		.crc2 = 0xe5,
	};
	printf("open module\n");
	//open module
	if ((ret = (fp = open("/dev/pn544", O_RDWR))) < 0) {
		printf("pn544 open error retcode = %d, errno = %d\n", ret, errno);
		exit(0);
	}
	printf("hardware reset\n");
	//hardware reset
	ioctl(fp, PN544_SET_PWR, 1);
	ioctl(fp, PN544_SET_PWR, 0);
	ioctl(fp, PN544_SET_PWR, 1);

	printf("write one RSET frame\n");
	//write one RSET frame
	if ((ret = (write(fp, &msg, sizeof(struct hci_reset))))<0){
		printf("pn544 write error retcode = %d, errno = %d\n", ret, errno);
		goto err_;
	}
	printf("read HCI responses\n");
	//read HCI responses(normally is 0x03 e6 17 a7)
	while (1){
		if ((ret = (read(fp, &buf[0], _read_sample_test_size))) < 0) {
			printf("pn544 read error retcode = %d, errno = %d\n", ret, errno);
		}
		else{
			for (i=0;i<_read_sample_test_size;i++){
			printf("NFC->PC: 0x%02x\n", buf[i]);}
			break;
		}
		sleep(1);
	}
	printf("close module\n");
err_:
	close(fp);	
	return 0;
}

