mkdir -p build
cd build
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
cd ..