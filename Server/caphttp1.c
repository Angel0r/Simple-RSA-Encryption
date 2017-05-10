#include <glib.h>
#define _GNU_SOURCEinclude <gio/gio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <capture.h>
#include <axsdk/axhttp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define file "/tmp/jpg.jpg"

char* camera;
char* seq(char* s1, char* s2) {
	char *combine = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(combine, s1);
	strcat(combine, s2);
	return combine;
}
char * strres(int re) {
	if (re == 1) {
		return "320x180";
	} else if (re == 2) {
		return "480x270";
	} else if (re == 3) {
		return "640x360";
	} else if (re == 4) {
		return "800x450";
	}
	return "320x180";
}
int read_socket(int newSockfd) {
	unsigned char buf[3];
	int k = 0;
	while (1) {
		int nbytes = recv(newSockfd, &buf[k], 1, 0);
		if (nbytes == -1) {
			break;
		}
		if (nbytes == 0) {
			break;
		}
		if (k == 3) {
			break;
		}
		k++;
	}
	unsigned long int i2 = ((unsigned long int) buf[0] << 24)
			| ((unsigned long int) buf[1] << 16)
			| ((unsigned long int) buf[2] << 8) | buf[3];
	int i;
	if (i2 <= 0x7fffffffu) {
		i = i2;
	} else {
		i = -1 - (long int) (0xffffffffu - i2);
	}
	return i;
}
int main(void) {
	int process;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int newsockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	unsigned char buffer[256];
	signed char xorByteArray[2];
	int xorint = 0;
	unsigned char res = 0;
	unsigned char fps = 0;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5005);
	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	for (;;) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		bzero(buffer, 256);
		syslog(LOG_INFO, "Connection established");
		process = fork();
		if (process < 0) {
			syslog(LOG_INFO, "Error on fork");
			exit(1);
		}
		if (process == 0) {
			srand(time(NULL));
			int p = rand() % 1000;
			while (checkPrime(p) != 1) {
				p = rand() % 1000;
			}
			syslog(LOG_INFO, "p= %d", p);
			int q = rand() % 1000;
			while (checkPrime(q) != 1) {
				q = rand() % 1000;
			}
			int n = p * q;
			int t = (p - 1) * (q - 1);
			int e = gen_E(t);
			int d = gen_D(e, t);
			int htonle = htonl(e);
			int htonln = (htonl(n));
			write(newsockfd, (void *) &htonle, sizeof(e)); //Send e and n to client
			write(newsockfd, (void *) &htonln, sizeof(n));
			int xor = RSADecr(read_socket(newsockfd), d, n); //Receive XORKEY and decrypt
			xorint = xor;
			xorByteArray[0] = (signed char) (xorint & 0xFF);
			xorByteArray[1] = (signed char) ((xorint >> 8 & 0xFF));
			res = RSADecr(read_socket(newsockfd), d, n); //Receive RES and FPS from client and decrypt
			fps = RSADecr(read_socket(newsockfd), d, n);
			char * stringRES = strres(res);
			char stringFPS[2];
			sprintf(stringFPS, "%d", fps);
			char* con1 = seq("resolution=", stringRES);
			char* con2 = seq(con1, "&fps=");
			camera = seq(con2, stringFPS);
			media_stream *stream;
			stream = capture_open_stream(IMAGE_JPEG, camera);
			syslog(LOG_INFO, "stream Open");
			while (1) {
				media_frame *frame;
				frame = capture_get_frame(stream);
				void *data;
				data = capture_frame_data(frame);
				size_t frame_size = capture_frame_size(frame);
				int XORpicSize = ((int) frame_size) ^ xorint; //XOR encryption for image size
				int size = htonl((int) XORpicSize);
				write(newsockfd, &size, sizeof(int));  //send size to client
				int x;
				unsigned char dataBufferEncr[frame_size];
				int XORLength = sizeof(xorByteArray) / sizeof(xorByteArray[0]);
				for (x = 0; x < frame_size; x++) {
					dataBufferEncr[x] = ((unsigned char*) data)[x]
							^ xorByteArray[(XORLength - 1) - (x % XORLength)]; ////XOR encryption for frame size
				}
				write(newsockfd, &dataBufferEncr[0], frame_size);
				capture_frame_free(frame);
			}
			capture_close_stream(stream);
			exit(0);
		} else {
			close(newsockfd);
		}
	}
	close(sockfd);
	return (0);
}
int checkPrime(int number) {
	if (number <= 1)
		return 0;
	int i;
	for (i = 2; i < number; i++) {
		if (number % i == 0)
			return 0;
	}
	return 1;
}
int gen_E(int t) {
	int e;
	do {
		e = rand() % t;
	} while (GCD(e, t) != 1);
	return e;
}
int gen_D(int e, int t) {
	int returnTemp = 0;
	int nt = 1;
	int r = t;
	if (t < 0) {
		t = -t;
	}
	if (e < 0) {
		e = t - (-e % t);
	}
	int nr = e % t;
	while (nr != 0) {
		int quot = (r / nr) | 0;
		int tmp = nt;
		nt = returnTemp - quot * nt;
		returnTemp = tmp;
		tmp = nr;
		nr = r - quot * nr;
		r = tmp;
	}
	if (r > 1) {
		return -1;
	}
	if (returnTemp < 0) {
		returnTemp = returnTemp + t;
	}
	return returnTemp;
}
int GCD(int e, int t) {
	int c;
	while (e != 0) {
		c = e;
		e = t % e;
		t = c;
	}
	return t;
}
int RSAEncr(int m, int n, int e) {
	int j = 0;
	long long int encryptedM = 1;
	for (j = 0; j < e; j++) {
		encryptedM = encryptedM * m % n;
	}
	return (int) encryptedM;
}
int RSADecr(int mEnc, int d, int n) {
	int j = 0;
	long long int decryptedM = 1;
	for (j = 0; j < d; j++) {
		decryptedM = decryptedM * mEnc % n;
	}
	return (int) decryptedM;
}
