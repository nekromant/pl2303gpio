while true; do 
sudo ./cp2103gpio --gpio=0 --out 0
sleep 0.3
sudo ./cp2103gpio --gpio=0 --out 1
sleep 0.3
sudo ./cp2103gpio --gpio=0 --out 0
sleep 0.3
sudo ./cp2103gpio --gpio=0 --out 1
sleep 1
done
