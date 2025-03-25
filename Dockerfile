FROM ubuntu:latest

COPY ./build/Apollonia /usr/local/bin/Apollonia
RUN apt update \
    && apt upgrade -y \
    && apt install libavformat-dev -y\
    && rm -rf /var/lib/apt/lists/*

RUN chmod +x /usr/local/bin/Apollonia

ENTRYPOINT ["/usr/local/bin/Apollonia", "/mnt/music"]

EXPOSE 80
EXPOSE 8080