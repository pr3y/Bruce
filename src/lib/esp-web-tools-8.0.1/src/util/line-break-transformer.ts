export class LineBreakTransformer implements Transformer<string, string> {
  private chunks = "";

  transform(
    chunk: string,
    controller: TransformStreamDefaultController<string>
  ) {
    // Append new chunks to existing chunks.
    this.chunks += chunk;
    // For each line breaks in chunks, send the parsed lines out.
    const lines = this.chunks.split("\r\n");
    this.chunks = lines.pop()!;
    lines.forEach((line) => controller.enqueue(line + "\r\n"));
  }

  flush(controller: TransformStreamDefaultController<string>) {
    // When the stream is closed, flush any remaining chunks out.
    controller.enqueue(this.chunks);
  }
}
