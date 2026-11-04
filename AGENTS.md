# DataCore development conventions

Use C++20, small cohesive components, explicit borrowed lifetimes and RAII.
Document C++ files/types/functions with Doxygen, including errors and ownership.
Keep public headers self-contained. No global mutable state, service locator,
central implementation-selection enum or generic orchestration framework.
Use one matching header/source pair per significant class; header-only contracts
and cohesive values need no empty implementation files.

DataCore must never depend on ArchiveManager or vanbuild. DataCore is application
independent; DataAcquisition may eventually consume DataCore through explicit
CMake composition. No cross-repository submodules or copied application code.
Do not implement deferred backends, drivers, threads or daemons without approval.

Build with CMake, run CTest, check installed-package consumption where applicable,
and run git diff --check. Verify Linux and macOS CI; report actual results.
Use isolated fixtures; do not access production archives or hardware in tests.
Develop subsequent changes on feature branches and stop for review before merging.

Public API convention: use namespace `tarmac::DataCore` and matching
`<tarmac/DataCore/...>` include paths. `tarmac` denotes ownership, not an
extra software dependency. Preserve the `DataCore::DataCore` CMake target.
