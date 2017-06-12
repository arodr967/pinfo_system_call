cd /usr/src/linux
make -j bzImage
make -j modules
make -j 
make -j modules_install
make -j install

sleep 10
