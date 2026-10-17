# text

Builds text from a format and values, for log records and for any other message, such as the
text of an error. The module is header-only, depends on the C++ standard library alone, and is
exported on its own as `SushiLog::text`, so a project formats text without linking the logger.

## Public entry points

`text_format.hpp`:

| Name | What it does |
| --- | --- |
| `format_text(format, args...)` | Writes each `{}` with the next argument through `operator<<`; `{{` and `}}` are literal braces; no format specs. An 8-bit integer is written as a number, a scoped enum with no `operator<<` as its value, a null C string as `(null)`; a `{}` with no argument left becomes `{?}`, and extra arguments are counted at the end as `{+N}` |

`value_text.hpp`:

| Name | What it does | Replaces |
| --- | --- | --- |
| `fixed(value, precision)` | A number with exactly that many digits after the point | `{:.2f}` |
| `general(value, precision = 6)` | The shorter of fixed and scientific form | `{:g}`, `{:.6g}` |
| `scientific(value, precision)` | Scientific form | `{:.3e}` |
| `hex(value, width)` | Lower-case hexadecimal, zero-padded to the width | `{:08x}` |
| `right(value, width)` | Any streamable value right-aligned in the width | `{:>8}` |
| `left(value, width)` | Any streamable value left-aligned in the width | `{:<8}` |

A value that needs a width, a precision or hex goes through a helper first, then into a format:
`format_text("loss {} at step {}", fixed(loss, 4), step)`. Helpers compose: `right(fixed(v, 2), 8)`
replaces `{:8.2f}`.

## Tests

`tests/core/text/` holds one test file per header.
