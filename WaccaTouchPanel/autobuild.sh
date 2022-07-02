mkdir -p build
mkdir -p artifacts
cd build

export CUSTOM_CFLAGS="-DOPT_PANEL1"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_1.uf2

export CUSTOM_CFLAGS="-DOPT_PANEL2"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_2.uf2

export CUSTOM_CFLAGS="-DOPT_PANEL3"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_3.uf2

export CUSTOM_CFLAGS="-DOPT_PANEL4"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_4.uf2

export CUSTOM_CFLAGS="-DOPT_PANEL5"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_5.uf2

export CUSTOM_CFLAGS="-DOPT_PANEL6"
cmake -G "Unix Makefiles" ..
make wacca_touch_panel
mv wacca_touch_panel.uf2 ../artifacts/wacca_touch_panel_6.uf2

cd ..