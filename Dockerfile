FROM ubuntu

RUN apt update && apt install -y make

WORKDIR /app
COPY . /app
RUN make

ENTRYPOINT /app/build/release/ocr network