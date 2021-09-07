FROM madduci/docker-alpine-cpp:gcc-6

WORKDIR /usr/src/tcp_server_client

COPY . .

RUN chmod +x build.sh
RUN ./build.sh

EXPOSE 65123

CMD ["sh", "tcp_server"]


