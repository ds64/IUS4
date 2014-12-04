# ---------------------------------------------------------------------------
# ��� �������

NAME	= test_serial

# ��������� ����������� � �������

CC      = sdcc
CFLAGS  = -I./INCLUDE -c --stack-auto
LFLAGS  = --code-loc 0x2100 --xram-loc 0x6000 --stack-auto --stack-loc 0x80

# ��������� ������� �������������� ������ ������

PROJECT  = $(shell type PROJECT)
VERSION  = $(shell type VERSION)
BUILD    = $(shell type BUILD)
TYPE     = $(shell type TYPE)

PROJNAME = ${PROJECT}-${VERSION}-${BUILD}-${TYPE}
TARBALL  = ${PROJNAME}.tar

# ��������� M3P

M3P		 = m3p
COMPORT	 = com3
COMLOG	 = $(COMPORT)_log.txt
BAUD	 = 9600

# �������� � ��������� ��������

SRC_DIR = SRC
# ---------------------------------------------------------------------------

all: test_serial

clean:
	del $(NAME).hex
	del $(NAME).bin
	del $(NAME).map
	del $(NAME).mem
	del $(NAME).lnk
	del pm3p_*.txt
	del com?_log.txt
	del $(TARBALL).gz
	del $(SRC_DIR)\*.asm
	del $(SRC_DIR)\*.rel
	del $(SRC_DIR)\*.rst
	del $(SRC_DIR)\*.sym
	del $(SRC_DIR)\*.lst

load:
	$(M3P) lfile load.m3p


dist:
	tar -cvf $(TARBALL) --exclude=*.tar .
	gzip $(TARBALL)

term:
	$(M3P) echo $(COMLOG) $(BAUD) openchannel $(COMPORT) +echo 6 term -echo bye



TEST_SERIAL_SRC = \
$(SRC_DIR)/asio.c \
$(SRC_DIR)/buf.c \
$(SRC_DIR)/keys.c \
$(SRC_DIR)/main.c \
$(SRC_DIR)/max.c \
$(SRC_DIR)/lcd.c \
$(SRC_DIR)/eeprom.c \
$(SRC_DIR)/timer.c
#$(SRC_DIR)/rtc.� \

TEST_SERIAL_OBJ = $(TEST_SERIAL_SRC:.c=.rel)

test_serial : $(TEST_SERIAL_OBJ) makefile
	$(CC) $(TEST_SERIAL_OBJ) -o test_serial.hex $(LFLAGS)
	$(M3P) hb166 test_serial.hex test_serial.bin bye


$(TEST_SERIAL_OBJ) : %.rel : %.c makefile
	$(CC) -c $(CFLAGS) $< -o $@
