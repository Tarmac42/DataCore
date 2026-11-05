# ADR-0001: Shared Data Foundation and Acquisition/Control Boundaries

- **Status:** Accepted as architectural direction; integrations remain proposed
- **Date:** 2026-11-04
- **Scope:** DataCore, ArchiveManager, DataAcquisition, VanBuild
- **Type:** Cross-project design decision (not an implementation claim)

## Context

ArchiveManager needs reusable typed data I/O without coupling the foundation to
archive-specific models. VanBuild also has two distinct responsibilities:
acquiring observations from physical sensors and controlling van equipment through
state machines. Independent environmental experiments, such as temperature and
humidity logging on Raspberry Pi devices, need acquisition without van-specific
control logic.

The existing DataCore v0.1.0 repository provides header-only typed record and
writer contracts. Production readers, file/database writers, sensor drivers,
acquisition services, and the proposed integrations are **not** implemented here.

## Decision

Use DataCore as the independent, reusable typed-data foundation. Keep three
domain responsibilities separate:

- **ArchiveManager:** discovers, catalogs, preserves, queries, and deduplicates
  archive data; consumes DataCore contracts as suitable.
- **DataAcquisition:** acquires, timestamps, qualifies, and publishes sensor or
  instrument observations. Drivers, sampling policies, and sensor metadata
  belong to DataAcquisition or its consuming applications, not DataCore.
- **VanBuild:** owns equipment state machines, operating modes, control policies,
  actuator commands, and UI. It consumes selected DataAcquisition observations
  through explicit interfaces.

The intended compile-time dependency direction is:

```text
ArchiveManager --------------------> DataCore
DataAcquisition -------------------> DataCore
VanBuild --------> DataAcquisition
    `------------------------------> DataCore (optional, when needed)
```

These are **intended architectural relationships**, not a declaration that
all repository dependencies or interfaces are wired today. There must be no
reverse dependencies or cycles. DataCore never imports application headers.

## Runtime boundaries

1. **Observation path:** Sensor drivers and protocol adapters publish typed
   values, provenance, timestamps, units, and quality information through a
   DataAcquisition-facing observation interface. A VanBuild control service
   may subscribe to the measurements it needs.
2. **Decision path:** VanBuild state machines interpret observations in the
   context of operating modes, safety constraints, timeouts, and equipment
   policies. Acquisition does **not** decide when pumps, heaters, or other
   actuators should operate.
3. **Actuation path:** VanBuild sends commands through a separate actuator
   interface. Sensor publication does not implicitly actuate equipment.
4. **Historical path:** Observations and events may be queued for independent
   DataCore-backed persistence or eventual ArchiveManager ingestion. Neither
   persistence nor ArchiveManager is in the real-time control path.

Acquisition reports measurement freshness/validity. Control policies decide
the safe response to missing, invalid, or stale observations. Loss of optional
history storage must not block live control; safety-critical behavior requires
separately specified fail-safe and hardware protections.

## DataCore boundary

DataCore provides portable, reusable typed-data contracts and progressively
implemented I/O mechanisms. It does **not** own universal sensor identities,
units, calibration schemas, device drivers, polling threads, equipment states,
or application-specific persistence policy. Consumers define their domain
records, mappings, and lifecycle; adapters may translate between those records
and DataCore's typed writer/reader contracts.

Do not add a global plugin registry, universal record variant, implementation
enum, cross-repository submodule, or generic application framework merely to
share these capabilities. Backends and adapters use explicit composition.

## Consequences

- VanBuild's existing Arduino/RS-485 acquisition and actuator paths can be
  refactored behind separate interfaces without coupling them to data storage.
- Independent acquisition deployments (such as attic monitoring stations) can
  reuse DataAcquisition without depending on VanBuild.
- ArchiveManager can optionally manage acquired datasets asynchronously.
- Live observations, historical measurements, and equipment events are related
  but distinct concepts; schemas and retention policies must be designed at
  the appropriate layer.
- Interfaces and failure behavior should be validated incrementally against
  real consumers before implementation.

## Deferred decisions

- Exact observation, event, subscription, and actuator interface signatures.
- Whether a separate reusable ControlCore is justified.
- Sensor-driver plugin ABI and deployment/loading lifecycle.
- Historical storage formats, batching, recovery, synchronization, and query
  services.
- Repository integration mechanics and migration of existing VanBuild services.

No production implementation, build dependency, or repository restructuring is
authorized by this ADR alone.
