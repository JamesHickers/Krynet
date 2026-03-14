g++ src/krynet_linux_secure.cpp \
  -I sciter/include \
  -L sciter/bin.linux/x64 \
  -lsciter \
  -std=c++17 \
  -O2 \
  -o krynet
