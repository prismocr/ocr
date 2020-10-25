FROM gcc:10.2.0 AS builder

WORKDIR /app
COPY . /app
RUN make release

FROM scratch
COPY --from=builder /app/build/release/ocr /ocr

ENTRYPOINT ["/ocr"]
CMD ["network"]
