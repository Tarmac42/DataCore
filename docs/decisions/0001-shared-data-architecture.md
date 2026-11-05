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

## Acquisition clients and sensor-driver API

**DataAcquisition defines the driver-facing API; acquisition clients implement
and deploy concrete drivers.** A Raspberry Pi deployed in the van or an attic is
one example of an acquisition client. Each client can host multiple drivers for
locally attached or remotely reachable sensors and instruments.

The acquisition engine depends only on the DataAcquisition-defined source
interface, never on a specific manufacturer, bus, connection technology, or
driver implementation. A driver encapsulates its own transport and sensor
protocol, including I2C, SPI, OneWire, RS-485/serial, USB, Bluetooth, TCP/IP,
MQTT, or ingestion from externally produced files. A source need not be a
single physical sensor: it may represent an instrument, Arduino node, or
aggregated feed exposing multiple measurement channels.

The **required API capabilities** are architectural requirements, not
committed C++ method signatures:

- Identify the source and its distinct measurement channels through stable
  IDs and descriptive metadata.
- Provide an explicit initialization/start/stop lifecycle and communicate
  connection status and failures.
- Deliver typed observations with timestamps, provenance, units or unit
  references, and measurement-quality information.
- Support scheduled/polled sources and unsolicited/push-based sources without
  forcing callers to know the transport.
- Define threading, ownership, backpressure, restart, and callback lifetime
  behavior before a concrete driver interface is finalized.
- Permit simulated drivers for testing without real sensor hardware.

**Deployment boundary:** acquisition drivers execute on the acquisition
client (for example, the van's Raspberry Pi). DataAcquisition orchestrates
collection and publication through the common API; downstream applications
consume observations without loading or calling driver-specific code.
Acquisition clients may operate offline and forward recorded observations
later, but storage and forwarding must not be prerequisites for live
VanBuild equipment control.

Initial drivers may be compiled and explicitly composed C++ implementations.
A versioned, C-compatible plugin ABI for dynamically loaded drivers is a
separate future design decision; this ADR does not prescribe runtime loading,
a global registry, or a concrete interface signature.

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
- Exact sensor-source API signatures, driver lifecycle/threading details, and plugin ABI/loading semantics.
- Historical storage formats, batching, recovery, synchronization, and query
  services.
- Repository integration mechanics and migration of existing VanBuild services.

No production implementation, build dependency, or repository restructuring is
authorized by this ADR alone.
