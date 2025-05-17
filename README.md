# rainbowCompressor
rainbow compressor for pre-compression of files to reduce entropy

testing purposes only for now WIP


🧩 In the current version:

The compressor does not insert any separators like spaces or hex markers.
Compressed output is a binary stream where:
Some bytes are literal characters from the original input.
Some bytes are 1-byte tokens (e.g., \x01, \x02, etc.) representing trigram phrases.
✅ Why this usually works:

The compressor only replaces exact trigrams (e.g., "the", "com", "dat") with a unique byte, e.g., \x01 to \xFF.
Each 1-byte token is never confused with regular data because:
The dictionary is saved separately (.dict file).
The decompressor uses the reverse mapping to decode any special tokens.
⚠️ But here’s the catch:

If the original input includes raw binary data (like \x01 as part of the text), the token byte might clash with actual data.
This is fine for text files, but for arbitrary binary files (e.g., images, zip files), you’d risk corruption.
🛡️ Solutions (if you want robustness):

1. Prefix Token Range
Use token range above 127 (e.g., \x80 to \xFF) to reduce collision with ASCII data.
2. Escape Literal Conflicts
If a raw input byte matches a token (say \x01), escape it with a prefix (e.g., \x00\x01 = literal 0x01).
Now:
\x01 = token for "the"
\x00\x01 = literal byte 0x01
3. Use bit-packing
Store compressed data in a custom binary format where every token is clearly delineated, e.g.,:
0x00 = literal
0x01-0xFF = dictionary token
This adds 1 byte of overhead per token, but is safe for binary data.
4. Or use markers (e.g., if going for a textual hex-based format)
Instead of binary, store hex tokens like <01>, <02> (larger, but readable).
Easy to debug, compressible after (e.g., with xz or brotli).


✅ Best Practice for Your Use Case

If you're compressing text data only (logs, source code, web content), then:

You're fine using the current raw byte token approach.
If you want to handle any file type safely, I’d recommend:

Adding a flag byte before every token/literal, or
Escaping literals that match your token range.
