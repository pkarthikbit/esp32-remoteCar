cd /home/priyanka/esp-idf/
. ./export.sh
cd /home/priyanka/Documents/esp32-remoteCar/esp-idf
idf.py set-target esp32-c6
#idf.py menuconfig
idf.py build
