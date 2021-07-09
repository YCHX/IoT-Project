#
TARGET1 = single
TARGET2 = final
TARGET3 = final-sub

#
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -Dsoc_cv_av -static -g -Wall  -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av
LDFLAGS =  -g -Wall  
CC = $(CROSS_COMPILE)gcc-5
ARCH= arm


build: $(TARGET)
$(TARGET1): NumberControl.o sensor.o synchronize.o synthesized-single.o 
	$(CC) $(LDFLAGS)   $^ -o $@
$(TARGET2): NumberControl.o sensor.o synchronize.o synthesized.o 
	$(CC) $(LDFLAGS)   $^ -o $@
$(TARGET3): NumberControl.o sensor.o synchronize.o synthesized-sub.o 
	$(CC) $(LDFLAGS)   $^ -o $@
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) *.a *.o *~ 
