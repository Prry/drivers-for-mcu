#ifndef _24CLxx_H_
#define _24CLxx_H_

extern void ee_24clxx_readbytes(u16 ReadAddr, char* pBuffer, u16 NumByteToRead);
extern char ee_24clxx_writebyte(u16 addr,u8 data);
extern char ee_24clxx_writebytes(u16 write_addr, char* pwrite_buff, u16 writebytes);
extern char ee_24clxx_erasebytes(u16 WriteAddr, char Erasedata, u16 NumByteToErase);

#endif
