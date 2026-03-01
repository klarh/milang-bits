# milang-bits: Suggested Subprojects

Optional "batteries" for the milang programming language.
Each subproject lives in its own directory at the repo root and is imported
as a regular `.mi` module. C backing libraries use milang's built-in C FFI
(`import'` with `.h` headers) — no special infrastructure needed.

### Design principles

- **Capability-aware**: libraries that perform I/O accept the narrowest
  `world` sub-record they need (e.g. `world.fs.read`, `world.io`), following
  milang's least-privilege model.
- **Idiomatic milang**: return `Maybe` for fallible operations, use pattern
  matching for ADT dispatch, expose functions suitable for `|>` pipelines.
- **Zero-cost where possible**: lean on partial evaluation — configuration,
  constants, and pure transforms should resolve at compile time.
- **C under the hood when it matters**: milang compiles to C and has first-class
  FFI; performance-critical bits (arrays, hashing, crypto, regex) can wrap
  proven C libraries and expose a milang-friendly API.
- **Powerful domain-specific languages**: With the aggressive compile-time
  reduction inherent in milang, it often makes sense to build up domain-
  specific languages for a given application.

---

## Core Data Structures

### `strings` — Improved Strings & Ropes
The core language provides `split`, `trim`, `indexOf`, `replace`, `slice`,
`toUpper`/`toLower`, and `charAt`. This library adds what's missing:

- **Rope data structure** for efficient insert/delete/concat on large texts
  (editor buffers, log assembly, templating engines).
- `startsWith`, `endsWith`, `contains`, `padLeft`/`padRight`, `repeat`,
  `lines`, `unlines`, `words`, `unwords`.
- Unicode-aware `charCount` (codepoint length vs byte length), normalization
  (NFC/NFD), and grapheme-cluster iteration via ICU or utf8proc.
- `StringBuilder` record for efficient repeated concatenation.

### `collections` — Maps, Sets, Deques, Priority Queues
Milang's built-in `List` is a linked cons-cell list — great for functional
patterns, but O(n) for random access and missing key-value semantics.

- **HashMap / TreeMap** (key→value), **HashSet / TreeSet**.
- **Deque** (double-ended queue) backed by a ring buffer.
- **PriorityQueue** (min-heap).
- All containers return `Maybe` for lookups and are compatible with `map`,
  `fold`, `filter` via standard iteration protocol.
- C backing: hash tables can wrap a proven open-addressing implementation.

### `array` — Contiguous Growable Arrays
A flat, cache-friendly `Array` type backed by C `malloc`/`realloc`:

- O(1) index, amortised O(1) push, O(n) insert/remove.
- `map`, `filter`, `fold`, `sort`, `slice`, `zip`, `enumerate` — same API
  shape as the built-in `List` so switching is painless.
- Typed variants via sized types (`Array (Int' 32)`, `Array Float`, etc.)
  for memory-efficient storage and FFI interop.

### `vectors` — Geometric / Linear-Algebra Vectors
Fixed-size dense vectors (Vec2, Vec3, Vec4) and matrices (Mat3, Mat4):

- Dot product, cross product, magnitude, normalize, lerp, slerp.
- Element-wise `+`, `-`, `*`, `/` via user-defined operators.
- SIMD-friendly flat memory layout emitted in generated C.
- Intended for graphics, physics, and spatial work — not to be confused
  with `array` (general-purpose growable container).

### `iter` — Lazy Iteration Protocol
A unified lazy iteration abstraction that works across `List`, `Array`,
file lines, map entries, and any user-defined sequence:

- **Core protocol**: `next` returns `Maybe` — `Just {val; rest}` or
  `Nothing`. Any type that implements `next` works with all combinators.
- **Combinators**: `map`, `filter`, `take`, `drop`, `zip`, `flatMap`,
  `enumerate`, `takeWhile`, `dropWhile`, `scan`, `chunk`, `window`.
- **Collectors**: `toList`, `toArray`, `fold`, `sum`, `count`, `forEach`.
- Lazy by default — no intermediate collections are allocated. Only
  `toList`/`toArray`/`fold` force evaluation.
