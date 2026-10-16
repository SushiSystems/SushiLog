# FAQ

**Where do my records go?** To stderr, one line each, unless a sink is set. See
[`LOGGING_GUIDE.md`](LOGGING_GUIDE.md), "Where records go".

**How do I see one module's debug records?** Set the project's variable, such as
`SR_LOG_LEVEL=warn,memory=debug`. See [`LOGGING_GUIDE.md`](LOGGING_GUIDE.md), "Choosing what you
see".

**I called `set_log_level` and nothing changed, and a WARN says the logger has seen no
category of my project.** You set the level through another copy of SushiLog than the one the
project logs through, such as a plug-in's static copy. See [`LOGGING_GUIDE.md`](LOGGING_GUIDE.md).

**How do I write records to a file or to my engine's log?** Set a sink. See
[`LOGGING_GUIDE.md`](LOGGING_GUIDE.md), "Sending records elsewhere".

**My plug-in has its own SushiLog. How do its records reach the host?** Through the C interface.
See [`LOGGING_GUIDE.md`](LOGGING_GUIDE.md), "Plug-ins".

**The process ended with "was compiled against SushiLog ABI".** See
[`LOGGING_GUIDE.md`](LOGGING_GUIDE.md), "The ABI message".

**How do I build, test or check SushiLog?** With `sl build`, `sl test` and `sl check`. See
[`CLI_GUIDE.md`](CLI_GUIDE.md).

**How do I test the installed package?** `sl test --package`. See [`CLI_GUIDE.md`](CLI_GUIDE.md).
