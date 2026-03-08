# milang-bits

Optional "batteries" libraries for the [milang](https://github.com/klarh/milang)
programming language. Each library lives in its own directory and is imported as a
regular `.mi` module.

See the [milang documentation](https://klarh.github.io/milang/html) for the
language reference.

## Usage

Import a library from your milang source file using a relative path:

```
math = import "../math/math.mi"

main world = world.io.println (toString (math.factorial 10))
```

Libraries that use C FFI (array, binary, collections, regex, sdl) require the
corresponding system libraries to be installed.

## Libraries

### Data Structures

| Library | Description |
|---------|-------------|
| **[array](array/)** | Contiguous growable arrays backed by C malloc/realloc. O(1) index, amortised O(1) push. Provides `arr_new`, `arr_push`, `arr_get`, `arr_set`, `arr_map`, `arr_filter`, `arr_fold`, `enumerate`, `zip`. |
| **[collections](collections/)** | C-backed HashMap (string→int), Deque (ring buffer), MinHeap (priority queue), and IntSet. |
| **[graph](graph/)** | Directed graph with edge-list representation. Construction, neighbor queries, in/out degree, sources/sinks, edge filtering. |

### Text & Parsing

| Library | Description |
|---------|-------------|
| **[strings](strings/)** | Extended string utilities: case conversion (`camel_to_snake`, `snake_to_camel`), char predicates (`is_alpha`, `is_digit`), `repeat`, `center`, `chars`, `reverse`. |
| **[fmt](fmt/)** | String formatting with `interpolate` (positional `{}`), `pad_left`/`pad_right`, `surround`, `join_with`. |
| **[parsec](parsec/)** | Parser combinator library. Core combinators (`char_p`, `string_p`, `satisfy`), repetition (`many`, `many1`, `sep_by`), sequencing (`seq`, `then_p`), alternation (`choice`), and transforms (`map_p`). |
| **[regex](regex/)** | Regular expressions via PCRE2. `compile`, `match_regex`, `match_all`, `test_regex`, `replace_first`, `replace_all`, `split_regex`, named captures. |

### Serialization & Binary

| Library | Description |
|---------|-------------|
| **[serialization](serialization/)** | JSON serialization (`to_json`, `to_json_array`), CSV parsing (`parse_csv`, `parse_csv_table`, `format_csv`), INI parsing (`parse_ini`). |
| **[binary](binary/)** | Binary data read/write with explicit endianness. Byte buffers, `write_u8`/`u16`/`u32`/`u64`, `read_u8`/`u16`/`u32`/`u64`, hex conversion. |

### I/O & System

| Library | Description |
|---------|-------------|
| **[fileio](fileio/)** | Convenience wrappers for `world.fs`: `read_file`, `write_file`, `append_file`, `read_lines`, `write_lines`, `exists`. |
| **[path](path/)** | Pure path manipulation: `basename`, `dirname`, `extension`, `stem`, `join_path`, `is_absolute`, `normalize`. |
| **[argparse](argparse/)** | Command-line argument parsing. Declarative specs with `flag`, `option`, `positional`, auto-generated `--help`. |

### Numeric & Query

| Library | Description |
|---------|-------------|
| **[math](math/)** | Numeric utilities: `abs`, `sign`, `clamp`, `min`/`max`, `factorial`, `pow`, `is_even`/`is_odd`, `min_list`/`max_list`. |
| **[query](query/)** | LINQ-style collection query DSL: `from`, `where_`, `select`, `order_by`, `group_by`, `limit`, `distinct`, `count_of`, `sum_of`, `avg_of`, `min_of`, `max_of`. |
| **[iter](iter/)** | List combinators beyond the prelude: `takeWhile`, `dropWhile`, `find`, `findIndex`, `partition`, `intersperse`, `zip_with`, `chunks`, `windows`, `span`, `uniq`. |

### Composition & State

| Library | Description |
|---------|-------------|
| **[lens](lens/)** | Functional optics for nested record access. `field`, `compose`, `view`, `set`, `over`, `path_lens`, `first_lens`, `last_lens`. |
| **[state](state/)** | State machine DSL. Define states/events/transitions, `step` to advance, `valid_events` to query. Pure (`:~ []`). |

### Graphics

| Library | Description |
|---------|-------------|
| **[sdl](sdl/)** | SDL2 graphics bindings via FFI. Window management, 2D rendering (rects, lines, points), text rendering (SDL2_ttf), event handling, render-to-texture canvas. Includes [bouncing ball](sdl/examples/bouncing_ball.mi) and [drawing canvas](sdl/examples/canvas.mi) examples. |

### Testing & Output

| Library | Description |
|---------|-------------|
| **[testing](testing/)** | Lightweight test assertions: `assert_eq`, `assert_neq`, `assert_true`, `assert_contains`, `assert_starts_with`. Returns PASS/FAIL strings. |
| **[color](color/)** | ANSI terminal colors and styling: `bold`, `italic`, `underline`, 16-color, 256-color, and RGB `fg`/`bg`. `strip` to remove escape codes. |

## System Requirements

- [milang](https://github.com/klarh/milang) compiler
- GCC or Clang (milang compiles to C)
- For **sdl**: `libsdl2-dev`, `libsdl2-ttf-dev`
- For **regex**: `libpcre2-dev`

## Running Tests

Each library has tests in its `tests/` directory:

```sh
./milang run array/tests/test_array.mi
./milang run parsec/tests/test_parsec.mi
```

Run all 26 tests:

```sh
for f in $(find . -path '*/tests/test_*.mi' -not -path './bugs/*' | sort); do
  echo -n "$f: "
  timeout 30 ./milang run "$f" > /dev/null 2>&1 && echo "OK" || echo "FAIL"
done
```

## License

MIT — see [LICENSE](LICENSE).
