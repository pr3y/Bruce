export declare class LineBreakTransformer implements Transformer<string, string> {
    private chunks;
    transform(chunk: string, controller: TransformStreamDefaultController<string>): void;
    flush(controller: TransformStreamDefaultController<string>): void;
}
