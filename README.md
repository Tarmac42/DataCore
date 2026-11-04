# DataCore

**DataCore** is an application-independent C++20 library for typed data records and extensible data I/O contracts. It is intended to let applications write, read, and exchange structured information without coupling their domain logic to CSV, JSON, SQLite, or another storage format.

> **Status: architectural foundation (v0.1.0).** This repository currently provides tested, header-only contracts, not production CSV, JSON, or SQLite readers/writers.

## What exists today

- `tarmac::DataCore` namespace and self-contained C++20 public headers.
- `models/RecordEnvelope.hpp`: optional, owning record envelope with caller-provided source and timestamp.
- `writers/RecordWriter.hpp`: typed, synchronous `write`/`flush` contract; record types are checked at compile time.
- `Error.hpp`: common data-operation diagnostic type.
- An installable CMake interface target, `DataCore::DataCore`.
- Contract tests, standalone-header compilation, installed-package consumer verification, and Linux/macOS CI.

`readers/`, `serialization/`, and `writers/{csv,json,sqlite}/` are reserved for future implementations. They are **not** functional backends yet.

## Architecture

```text
ArchiveManager ---------┐
DataAcquisition --------┼──> DataCore
VanBuild (optional) ----┘
```

These are **intended future dependencies**, not integrations already completed. DataCore has no dependencies on the applications.

DataCore supplies reusable data contracts and low-level I/O capabilities. Each consumer defines its own domain record types, schema and storage mappings, persistence policies, and application lifecycle. In particular, ArchiveManager's catalog-specific writer and locking/recovery rules do not automatically become generic DataCore features.

See [Architecture](docs/architecture.md) for the detailed contract and extension rules.

## Build and test

Requires a C++20-capable compiler and CMake 3.20 or newer.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
cmake --install build --prefix /your/chosen/prefix
```

Consumer projects can use the installed CMake package:

```cmake
find_package(DataCore 0.1 CONFIG REQUIRED)
target_link_libraries(my_application PRIVATE DataCore::DataCore)
```

Set `CMAKE_PREFIX_PATH` to the install prefix if CMake cannot find the package. The initial library is `INTERFACE`/header-only; there is no runtime DataCore binary.

## Ownership and error contracts

Concrete writers will be owned by their composing application or service and must manage streams/connections through RAII. `write` borrows its input during the synchronous call; retained values must be copied. Use one writer instance serially, without reentrant calls. Destructors must not throw.

A successful `write` or `flush` **does not imply durable storage**. Partial output, rollback, atomic publication, transaction commits, and fsync semantics must be specified separately by each future backend. The current API does not provide automatic retries or implicit batch atomicity.

## Planned development

- Review and stabilize typed writer contracts and error/ownership semantics.
- Define concrete CSV and JSON mappings, escaping, framing, and failure behavior.
- Define SQLite schema/transaction/connection boundaries before implementing its writer.
- Design readers and serialization based on real consumer requirements.
- Introduce new implementations as separate, explicitly composed components; avoid central format enums or a global registry.

No application is required to use DataCore, and no application-specific code should be imported into this repository.

## Development and licensing

Follow [AGENTS.md](AGENTS.md) for development rules. Changes should be small, tested, and reviewed before merging; the CI workflows cover Linux and macOS.

**License:** No license has been selected. The presence of source code does not itself grant permission to reuse or redistribute it.
