# DataCore architecture foundation

DataCore owns reusable typed data contracts, independent of applications. It must
never import ArchiveManager, DataAcquisition or VanBuild headers, services or
configuration. Application-specific records, persistence mappings and resource
construction belong in application composition or explicitly reviewed adapters.

## Dependencies and modules

```text
ArchiveManager ------> DataCore
DataAcquisition -----> DataCore (future integration)
VanBuild ------------> DataCore (optional future integration)
```

These arrows are design intent; this assignment changes neither existing app.
No submodules, vendored application code, global registry, implementation-selection
enum, universal record variant or application framework is introduced.

| Module | Present contract | Deferred work |
| --- | --- | --- |
| models | Ordinary typed payloads; optional RecordEnvelope aggregate | Domain-specific schemas remain with consumers |
| writers | RecordWriter<Record>, synchronous write/flush | CSV, JSON, SQLite implementations |
| readers | Reserved documented boundary | Concrete typed read requirements |
| serialization | Reserved documented boundary | Explicit record/field codecs and schemas |
| errors | Error diagnostic base | Adapter-specific categories where justified |

Records require no common superclass or runtime type tag. A writer is specialized
for a concrete record type at compile time. Two incompatible record types cannot
be passed through the same writer interface. RecordEnvelope is optional: source
is an opaque identifier, observed_at is caller-supplied UTC nanoseconds, payload
is a statically typed owning value. Units, identity and timestamp provenance remain
part of the consumer's record contract; DataCore invents none.

## Ownership, errors and extension

Composition owns concrete writers. Each writer owns its stream/connection through
RAII; input references are borrowed only during synchronous write. Any retained
values must be copied into writer-owned storage. Instances require serialized,
non-reentrant use; constness is not a thread-safety promise. Destructors release
resources without throwing. Explicit flush exposes pending failures.

Error carries a diagnostic, while allocation failures and payload-specific
exceptions may propagate unchanged. A failed write may leave partial output;
there is no implicit retry, batch atomicity or durability guarantee. Flush does not
universally mean fsync or transaction commit. No output is claimed durable merely
because an interface call returned. Future backends must specify these semantics.

Add a backend through a dedicated concrete class, explicit constructor dependencies
and a typed mapping. Select it in application composition, with no changes to a
central enum/dispatcher. Add dependencies only to the backend's reviewed target;
SQLite types must stay out of the portable writer contract. Proposed backends:

- CSV: explicit ordered columns, quoting/encoding, null and partial-row behavior.
- JSON: explicit field mapping, framing, numeric/invalid-byte rules and failure semantics.
- SQLite: explicit schema, transaction and connection ownership, bound values,
  rollback and commit/durability rules. No schema or migration is implied today.

Persistent archive records, filesystem ingestion, folder watching and safety/control
operations remain application responsibilities. No production code is moved.

## Build and validation

CMake exports relocatable DataCore::DataCore with C++20 as a usage requirement.
Tests verify owning record copies, ordered typed dispatch, explicit flush failure,
standalone headers and a separate consumer of the installed package. Linux and
macOS CI run the same build/test/install sequence. Test doubles are not production
writers. Review the API, error contract and backend-specific durability semantics
before implementing any writer, reader or serialization module.
