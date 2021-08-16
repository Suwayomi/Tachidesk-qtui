### contributing
I don't currently have a road map as i'm working on getting basic features implemented, if you want to help, hop onto discord and send me a message.

### building
clone the repo, be sure to initialize submodules
install qt5 and optionally ninja

from the root dir run:
  `cmake -DCMAKE_BUILD_TYPE:STRING=Debug -B./.build -G Ninja` leave off ninja if you didn't install it
  `cmake --build .build --config Debug --target all -- -j12` (or how ever many threads you want to build with)