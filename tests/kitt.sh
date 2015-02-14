while true; do 
./cp2103gpio --gpio=0 --out 1 --gpio 1 --out 0 --sleep 300
./cp2103gpio --gpio=1 --out 1 --gpio 2 --out 0 --sleep 300
./cp2103gpio --gpio=2 --out 1 --gpio 1 --out 0 --sleep 300
./cp2103gpio --gpio=1 --out 1 --gpio 0 --out 0 --sleep 300





done
