if not exist "build" mkdir build
cd build
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
cd ..