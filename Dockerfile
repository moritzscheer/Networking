
# Use the latest Alpine base image
FROM alpine:latest

# Instal all dependencies required for the project
RUN apk update && \
    apk add gcc

EXPOSE 80

# Copies the sourcecode into the container
COPY . .

# Runs this command to compile the code to an executable
RUN make build

# Opens shell for Logging
CMD ["/bin/sh"]
