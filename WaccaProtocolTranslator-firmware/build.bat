if not exist "build" mkdir build
cd build
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
cd ..