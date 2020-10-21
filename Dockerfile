FROM ubuntu AS builder

RUN apt update && apt install -y make

WORKDIR /app
COPY . /app
RUN make release

FROM scratch
COPY --from=builder /app/build/release/ocr /ocr

ENTRYPOINT ["/ocr"]
CMD ["network"]