bit get_ack( unsigned char address ); 
bit receive_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len);
bit transmit_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len);

bit receive_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len);
bit transmit_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len);


