#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#include "headers/modem_control.h"
#include "headers/info.h"

int init_modem(char *device, int recv_timeout) {
	int fd;
	struct termios options;

	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (fd == -1) {
		return -1;

	}

	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd, &options);

	options.c_cflag    |= (CLOCAL | CREAD);
	options.c_iflag    &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag    &= ~OPOST;
	options.c_cc[VMIN]  = 0;
	options.c_cc[VTIME] = recv_timeout;

	tcsetattr(fd, TCSANOW, &options);

	return fd;

}

int get_message(int fd, int msgid, struct body *body_ptr,
				char *buff, size_t buff_size) {
	size_t size;
	size_t i;
	size_t n;
	size_t phone_n;
	size_t msg_n;
	int str2id_n;
	int head_index;
	int index;
	int j, k;
	char *tok;

	char aux[LONG_DATA];
	char head[MEDIUM_DATA];
	char message[MEDIUM_DATA];
	char phonenumber[SHORT_DATA];
	char str2id[SHORT_DATA];

	body_ptr->index = msgid;

	head_index = 0;
	n = 0;
	phone_n = sizeof(phonenumber);
	msg_n = sizeof(message);

	/* Se parsea el índice del mensaje */
	str2id_n = snprintf(str2id, sizeof(str2id), "AT+CMGR=%d\r", msgid);

    if (write(fd, "AT\r", 3) < 3) {
		return 1;

	}
	
	if (write(fd, "AT+CMGF=1\r", 10) < 10) {
		return 1;

	}
	
	if (write(fd, str2id, str2id_n) < str2id_n) {
		return 1;

	}

	while ((size = read(fd, aux, sizeof(aux)))) {
		if (n >= buff_size-1) {
			break;

		}

		for (i = 0; i < size && n < buff_size-1; i++, n++) {
			buff[n] = aux[i];

		}

	}

	buff[n] = '\0';

	/* Se obtiene el índice de donde comienza el mensaje */
	index = check_status(fd, 7, "+CMGR: ", buff, buff_size);

	/* Si es un carácter no imprimible, lo más probable es que no exista el mensaje */
	if (!isprint(buff[index])) {
		return ENOENT;
	
	}

	while (buff[index] != '\n') {
		head[head_index++] = buff[index];
		index++;

	}

	head[head_index] = '\0';

	tok = strtok(head, ",");

	if (!tok) {
		return 1;

	}

	if (strncmp("+CMGR: \"REC READ\"", tok, strlen(tok)) == 0) {
		body_ptr->read = 1;

	} else if (strncmp("+CMGR: \"REC UNREAD\"", tok, strlen(tok)) == 0) {
		body_ptr->read = 0;

	} else {
		body_ptr->read = -1;

	}

	body_ptr->phonenumber = strtok(NULL, ",");

	if (!body_ptr->phonenumber) {
		return 1;

	}

	/* Se aumenta un índice para eliminar el carácter de doble comillas */
	body_ptr->phonenumber++;

	for (j = 0; j < phone_n; j++) {
		/* Ahora se elimina el final */
		if (body_ptr->phonenumber[j] == '"') {
			body_ptr->phonenumber[j] = '\0';

		}

	}

	/* Eliminamos los caracteres de nueva línea */
	for (; buff[index] == '\n'; index++);

	for (k = 0; k < msg_n && buff[index] != '\n'; k++) {
		message[k] = buff[index++];

	}

	while (!isprint(message[k-1])) {
		k--;

	}

	message[k] = '\0';

	body_ptr->message = strdup(message);

	return 0;

}

int send_message(int fd, char *phonenumber, char *message, size_t msg_length) {
	char buff[SHORT_DATA];
	int size;

	size = snprintf(buff, sizeof(buff), "AT+CMGS=\"%s\"\r", phonenumber);

	if (write(fd, "AT\r", 3) < 3) {
		return 0;

	}

	if (write(fd, "AT+CMGF=1\r", 10) < 10) {
		return 0;

	}
	
	if (write(fd, buff, size) < size) {
		return 0;

	}
	
	if (write(fd, message, msg_length) < msg_length) {
		return 0;

	}

	return 1;

}

int delete_message(int fd, int index) {
	char str2id[SHORT_DATA];
	int size;

	size = snprintf(str2id, sizeof(str2id), "AT+CMGD=%d\r", index);

	if (write(fd, str2id, size) < size) {
		return 0;

	}
	
	return 1;

}

int check_status(int fd, int strn, char *str2cmp,
				 char *buff, size_t size) {
	int i, j;
	char ptr[strn];

	for (i = 0; i < size-(strn-1); i++) {
		for (j = 0; j < strn-1; j++) {
			ptr[j] = buff[i+j];

		}

		ptr[j] = '\0';

		if (strncmp(str2cmp, ptr, strlen(ptr)) == 0) {
			return i;

		}

	}

	return 0;

}
