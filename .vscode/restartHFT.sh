
echo -e "\nStopping All...............\n" 

cd /root/Utrade/muTrade-1.0.0-Linux_I2
./stopServer.sh
./stopDms.sh
cd /root/Utrade/muTrade-1.0.0-Linux_I2/Esm
./stopEsm.sh
pkill PCap

echo -e "\nAll Stopped\n"


echo -e "\nStopping All...............\n" 

cd /root/Utrade/muTrade-1.0.0-Linux_I2
./stopServer.sh
./stopDms.sh
cd /root/Utrade/muTrade-1.0.0-Linux_I2/Esm
./stopEsm.sh
pkill PCap

echo -e "\nAll Stopped\n"


echo -e "\nStarting All...............\n" 

cd /root/Utrade/muTrade-1.0.0-Linux_I2/feed/bin
./startFeedCm.sh
./startFeedFo.sh
cd /root/Utrade/muTrade-1.0.0-Linux_I2/Esm
./restartEsm.sh
./isESMRunning.sh
#cd /root/Utrade/muTrade-1.0.0-Linux_I2/
#./restartServer.sh
#./isServerRunning.sh
#cd /root/Utrade/muTrade-1.0.0-Linux_I2/
#./restartDms.sh
#./isDmsRunning.sh
#cd /root/Utrade/muTrade-1.0.0-Linux_I2/logs/hftLogs
#ls -lrth

echo -e "\nAll Started\n"
