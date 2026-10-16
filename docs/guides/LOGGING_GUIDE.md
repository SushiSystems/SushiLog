# Logging guide

How to log through SushiLog from a Sushi library or a program, where the records go, how to
choose which records you see, and how to hand them to a file or to a host's own logging system.
The rules a log call follows are the `logging` skill of SushiSkills; this page is how to use the
library.

## Where records go

Every record is one line on stderr, written and flushed once per record:

```
[12:04:31.207] [WARN] [sushiruntime.memory] [t3] [usm_allocator.cpp:88] share refused for 64 MiB on device 1; falling back to a host copy
```

Source: not compiled

The fields are the local time to the millisecond, the level, the project and module, the number
SushiLog gave the thread on its first record, the file and line of the call, then the message.
Stdout stays yours.

Records leave the calling thread through a queue of 4096 and reach the sink on SushiLog's own
thread. When the queue is full a record is dropped, and the next record delivered is preceded by
a `WARN` that says how many were dropped. A `FATAL` record waits for the queue, is delivered on
the calling thread and flushes every stream, so the reason a process ends is the last line
before it ends.

## Logging from a project

A project declares its `LogProject` once, a category per module, and its macros on
`SL_LOG_FRONT_AT`, passing a ceiling it computes in each translation unit. The test front of this
repository is the shape:

```cpp
namespace SushiLogTests
{
    /** @brief Holds the ceiling of this translation unit, as a project's front computes it. */
    [[maybe_unused]] constexpr SushiLog::LogLevel K_SAMPLE_LOG_CEILING =
        SushiLog::log_level_ceiling(SAMPLE_ENABLE_LOGGING != 0);
} // namespace SushiLogTests

/** @brief Logs at @p level in @p category through this translation unit's ceiling. */
#define SAMPLE_LOG_AT(level, category, ...)                                                      \
    SL_LOG_FRONT_AT(SushiLogTests::K_SAMPLE_LOG_CEILING, level, category, __VA_ARGS__)
```

Source: `tests/foundation/log/sample_front.hpp`

A call past the ceiling compiles to nothing: `ERROR` when the project's enable switch is 0,
`DEBUG` under `NDEBUG`, `TRACE` otherwise. A call below its category's level costs one relaxed
load and evaluates none of its arguments.

## Choosing what you see

Every category logs at `WARN` until something sets it. Each project reads its own variable the
first time one of its categories is used: a level for all its modules, then any number of
`module=level` pairs, in any case.

```
SR_LOG_LEVEL=off
SA_LOG_LEVEL=warn,autograd=trace
```

Source: not compiled

These two lines silence SushiRuntime and follow SushiAI's autograd module, in one process. A
value SushiLog cannot read is reported once on stderr, and that project stays at `WARN`.

In code, `set_log_level(project, level)` sets every category of a project and
`set_log_level(category, level)` sets one, which the project call then leaves alone. A call for
a category made before the first log call wins over the variable. A call for a project none of
whose categories SushiLog has seen changes nothing and writes one `WARN` naming the project:
you set it through a logger that is not the one the project logs through, such as the private
copy of a plug-in.

## Sending records elsewhere

`set_log_sink` replaces stderr with your own callable; an empty sink restores stderr. The sink
receives each field apart, as a `LogRecord`, so you can send one project's records to their own
file, or file each field where a host's logging system keeps it. The record is valid only during
the call, and a sink never logs, nor sets the sink, a level or the mode; if it does, SushiLog
writes the record and the reason to stderr instead of waiting forever. A sink that throws loses
nothing: the exception and the record go to stderr.

`set_log_synchronous(true)` first waits for the queue, then delivers every record on the calling
thread. Tests use it to read records in order; a host that wants records in the order they were
made uses it too.

A host outside C++ uses the C interface in `SushiLog/log/log_c_sink.h`. The consumer test does
both:

```cpp
    std::string message;
    SushiLog::set_log_synchronous(true);
    SushiLog::set_log_sink(SushiLog::LogSink([&message](const SushiLog::LogRecord& record)
    {
        message = std::string(record.message);
    }));
    SL_LOG_FRONT_AT(SushiLog::log_level_ceiling(true), SushiLog::LogLevel::Warn,
                    K_PACKAGE_LOG_CATEGORY, "through the {} target", CONSUMER_TARGET);

    sl_set_log_sink(&keep_message, nullptr);
```

Source: `tests/package/consumer.cpp`

The strings of a `SushiLogRecord` are not NUL-terminated, and its `size` says which fields the
logger that filled it knows; read only the fields your own `sizeof(SushiLogRecord)` and `size`
both cover. A host whose records should join SushiLog's stream hands each to
`log_forward_sink()`, or `sl_forward_record` from C: the record passes no level gate, keeps its
time and call site, and gets SushiLog's thread number and line.

## Plug-ins

A plug-in a host may load twice links `SushiLog::SushiLogStatic`, whose symbols stay inside the
plug-in. Its private logger joins the host's at start-up, before its first log call: the host
hands it `sl_forward_record`, and the plug-in calls `sl_set_log_synchronous(1)` and
`sl_set_log_sink` with it, so every record of the plug-in enters the host's queue. A
`LogSink` never crosses a library boundary; the C interface does.

Everything else links `SushiLog::SushiLog`, the shared library, which a process loads once. On
Windows its `sushilog.dll` has to sit beside each program that loads it; the build copies it
there for the tests and the package consumer, and the install puts it under `bin/`.

## The ABI message

A front's first call compares the ABI version its headers name to the one the loaded library
holds. When they differ the process ends with one line on stderr:

```
[SushiLog] sushiruntime was compiled against SushiLog ABI 1, and the loaded C:/app/sushilog.dll has ABI 2; rebuild sushiruntime against the SushiLog it loads. The process ends.
```

Source: not compiled

It means the program found a SushiLog other than the one the named project was built with,
usually an older `sushilog.dll` earlier on the search path, or a project not rebuilt after a
SushiLog upgrade. Rebuild that project against the SushiLog the process loads, or put the
matching library beside the program.
