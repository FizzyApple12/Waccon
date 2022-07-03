mkdir -p build
mkdir -p artifacts
cd build

export CUSTOM_CFLAGS="-DOPT_PRESETPANELS"
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
mv wacca_protocol_translator.uf2 ../artifacts/wacca_protocol_translator_left.uf2

export CUSTOM_CFLAGS="-DOPT_PRESETPANELS -DOPT_RIGHT"
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
mv wacca_protocol_translator.uf2 ../artifacts/wacca_protocol_translator_right.uf2

export CUSTOM_CFLAGS="-DOPT_PRESETPANELS -DOPT_SINGLE_TOUCH"
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
mv wacca_protocol_translator.uf2 ../artifacts/wacca_protocol_translator_single_touch_left.uf2

export CUSTOM_CFLAGS="-DOPT_PRESETPANELS -DOPT_SINGLE_TOUCH -DOPT_RIGHT"
cmake -G "Unix Makefiles" ..
make wacca_protocol_translator
mv wacca_protocol_translator.uf2 ../artifacts/wacca_protocol_translator_single_touch_right.uf2

cd ..