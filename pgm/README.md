# tb-0x-kit-pgm
How to write program to TB-03F-KIT or TB-04-KIT.

1. Connect the SWS pin to pin 2 of CH340.
2. Execute the command, correcting the com port name.

![SCH](https://github.com/pvvx/TlsrComProg825x/blob/main/Doc/img/TB-03F-KIT-PGM.gif)

Как записать программу в TB-03F-KIT или TB-04-KIT.

1. Временно подключите вывод SWS к выводу 2 CH340.
2. Выполните команду, исправив имя com-порта.

Sampling log:
```
>TlsrComProg.py -p COM7 -t5000 we 0 ../fw/tb3f_b2z.bin
================================================
TLSR825x Floader version 27.12.23
------------------------------------------------
Open COM7, 230400 baud...
Reset module (RTS low)...
Activate (5000 ms)...
Warning: Wrong RX-TX connection?
Connection...
Load <floader.bin> to 0x40000...
Bin bytes writen: 1896
CPU go Start...
------------------------------------------------
ChipID: 0x5562 (TLSR8253), Floader ver: 1.0
Flash JEDEC ID: c86013, Size: 512 kbytes
------------------------------------------------
Inputfile: ../fw/tb3f_b2z.bin
Write Flash data 0x00000000 to 0x00026393...
------------------------------------------------
(1) Warning
```



