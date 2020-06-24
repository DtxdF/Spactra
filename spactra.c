#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "headers/modem_control.h"
#include "headers/system_control.h"
#include "headers/info.h"

int main(void) {
	int fd;
	int msgid;
	int result;
	int control;
	int loop_n = 0;
	char buff[LONG_DATA];
	struct body body_msg;

	fd = init_modem(DEVICE, CHAR_TIMEOUT);

	if (fd == -1) {
		perror("init_modem()");

		return errno;

	}

	printf("Leyendo un máximo de %d mensajes...\n", MEMORY_INDEX);

	while (1) {
		for (msgid = 0; msgid < MEMORY_INDEX; msgid++) {
			result = get_message(fd, msgid, &body_msg, buff, sizeof(buff));

			if (result != 0) {
				continue;

			}

			control = strncmp(PHONECONTROL, body_msg.phonenumber,
							  strlen(body_msg.phonenumber));

			if (body_msg.read == -1) {
				printf("No se pudo detectar si el mensaje fue leído o no\n");
				free(body_msg.message);

				continue;

			}

			if ((body_msg.read == 0) && (control == 0)) {
				if (send_cmd(fd, body_msg.message, PHONECONTROL)) {
					printf("Comando ejecutado exitosamente: %s (%d)\n",
						   body_msg.message, body_msg.index);

				} else {
					printf("Ocurrió un error enviando un mensaje a \"%s\"\n",
						   body_msg.phonenumber);

				}

				if (delete_message(fd, body_msg.index)) {
					printf("Mensaje número %d borrado con éxito\n",
						   body_msg.index);

				} else {
					printf("Ocurrió un error borrando el mensaje (%d) del número \"%s\"\n",
						   body_msg.index, body_msg.phonenumber);


				}

			}

			free(body_msg.message);

		}

		printf("Reiniciando bucle: %d\n", ++loop_n);

	}

	close(fd);

	return EXIT_SUCCESS;

}
