# Glossary

The words this repository's documents use. A term defined in code names the header that holds it.

| Word | Meaning |
| --- | --- |
| Project | A library that logs, with its name and the variable that sets its levels: `LogProject` in `log_project.hpp` |
| Category | A project and one of its modules: `LogCategory` in `log_category.hpp` |
| Front | A project's own log macros, defined on `SL_LOG_FRONT_AT` in `log_front.hpp` |
| Ceiling | The most detailed level a translation unit compiles log calls for: `log_level_ceiling` |
| Sink | The callable each record reaches: `LogSink` in `log_sink.hpp`; empty means stderr |
| Owned project | A project one of whose categories this logger has seen |
| Forwarding | Entering another logger's record into this logger's queue: `log_forward_sink` or `sl_forward_record` |
| ABI version | The version of SushiLog's types and calls: `K_ABI_VERSION` in `log_abi.hpp` |
