# PL2303 and CP2103 Userspace GPIO control tools

This is a set of small tools for linux userspace that allows you to interact
with GPIO lines on PL2303HX and CP2103 devices. 
PL2303: It has only been tested on PL2303HXA, but may work for other revisions. 
CP2103: It has only been tested on CP2103, but may work for other revisions/versions.

Since no common GPIO driver for those exists at the time of writing in
the linux kernel - this should serve as a placeholder till upstream guys 
fix that. 


# Compiling & installing

You'll need libusb-1.0-dev, gcc and pkg-config. Just run 

```
make 
sudo make install
```
##Installing udev rules (Linux only)

This package comes with udev rules that will be dropped into your
/etc/udev/rules.d/

To automagically install and reload rules run

```
sudo make install-rules
```

and replug your pl2303/cp2103 dongles. You should no longer need root
permissions to access gpio in them. 

## Do I have to close minicom / rmmod pl2303 to use this tool ? 

Nope. Just run it in a separate terminal whenever you need it

## Can I chose which of 70 PL2303 devices plugged this will work with?

Yep. You can specify --product, --serial and --manuf strings to match
the device you want. 
These must _precede_ any other options. Example: 

```
./cp2303gpio --product=PowerUART --serial=fireblade --gpio 0 --out 1
```

## Usage

CP2103
```
PL2303HXA/CP2103 userspace GPIO control tool
(c) Andrew 'Necromant' Andrianov 2014, License: GPLv3
Usage: ./cp2103gpio [action1] [action2] ...
Options are: 
         --product=blah    - Use device with this 'product' string
         --serial=blah     - Use device with this 'serial' string
         --manuf=blah      - Use device with this 'manufacturer' string
         -g/--gpio  n      - select GPIO, n=0, 1
         -i/--in           - configure GPIO as input
         -o/--out v        - configure GPIO as output with value v
         -r/--read v       - Read current GPIO value

         -s/--sleep v      - Delay for v ms

Examples: 
        ./cp2103gpio --gpio=1 --out 1
        ./cp2103gpio --gpio=0 --out 0 --gpio=1 --in

All arguments are executed from left to right, you can add 
delays using --sleep v option. e.g. 
        ./cp2103gpio --gpio=0 --out 0 --sleep 1000 --gpio=0 --out 1
```

PL2303
```
PL2303HXA/CP2103 userspace GPIO control tool
(c) Andrew 'Necromant' Andrianov 2014, License: GPLv3
Usage: ./pl2303gpio [action1] [action2] ...
Options are: 
         --product=blah    - Use device with this 'product' string
         --serial=blah     - Use device with this 'serial' string
         --manuf=blah      - Use device with this 'manufacturer' string
         -g/--gpio  n      - select GPIO, n=0, 1
         -i/--in           - configure GPIO as input
         -o/--out v        - configure GPIO as output with value v
         -r/--read v       - Read current GPIO value

         -s/--sleep v      - Delay for v ms

Examples: 
        ./pl2303gpio --gpio=1 --out 1
        ./pl2303gpio --gpio=0 --out 0 --gpio=1 --in

All arguments are executed from left to right, you can add 
delays using --sleep v option. e.g. 
        ./pl2303gpio --gpio=0 --out 0 --sleep 1000 --gpio=0 --out 1
```

## Limitations
cp2303 code misses magic required for 'input' mode of GPIO lines. 

## Tips and tricks
The arguments are executed from left to right. Therefore you can use only one 
call to pl2303gpio/cp2303gpio to execute a sequence of actions. If the gpio line 
stays the same, you can omit subsequent --gpio arguments. 
You can use --sleep to add arbitary delays (in milliseconds)

Example (sets the gpio3 line to 1, sleeps for one second, sets the gpio3 to 0: 
```
./cp2303gpio --gpio=3 --out=1 --sleep=1000 --out=0
```

##Bonus scripts
The 'extra' directory contains a few scripts I use on my OpenWRT box to manage a few
SBC servers. They are installed via install-scripts. 

serverctl - is a bach script that allows quickly to power on/power off/reboot/access minicom on any of the boxes by just supplying the hostname (which is also wired into cp2303 onboard eeprom as 'serial number').  
serverd   - is a lua daemon that pings servers in round-robin fasion and displays their state on LEDs 

## Mac OS X hint
These tools have been reported to work on Mac OS X. For obvious reasons you
don't need udev rules on a mac. 


## License

GPLv3. I'm too lazy to copy all the text here. Google for it.