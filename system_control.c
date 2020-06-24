#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "headers/modem_control.h"
#include "headers/info.h"

int send_cmd(int fd, char *cmd, char *phonenumber) {
	FILE * result;
	char buff[MESSAGE_LIMIT];
	size_t size = 0;

	printf("Ejecutando %s...\n", cmd);

	result = popen(cmd, "r");

	if (!result) {
		return 0;

	}

	while (!feof(result) && !ferror(result) && (size = fread(buff, sizeof(char), sizeof(buff)-1, result)) > 0) {
		buff[size] = CHAR_DELIM;

		printf("Enviando %ld bytes...\n", size);

		if (send_message(fd, phonenumber, buff, size+1)) {
			printf("¡Mensaje enviado con éxito!\n");

		} else {
			if (errno == 0) {
				printf("Ocurrió un error enviando el mensaje\n");

			} else {
				perror("Ocurrió un error enviando el mensaje");

			}

		}

		printf("Esperando %d segundos para retomar el bucle ó salir\n", LOOP_SLEEP);

		sleep(LOOP_SLEEP);

	}

	printf("Fin de la ejecución de \"%s\"\n", cmd);

	return 1;

}
