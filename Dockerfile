FROM alpine:3.8

RUN set -ex && \
    apk add --no-cache gcc musl-dev cmake cmake clang clang-dev make g++ libc-dev linux-headers

WORKDIR /usr/src/tcp_server_client

COPY . .

RUN chmod +x build.sh
RUN ./build.sh

EXPOSE 65123

