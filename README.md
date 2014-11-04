# PL2303 Userspace GPIO control tool

This is a small tool for linux userspace that allows you to interact
with GPIO lines on PL2303HX devices. 
It has only been tested on PL2303HXA, but may work for other revisions. 
Since no common GPIO driver for those exists at the time of writing in
the linux kernel - this should serve as a placeholder till upstream guys 
fix that.  


## Compiling & installing

You'll need libusb-1.0-dev, gcc and pkg-config. Just run 

```
make 
sudo make install
```

pl2303gpio comes with udev rules that will be dropped into your
/etc/udev/rules.d/
Just reload udev after installing it

## Do I have to close minicom / rmmod pl2303 to use this tool ? 

Nope. Just run it in a separate terminal whenever you need it

## Can I chose which of 70 PL2303 devices plugged this will work with?

Nope, you can't. Patches welcome. 

## Using

```
pl2303gpio --help
PL2303HXA userspace GPIO control tool
(c) Andrew 'Necromant' Andrianov 2014, License: GPLv3
Usage: ./pl2303gpio [action1] [action2] ...
Options are: 
         -g/--gpio  n  - select GPIO, n=0, 1
         -i/--in       - configure GPIO as input
         -o/--out v    - configure GPIO as output with value v
         -r/--read v   - Read current GPIO value

Examples: 
        ./pl2303gpio --gpio=1 --out 1
        ./pl2303gpio --gpio=0 --out 0 --gpio=1 --in
```

## License

GPLv3. I'm too lazy to copy all the text here. Google for it.