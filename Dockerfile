
########################################################################################################################
# build stage
########################################################################################################################

FROM alpine:3.19.0 AS build

RUN apk update && \
    apk add --no-cache \
        build-base \
        cmake;

WORKDIR /Networking

COPY config/ ./config/
COPY public/ ./public/
COPY src/ ./src/

WORKDIR /Networking/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --parallel 8;

########################################################################################################################
# 
########################################################################################################################

from alpine:3.19.0

RUN apk update && \
    apk add --no-cache 

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

COPY --chown=shs:shs --from=build \
    ./simplehttpserver/build/src/simplehttpserver \
    ./app/

EXPOSE 80

CMD ["/bin/sh"]

ENTRYPOINT [ "./app/simplehttpserver" ]
