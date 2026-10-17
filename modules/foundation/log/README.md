# log

The logger. It owns the level of every project and category, the record line, the bounded queue
and the thread that empties it, the sink, the C interface and the ABI check. It depends on the
C++ standard library only.

## Public entry points

| Header | Holds |
| --- | --- |
| `SushiLog/log/log_level.hpp` | `LogLevel`, Trace 0 to Off 6; the numbers are part of the C interface |
| `SushiLog/log/log_project.hpp` | `LogProject`: a project's name and the variable that sets its levels |
| `SushiLog/log/log_category.hpp` | `LogCategory` and `make_log_category`: a project and a module |
| `SushiLog/log/log_record.hpp` | `LogRecord`, the fields a sink receives |
| `SushiLog/log/log_sink.hpp` | `LogSink`, the callable a sink is stored as |
| `SushiLog/log/delegate.hpp` | `Delegate`, the move-only callable `LogSink` is built on |
| `SushiLog/log/log_calls.hpp` | `set_log_level`, `log_level`, `set_log_sink`, `set_log_synchronous`, `log_forward_sink`, `log_at` |
| `SushiLog/log/log_front.hpp` | `SL_LOG_FRONT_AT` and `log_level_ceiling`, on which a project defines its macros |
| `SushiLog/log/log_abi.hpp` | `K_ABI_VERSION`, `abi_version` and the check a front runs on its first call |
| `SushiLog/log/log_c_sink.h` | The C interface: `SushiLogRecord`, `sl_set_log_sink`, `sl_set_log_synchronous`, `sl_forward_record` |
| `SushiLog/log/export.hpp` | `SUSHILOG_API`; empty under `SUSHILOG_STATIC` |

## Bricks under source/

Each file holds one responsibility; `logger.cpp` composes them and adds nothing.

| Brick | Holds |
| --- | --- |
| `level_table` | `LevelTable`: the atomic level of each category, the level of each project, which projects this logger owns |
| `level_environment` | `read_level_environment`: reads a project's variable once, when the table adds the project |
| `level_name` | The label of each level and the level a word names |
| `record_format` | `QueuedRecord`, `make_record`, the thread numbers and the clock |
| `own_record` | The records the logger writes about itself, under `sushilog.log` |
| `log_queue` | `LogQueue`: the bounded queue, the drop count and the drop report |
| `sink_holder` | `SinkHolder`: the sink, its swap, a sink that throws or re-enters |
| `log_writer` | `LogWriter`: the thread, synchronous delivery, the order of `FATAL`, the end at exit |
| `forward_sink` | `forwarded_record`: another logger's record as this one queues it |
| `library_path` | The file the library was loaded from, for the ABI message |
| `stderr_line` | `write_stderr_line`, the one place the logger writes to the console |
| `logger` | `Logger` and `logger()`: the composition, never destroyed |
| `log_calls`, `log_front`, `log_abi`, `log_c_sink` | The definitions of the public headers of the same name |

## Behaviour

A record passes two gates. The ceiling of its translation unit compiles a call past it to
nothing; the atomic level of its category, `WARN` unless something sets it, costs one relaxed
load. A project's variable sets its levels the first time one of its categories is used.

A level call for a project or a category adds it when it is new, reading the project's variable
first, so a call made before the first log call wins over the variable, and every category of a
project set that way starts at its level.

The queue holds 4096 records. A record that finds it full is dropped, and the next record
delivered is preceded by a `WARN` naming how many were dropped. `FATAL` waits for the queue,
is delivered on the calling thread and flushes every stream. A sink that throws, logs from
inside itself or sets the sink or the mode from inside itself loses nothing: the record and the
reason go to stderr. The logger is never destroyed; at static destruction it stops its thread
and delivers what is queued, and a later call is delivered on the calling thread. On Windows the
system ends the thread first; if it ends inside a sink call, the queue is lost and later
records go to stderr.

A front's first call compares the `K_ABI_VERSION` it was compiled with to `abi_version()` of the
loaded library; when they differ it writes one line naming the project, both versions and the
library's path, then terminates.

The tests are in `tests/foundation/log/`.
