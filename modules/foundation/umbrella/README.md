# umbrella

`SushiLog/SushiLog.hpp`, which includes every public C++ header of the `log` module so a
consumer writes one include. It leaves `SushiLog/log/log_c_sink.h` out: a C host or a plug-in
that joins through the C interface includes it on its own. It depends on `log`; no module
includes it.

The test is in `tests/foundation/umbrella/`.
