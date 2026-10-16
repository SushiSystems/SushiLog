# SushiLog

SushiLog is the one logger of the Sushi libraries: SushiRuntime, SushiDSP, SushiBLAS, SushiAI and
SushiEngine all link it, so a process that loads several of them holds one queue, one thread and
one sink. Each project keeps its own log macros, its `LogProject` and its level variable, such as
`SR_LOG_LEVEL`, and writes through SushiLog.

It is a C++17 library that depends on the C++ standard library only. CMake installs it as the
package `SushiLog`, with two targets: `SushiLog::SushiLog`, a shared library, and
`SushiLog::SushiLogStatic`, a static one whose symbols stay hidden, for a plug-in a host may load
twice.

```bash
pip install -e ./cli
sl build
sl test --label small --label medium
```

The manual starts at [`docs/README.md`](docs/README.md); how to use the logger is in
[`docs/guides/LOGGING_GUIDE.md`](docs/guides/LOGGING_GUIDE.md).

SushiLog is source-available under the PolyForm Noncommercial License 1.0.0; see
[`LICENSE`](LICENSE) and [`COMMERCIAL.md`](COMMERCIAL.md).
