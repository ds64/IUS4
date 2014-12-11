#ifndef _ASIO_H_
#define _ASIO_H_

void ASIO_INIT( unsigned char speed  );
char ASIO_READ(void);
char ASIO_WCHAR(unsigned char ch);
void ASIO_WSTRING(char * str);

#endif
