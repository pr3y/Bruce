export class TimestampTransformer implements Transformer<string, string> {
  transform(
    chunk: string,
    controller: TransformStreamDefaultController<string>,
  ) {
    const date = new Date();
    const h = date.getHours().toString().padStart(2, "0");
    const m = date.getMinutes().toString().padStart(2, "0");
    const s = date.getSeconds().toString().padStart(2, "0");
    controller.enqueue(`[${h}:${m}:${s}]${chunk}`);
  }
}
