# Capability Annotations for FFI Libraries

Effect annotations use `:~ [effects]`. Pure functions get no annotation (or `:~ []`).
Only `unsafe` is used — for functions that carry C references and mutate things directly.
String allocations in normal function operation don't need annotation.

## Design: Safe/Unsafe Split

Each library gets a safe main API and (where applicable) a separate `mutable.mi`
for in-place mutation. Regex gets a pure one-shot API plus `compiled.mi` for reuse.

---

## regex/regex.mi — Pure one-shot API

All functions compile+use+free internally. No lifecycle exposed.

```
test pattern subject -> Int             (pure)
match pattern subject -> String         (pure)
matchStart pattern subject -> Int       (pure)
matchLen pattern subject -> Int         (pure)
capture pattern subject group -> Str    (pure)
captureCount pattern subject -> Int     (pure)
replace pattern subject repl -> Str     (pure)
replaceAll pattern subject repl -> Str  (pure)
findAll pattern subject -> List Str     (pure)
split pattern subject -> List Str       (pure)
isValid pattern -> Int                  (pure)
```

## regex/compiled.mi — Reusable compiled regex

```
compile pat -> Regex                    :~ [unsafe]  (allocates C object)
free re -> ()                           :~ [unsafe]  (deallocates)
test re subject -> Int                  (pure read)
match re subject -> String              (pure read)
matchStart re subject -> Int            (pure read)
matchLen re subject -> Int              (pure read)
capture re subject group -> String      (pure read)
captureCount re subject -> Int          (pure read)
replace re subject repl -> String       (pure read)
replaceAll re subject repl -> String    (pure read)
findAll re subject -> List String       (pure read)
split re subject -> List String         (pure read)
```

---

## collections — Split into 4 modules

### collections/hashmap.mi

```
new cap -> HashMap                      :~ [unsafe]  (C allocation)
free m -> ()                            :~ [unsafe]
size m -> Int                           (pure read)
get m key -> Maybe Int                  (pure read)
getOr m key default -> Int              (pure read)
has m key -> Int                        (pure read)
put m key val -> ()                     :~ [unsafe]  (mutates)
remove m key -> ()                      :~ [unsafe]  (mutates)
clear m -> ()                           :~ [unsafe]  (mutates)
```

### collections/deque.mi

```
new cap -> Deque                        :~ [unsafe]
free d -> ()                            :~ [unsafe]
size d -> Int                           (pure)
peekFront d -> Int                      (pure)
peekBack d -> Int                       (pure)
get d idx -> Int                        (pure)
pushBack d val -> ()                    :~ [unsafe]
pushFront d val -> ()                   :~ [unsafe]
popBack d -> Int                        :~ [unsafe]
popFront d -> Int                       :~ [unsafe]
clear d -> ()                           :~ [unsafe]
```

### collections/heap.mi

```
new cap -> MinHeap                      :~ [unsafe]
free h -> ()                            :~ [unsafe]
size h -> Int                           (pure)
peek h -> Int                           (pure)
push h val -> ()                        :~ [unsafe]
pop h -> Int                            :~ [unsafe]
```

### collections/intset.mi

```
new cap -> IntSet                       :~ [unsafe]
free s -> ()                            :~ [unsafe]
size s -> Int                           (pure)
has s val -> Int                        (pure)
add s val -> ()                         :~ [unsafe]
remove s val -> ()                      :~ [unsafe]
clear s -> ()                           :~ [unsafe]
```

---

## array/array.mi — Value-oriented API

Functions that return new arrays (sort, reverse, map, filter, etc.) don't mutate
the original. The caller gets a new C-backed array they must eventually free.

