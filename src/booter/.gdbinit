target remote localhost:1234
set architecture i8086
break *0x7c19

display/5i $pc
