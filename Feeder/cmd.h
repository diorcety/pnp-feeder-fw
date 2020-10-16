#ifndef CMD_H_
#define CMD_H_

#define MAX_CMD_SIZE 15
#define C_RST '\x03'
#define C_COM '\x0A'
extern char cmd[MAX_CMD_SIZE + 1];

void Handle();

#endif /* CMD_H_ */