```
fromList xs -> Array                    :~ [unsafe]  (C allocation + fill)
toList a -> List Int                    (pure)
length a -> Int                         (pure)
get a i -> Int                          (pure)
indexOf a val -> Int                    (pure)
contains a val -> Int                   (pure)
map f a -> Array                        :~ [unsafe]  (allocates new C array)
filter f a -> Array                     :~ [unsafe]  (allocates new C array)
sort a -> Array                         :~ [unsafe]  (copy + in-place sort)
reverse a -> Array                      :~ [unsafe]  (copy + in-place reverse)
slice a start end -> Array              :~ [unsafe]  (C allocation)
copy a -> Array                         :~ [unsafe]  (C allocation)
concat a b -> Array                     :~ [unsafe]  (C allocation)
fold f acc a -> a                       (pure)
sum a -> Int                            (pure)
toString a -> String                    (pure)
free a -> ()                            :~ [unsafe]
```

## array/mutable.mi — In-place mutation

```
new cap -> Array                        :~ [unsafe]
set a i val -> ()                       :~ [unsafe]
pushBack a val -> ()                    :~ [unsafe]
popBack a -> Int                        :~ [unsafe]
swap a i j -> ()                        :~ [unsafe]
reverseMut a -> ()                      :~ [unsafe]
sortMut a -> ()                         :~ [unsafe]
clear a -> ()                           :~ [unsafe]
```

---

## binary/binary.mi — Sequential read/write

Binary is inherently about mutation (writing to buffer, advancing read cursor).

```
-- Lifecycle
new cap -> Buffer                       :~ [unsafe]
free b -> ()                            :~ [unsafe]

-- Query (non-mutating reads)
length b -> Int                         (pure)
position b -> Int                       (pure)
remaining b -> Int                      (pure)

-- Write (all mutate buffer)
writeU8 b val -> ()                     :~ [unsafe]
writeI8 b val -> ()                     :~ [unsafe]
writeU16be b val -> ()                  :~ [unsafe]
writeU16le b val -> ()                  :~ [unsafe]
writeI16be b val -> ()                  :~ [unsafe]
writeI16le b val -> ()                  :~ [unsafe]
writeU32be b val -> ()                  :~ [unsafe]
writeU32le b val -> ()                  :~ [unsafe]
writeI32be b val -> ()                  :~ [unsafe]
writeI32le b val -> ()                  :~ [unsafe]
writeU64be b val -> ()                  :~ [unsafe]
writeU64le b val -> ()                  :~ [unsafe]
writeCstring b s -> ()                  :~ [unsafe]

-- Read (advance cursor = mutation)
readU8 b -> Int                         :~ [unsafe]
readI8 b -> Int                         :~ [unsafe]
readU16be b -> Int                      :~ [unsafe]
readU16le b -> Int                      :~ [unsafe]
readI16be b -> Int                      :~ [unsafe]
readI16le b -> Int                      :~ [unsafe]
readU32be b -> Int                      :~ [unsafe]
readU32le b -> Int                      :~ [unsafe]
readI32be b -> Int                      :~ [unsafe]
readI32le b -> Int                      :~ [unsafe]
readU64be b -> Int                      :~ [unsafe]
readU64le b -> Int                      :~ [unsafe]
readCstring b -> String                 :~ [unsafe]
readBytes b n -> String                 :~ [unsafe]

-- Utilities
toHex b -> String                       (pure)
slice b start end -> Buffer             :~ [unsafe]
seek b pos -> ()                        :~ [unsafe]
reset b -> ()                           :~ [unsafe]
```

---

## Implementation Notes

1. **Regex one-shot functions** need verification that milang evaluates all `with`
   bindings (including the internal `free` call). If not, a forcing trick is needed.

2. **Collections split** requires each sub-module to `import'` the same C FFI but
   wrap only its own functions. The C code stays in one file.

3. **Array value API** implements `sort`/`reverse` as copy+mutate (caller gets new
   array, original unchanged). This matches Haskell's Data.Vector behavior.

4. **Free in safe API**: kept in the main module since it's lifecycle management,
   not algorithmic mutation. Users need it regardless of which API tier they use.
