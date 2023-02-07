# data reader

* A toy parser for regular int/float arrays with "dynamic" typing

* Reads multi-dimensional integer/float arrays from an input file (or stdin).
  Rejects irregular and ill-typed input arrays.

  Example usage:
  ```
  $ make
  $ echo "[[1.4, 3.], [3, 7.2]]" | ./data_reader
  ```

  Supports comma separation, but currently does *not* enforce it -- where commas
  are omitted, whitespace separates values. As an example, the three strings
  `"[4,2]"`, `"[4 2]"`, and `"[4, 2]"` are all parsed as the array `[4, 2]`.

  `data_reader.c` based in very large part on
  [this](https://github.com/diku-dk/futhark/blob/b66b3049532193471cc739dd23b00d6f75b7b0a3/rts/c/values.h)
  (input parsing for Futhark compiled executables).
