#ifndef _MODEM_CONTROL_H
#define _MODEM_CONTROL_H

struct body {
	int index;
	int read;
	char *phonenumber;
	char *message;

};

int init_modem(char *, int);
int check_status(int, int, char *, char *, size_t);
int delete_message(int, int);
int send_message(int, char *, char *, size_t);
int get_message(int, int, struct body *, char *, size_t);

#endif
