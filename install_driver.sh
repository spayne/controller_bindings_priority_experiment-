#!/bin/bash
cp build_linux/libcontroller_bindings_test_driver.so test_driver_root/bin/linux64/
/home/sean/.steam/steam/steamapps/common/SteamVR/bin/linux64/vrpathreg adddriver $PWD/test_driver_root
/home/sean/.steam/steam/steamapps/common/SteamVR/bin/linux64/vrpathreg 
