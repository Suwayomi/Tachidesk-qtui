## Contributing
I don't currently have a road map as i'm working on getting basic features implemented, if you want to help, hop onto [the Suwayomi discord server](https://discord.gg/DDZdqZWaHA) and send `@norm#8295` a message.

## Building
1. Clone the repo and initialize submodules
    - `git clone https://github.com/Suwayomi/Tachidesk-qtui.git`
    - `git submodule init`
    - `git submodule update`
2. Install the dependencies
    - `gcc` and `g++`
    - `cmake >= 3.14`
    - `qt5 >= 6.2.0` (must include development pacakges!)
        - `qt6-base`
        - `qt6-websockets`
        - `qt6-quickcontrols`
        - `qt6-quickcontrols2`
    - `ninja` (optional)
4. From the project root dir run:
    - Debug config
        1. `cmake -DCMAKE_BUILD_TYPE:STRING=Debug -B./.build -G Ninja` leave off the `-G Ninja` part if you didn't install ninja
        2. `cmake --build .build --config Debug --target all -- -j12` (or how ever many threads you want to build with)
    - Release config
        1. `cmake -DCMAKE_BUILD_TYPE:STRING=Release -B./.build -G Ninja` leave off the `-G Ninja` part if you didn't install ninja
        2. `cmake --build .build --config Release --target all -- -j12` (or how ever many threads you want to build with)
5. The built binary can be found inside `.build`