- Extensible via open function chaining: define `iter val = val -> MyType = ...`
  to make any type iterable.
- Enables expressive pipelines:
  `file |> lines |> filter (startsWith "#") |> take 10 |> toList`

---

## I/O & Filesystem

### `fileio` — Incremental / Streaming File I/O
The core language has `world.fs.read.file` (read whole file) and
`world.fs.write.file` / `world.fs.write.append` (write/append whole string).
This library adds:

- **Line iterator**: lazy, streaming line-by-line reads without loading the
  entire file into memory.
- **Chunked read/write**: read or write in fixed-size byte chunks.
- **Buffered writer** with explicit flush.
- **Temp files**: create-and-cleanup helpers.
- **Atomic write**: write to temp then rename, for crash-safe updates.
- Accepts `world.fs.read` or `world.fs.write` capabilities as appropriate.

### `path` — Path Manipulation (pure)
A pure library (`:~ []`) — no I/O, just string-in / string-out:

- `join`, `dirname`, `basename`, `ext`, `withExt`, `isAbsolute`,
  `normalize`, `relativeTo`.
- Glob pattern matching (`matchGlob "*.mi" filename`).
- `walkDir` (this one needs `world.fs.read`) for recursive directory traversal.

### `serialization` — JSON, TOML, CSV, MessagePack
Parse text/binary formats into milang records and lists, and serialize back:

- **JSON**: parse → milang records/lists/strings/numbers; serialize back.
  Streaming parser for large documents.
- **TOML**: natural fit for milang records.
- **CSV**: parse into list of lists or list of records (with header row).
- **MessagePack** (binary): compact serialization for IPC and storage.
- C backing: use cJSON or similar for JSON; hand-rolled or existing C libs
  for the rest.

### `binary` — Binary Data & Wire Protocols
Read and write binary data with a declarative DSL for defining formats.
Essential for file formats, network protocols, and compact serialization:

- **Primitives**: `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`,
  `f32`, `f64` — with explicit endianness (`u16be`, `u16le`).
- **Combinators**: `bytes n` (fixed-length), `cstring` (null-terminated),
  `prefixed` (length-prefixed), `padded n`.
- **Struct DSL**: define a format as a record spec, decode/encode in one pass.
- **Byte buffer**: mutable buffer backed by C `malloc` for building packets.
- Pure decode functions (`:~ []`); buffer construction may need a capability
  or return a new buffer value.
- Foundation for `serialization`'s MessagePack and for `net` protocol
  implementations.

---

## Networking

### `net` — Networking
- **TCP**: connect, listen, accept, send, receive — wrapping POSIX sockets.
- **UDP**: bind, sendTo, recvFrom.
- **DNS**: resolve hostname → IP.
- **HTTP client**: minimal GET/POST/PUT/DELETE with headers, status, body.
  Wraps libcurl or a minimal hand-rolled HTTP/1.1 client.
- **URL parsing**: scheme, host, port, path, query, fragment.
- TLS via system OpenSSL/LibreSSL when available.
- All socket functions accept a new `world.net` capability sub-record.

---

## Numerics & Data

### `math` — Extended Math
Milang can already `import "math.h"` for sin/cos/sqrt/pow. This library
provides higher-level facilities:

- **Random number generation**: PCG or xoshiro with seed management.
  `random`, `randomRange`, `shuffle`, `sample`.
- **Arbitrary-precision integers** (bigint) via GMP or mini-gmp.
- **Rational numbers**: `Rational` ADT with exact arithmetic.
- **Complex numbers**: `Complex` ADT with standard operations.
- **Statistics**: mean, median, variance, stddev, percentile.
- **Constants**: pi, e, tau, phi, etc. (resolved at compile time via partial
  evaluation).
- **Interpolation**: lerp, clamp, smoothstep, remap.

### `ndarray` — N-Dimensional Arrays (NumPy-like)
Strided, typed, multi-dimensional arrays for numerical computing:

