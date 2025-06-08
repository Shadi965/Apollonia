FROM ubuntu@sha256:1e622c5f073b4f6bfad6632f2616c7f59ef256e96fe78bf6a595d1dc4376ac02

COPY ./build/Apollonia /usr/local/bin/Apollonia
RUN apt update \
    && apt upgrade -y \
    && apt install libavformat-dev libsqlite3-dev -y\
    && rm -rf /var/lib/apt/lists/*


WORKDIR /var/lib/Apollonia/

RUN chmod +x /usr/local/bin/Apollonia
    
ENTRYPOINT ["/usr/local/bin/Apollonia", "-p", "80", "-d", "./apollo.db", "-c", "./covers/"]

EXPOSE 80
EXPOSE 8080