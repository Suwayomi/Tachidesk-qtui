## Contributing
I don't currently have a road map as i'm working on getting basic features implemented, if you want to help, hop onto [the Suwayomi discord server](https://discord.gg/DDZdqZWaHA) and send `@norm#8295` a message.

## Building
1. Clone the repo, be sure to initialize submodules
2. Install qt5 and optionally ninja
3. From the root dir run:
    1. `cmake -DCMAKE_BUILD_TYPE:STRING=Debug -B./.build -G Ninja` leave off ninja if you didn't install it
    2. `cmake --build .build --config Debug --target all -- -j12` (or how ever many threads you want to build with)
