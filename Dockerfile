FROM ubuntu:latest AS build

# installs necessary packages for SDL_ttf
RUN apt-get update && apt-get install -y build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libpipewire-0.3-dev \
libwayland-dev libdecor-0-dev liburing-dev libfreetype6-dev libfreetype6 \
&& rm -rf /var/lib/apt/lists/*


# installs the three sdl packages needed for project
RUN git clone https://github.com/libsdl-org/SDL.git \
&& cd SDL && cmake -B build \
&& cmake --build build && cmake --install build

RUN git clone https://github.com/libsdl-org/SDL_image.git \
&& cd SDL_image && cmake -B build \
&& cmake --build build && cmake --install build

RUN git clone https://github.com/libsdl-org/SDL_ttf.git \
&& cd SDL_ttf && cmake -B build \
&& cmake --build build && cmake --install build

WORKDIR /app

RUN git clone https://github.com/michaelolawoye/MessageBoardApp.git \
&& cd MessageBoardApp && make CREATE_CLIENT_OUT

RUN apt-get update && apt-get install -y libpng-dev libjpeg-dev libbrotli1 zlib1g libpng16-16 \
&& rm -rf /var/lib/apt/lists/*

FROM ubuntu:latest
COPY --from=build /app/MessageBoardApp /MessageBoardApp
COPY --from=build /usr/local/lib/libSDL3.so.0 /usr/lib/
COPY --from=build /usr/local/lib/libSDL3_ttf.so.0 /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libfreetype.so.6.20.1 /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libbrotlidec.so.1 /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libpng16.so.16 /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libbrotlicommon.so.1 /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libbrotlienc.so.1 /usr/lib/

RUN apt-get update && apt-get install -y \
libx11-6 libxext6 libxrandr2 libxcursor1 libxinerama1 libxfixes3

RUN ldconfig

CMD ["/MessageBoardApp/build/client_main.exe"]