- Shape, reshape, transpose, broadcasting.
- Element-wise arithmetic, comparison, logical ops.
- Reductions: sum, mean, min, max, argmin, argmax along axes.
- Slicing and fancy indexing.
- Basic linear algebra: matmul, dot, determinant, inverse (wrapping BLAS/LAPACK
  when available, fallback to pure C).
- Memory layout: row-major contiguous by default, with stride support for views.

### `dataframe` — Tabular Data (Pandas-like)
Column-oriented table built on `ndarray` and `array`:

- Named, typed columns. Schema introspection via milang's `fieldNames`/`fields`.
- `select`, `where`, `groupBy`, `join`, `sort`, `agg`.
- CSV and JSON import/export (via `serialization`).
- Pretty-print tables to console.

---

## Composition & Access

### `lens` — Composable Record Access
Lenses provide composable getters and setters for nested record access.
Built on milang's `getField`/`setField` introspection:

- **Core types**: `Lens` ADT wrapping a getter and setter pair.
- **Constructors**: `field "name"` creates a lens for a record field.
- **Composition**: `lens1 >> lens2` composes lenses for nested access.
- **Operations**: `view lens record` (get), `set lens value record` (set),
  `over lens f record` (modify via function).
- Pure (`:~ []`) — lenses are just pairs of functions.
- Enables expressive nested updates:
  `over (address >> city) toUpper person`

### `query` — Collection Query DSL
A LINQ-style query builder for filtering, transforming, and aggregating
in-memory collections. Leverages milang's DSL-oriented design:

- **Pipeline operations**: `from`, `where`, `select`, `orderBy`, `groupBy`,
  `limit`, `distinct`, `join`.
- **Aggregations**: `count`, `sum`, `avg`, `min`, `max`.
- Operates on any iterable (via `iter` protocol).
- Query descriptions are records — they can be inspected, composed, and
  partially evaluated at compile time when the query shape is known.
- Lighter-weight alternative to the full `dataframe` library for most
  data processing tasks.

---

## Text & Parsing

### `parsec` — Parser Combinators
A combinator library for building parsers from small, composable pieces.
Milang's `|>` pipelines and partial evaluation make this a natural fit:

- **Core combinators**: `satisfy`, `char`, `string`, `many`, `many1`,
  `sepBy`, `choice`, `between`, `chainl1`.
- **Sequencing**: `<>` (sequence two parsers), `<|>` (try alternative),
  `map` (transform result).
- **Error reporting**: position tracking, expected-vs-found messages.
- **Character classes**: `digit`, `letter`, `alphaNum`, `space`, `upper`,
  `lower`, `oneOf`, `noneOf`.
- With partial evaluation, parsers defined with literal combinators can
  reduce at compile time into efficient state machines.
- Foundation for building other libraries' parsers (`serialization`,
  `datetime`, `fmt`).

### `regex` — Regular Expressions
- Compile-once, match-many API: `compile`, `match`, `matchAll`, `test`.
- Named captures, find-and-replace, split-by-pattern.
- Unicode-aware character classes.
- C backing: wrap PCRE2 or RE2 for production quality, or a minimal NFA
  engine for zero-dependency builds.

### `fmt` — String Formatting
- `format` function with positional and named placeholders:
  `format "Hello, {0}! You have {count} items." name {count = n}`.
- Number formatting: decimal, hex, octal, binary, scientific notation.
- Padding, alignment, truncation.
- Plays nicely with `toString` extensibility and `|>` pipelines.

### `datetime` — Date, Time, & Duration
- `Date`, `Time`, `DateTime`, `Duration` ADTs.
- Arithmetic: add/subtract durations, diff between dates.
- ISO 8601 parsing and formatting.
- IANA time zone support (via system `tzdata` or embedded).
- Monotonic clock for benchmarking / elapsed-time measurement.
- `world.time` capability sub-record for "get current time" (impure).

---

## System & Interop

### `os` — OS Interaction
The core language has `world.process.exec`, `world.process.exit`,
`world.argv`, and `world.getEnv`. This library adds:

- `setEnv`, `unsetEnv`, `allEnv` (full environment snapshot).
- **Process spawning** with separate stdin/stdout/stderr pipes (not just
  `exec` which returns a string).
