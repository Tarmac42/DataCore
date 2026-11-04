# DataCore

Application-independent C++20 typed data contracts in `tarmac::data`.
This initial release establishes ownership and extension boundaries. There are no
production CSV, JSON or SQLite writers and no external runtime dependencies.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
cmake --install build --prefix /your/chosen/prefix
```

Consumers use `find_package(DataCore 0.1 CONFIG REQUIRED)` and link
`DataCore::DataCore`. Supply the install prefix through `CMAKE_PREFIX_PATH`.
The initial installable library is INTERFACE/header-only because its contracts
are templates; no empty implementation files or artificial binary are needed.

- `models/RecordEnvelope.hpp`: optional owning timestamp/source/payload aggregate.
- `writers/RecordWriter.hpp`: typed synchronous write/flush contract.
- `Error.hpp`: data-operation error boundary.
- `readers/`, `serialization/`, `writers/{csv,json,sqlite}/`: documented future modules.
- `tests/`: contract tests, standalone headers and an installed-package consumer.

CI builds and tests Linux and macOS, including installation and package consumption.
See [architecture](docs/architecture.md) for lifetime and error rules. Interfaces
are preliminary and require review before production backends or application use.
No license has been selected; this foundation does not grant a license.
