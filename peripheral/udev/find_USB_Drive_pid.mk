TA=find_USB_Drive_pid
all:
	gcc $(TA).c -o $(TA) -ludev
clean:
	rm -rf $(TA).o $(TA) 
