1. Download qt5 source code from https://download.qt.io/archive/qt/5.15/
2. sudo apt-get install libpcre2-dev  libdouble-conversion-dev libmysql-ocaml-dev libxkbcommon-x11-dev libsqlite3-dev  libxcb-icccm4-dev libxcb-image0-dev \ 
   libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-xinerama0-dev libxinerama-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev \
   libxcb-xinerama0-dev  libxinerama-dev  libssl-dev   libjpeg-dev  firebird-dev libdbus-1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libharfbuzz-dev \
    libpng-dev unixodbc-dev libpq-dev freetds-dev flite1-dev libspeechd-dev libudev-dev libzstd-dev libx11-xcb-dev libglu1-mesa-dev "^libxcb.*"   build-essential
3. ./configure ./configure -confirm-license --prefix="/opt/qt5_15_0" -opensource -static
4. make && sudo make install
5. clone this repo
6. cmake . && make