- Signal handling (SIGINT, SIGTERM, etc.).
- Platform detection: `os.name`, `os.arch`.
- `which` (find executable on PATH).
- Temporary directory helpers.

### `logging` — Structured Logging
- Levels: trace, debug, info, warn, error.
- Structured key-value fields (milang records map naturally).
- Pluggable sinks: stderr (colored), file, JSON-lines.
- Logger accepts `world.io` or a dedicated `world.log` capability.
- Near-zero overhead when a level is disabled (compile-time elision via
  partial evaluation where possible).

### `testing` — Test Framework
- `assert`, `assertEqual`, `assertMatch`, `assertThrows`.
- Test discovery by convention (functions named `test_*`).
- Setup/teardown hooks.
- Parameterized / table-driven tests.
- Snapshot testing (compare output against `.expected` files).
- CLI runner with colored pass/fail output and failure diffs.

### `argparse` — Command-Line Argument Parsing
- Declarative option/flag/positional definitions as milang ADTs.
- Auto-generated `--help` text (compile-time reducible when spec is literal).
- Type-safe parsing into a `ParseResult` (Ok/Err) ADT.
- Sub-command support via nested specs.
- Builds on `world.argv` (pure once the argument list is obtained).

### `color` — Terminal Colors & Styling
ANSI escape code generation for styled terminal output:

- **Styles**: `bold`, `dim`, `italic`, `underline`, `strikethrough`.
- **Colors**: 16 standard colors, 256-color, and 24-bit RGB.
- **Composable**: `style bold >> fg red >> text "error"` builds a styled
  string via `|>` pipelines.
- **Auto-detection**: check `TERM`/`NO_COLOR` environment variables to
  disable styling when output is piped or unsupported.
- Pure (`:~ []`) — produces strings with embedded escape codes.
- Pairs with `logging`, `testing`, and `argparse` help text.

### `state` — State Machine DSL
Declarative state machine definitions using milang's ADTs and pattern
matching:

- **Define machines**: states and transitions as ADT constructors and
  records.
- **Transition function**: `step machine event` returns `Just newState` or
  `Nothing` for invalid transitions.
- **Guards**: conditional transitions via pattern matching guards.
- **Hooks**: `onEnter`, `onExit`, `onTransition` callbacks.
- Pure (`:~ []`) — the machine definition and stepping are pure functions;
  side effects live in the hooks (which accept capabilities).
- Useful for protocol implementations, game logic, UI flows, and
  workflow engines.

### `graph` — Graph Data Structures
Directed and undirected graph representations with standard algorithms:

- **Construction**: `addNode`, `addEdge`, `removeNode`, `removeEdge`.
- **Adjacency**: `neighbors`, `inEdges`, `outEdges`, `degree`.
- **Traversals**: BFS, DFS with visitor callbacks.
- **Algorithms**: topological sort, shortest path (Dijkstra), cycle
  detection, connected components, strongly connected components.
- **Representation**: adjacency-list backed by `collections` maps.
- Pure (`:~ []`) for the data structure; algorithms that need ordering use
  a comparison function parameter.
- Useful for dependency resolution, build systems, data pipelines, and
  network topology.

---

## Suggested Priority

| Phase | Subprojects | Rationale |
|-------|------------|-----------|
| **1 — Foundations** | `array`, `strings`, `collections`, `iter`, `fileio`, `path` | Fill the biggest gaps in the core language; `iter` is the composability glue |
| **2 — Data & Interchange** | `parsec`, `serialization`, `binary`, `fmt`, `math`, `regex`, `lens` | `parsec` is a force multiplier for parsers; `lens` for nested data |
| **3 — Numeric & Net** | `vectors`, `ndarray`, `net`, `datetime`, `query`, `state`, `graph` | Unlocks scientific, networked, and stateful programs |
| **4 — DX & Production** | `testing`, `logging`, `os`, `argparse`, `color` | Developer experience and production-readiness |
| **5 — Advanced** | `dataframe` | Power-user feature that builds on earlier phases |
