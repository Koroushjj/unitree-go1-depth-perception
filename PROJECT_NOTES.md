# Project notes

## Evidence and scope

Reviewed on 2026-10-15 using the supplied `Code` directory, the local UnitreeCameraSDK v1.1.0 example and build files, the scientific-project report, and the project images. The report describes network/SSH setup, SDK exploration, and the camera demonstrator. Its conclusion explicitly states that automatic movement reaction has not been implemented.

The report appendix uses a 0.8 m obstacle threshold, while the actual supplied `.cc` uses 1.0 m. The repository treats the actual source file as authoritative. A historical screenshot shows `CV_32FC1` output, including zeros and values around `2.3e-05` printed as meters. This supports the need for scale verification and invalid-depth handling; it does not demonstrate accurate metric ranging or detection performance.

## Original contribution versus upstream

The original SDK example simply opens a configuration, starts capture/stereo computation, requests colorized depth, displays the frame, and stops on Esc. The coursework changes the request to non-colorized depth, logs the matrix format, reads the center float value, adds a one-meter threshold message, and implements separate float/16-bit clipping and JET visualization. Custom YAML changes are listed in the README.

## Review findings and portfolio changes

| Finding in coursework | Treatment in revised monitor |
| --- | --- |
| Custom YAML filename is never opened | Required configuration-path argument |
| Zero, negative, or NaN depth can be reported as a clear path | Explicit unknown state for invalid/out-of-range measurements |
| Float and 16-bit units are assumed from type | Required scale argument; hardware calibration still required |
| 16-bit depth is visualized but not classified | Both supported types feed the same tested classification policy |
| Esc is skipped when frame reads fail | Event polling on failed reads, unknown-frame display, bounded inter-call timeout |
| Unsupported image formats are passed to display | Clear unsupported-format error |
| Cleanup occurs only on normal exit | Scoped capture/stereo cleanup during exceptions |
| Per-frame logging floods the terminal | Approximate one-second diagnostic interval |
| A single-pixel result is described as a clear path | Wording limited to the center sample |

The original files remain available unchanged. The revised monitor is a later code-quality improvement, not a claim that these changes were implemented or tested during the original coursework.

## Validation

On 2026-10-15, the portable test executable built with MSVC 19.44 in C++14 mode and passed **21 checks with zero failures**. Cases cover meter/millimeter scaling, exact threshold and range boundaries, zero/negative/tiny measurements, NaN, infinity, overflow, and invalid numeric arguments.

CMake 3.31 configured and built the tests-only target, and CTest passed **1/1 test**. The first CMake compiler probe encountered the machine's existing program-database-manager mismatch (`C1902`). A retry succeeded after disabling debug-symbol generation for the local probe with `CMAKE_CXX_FLAGS_DEBUG=/Od /Ob0 /MDd` and `CMAKE_EXE_LINKER_FLAGS_DEBUG=/DEBUG:NONE /INCREMENTAL:NO`. Those machine-specific overrides are not imposed on other users in the project.

The camera application was not compiled or run: this environment is Windows, WSL access was denied, and a compatible Linux SDK/OpenCV runtime and Go1 camera were not available. Full camera build, linking, capture, GUI behavior, SDK configuration compatibility, frame timing, and physical distance accuracy remain unverified. No robot commands were executed.

The three supplied coursework files were compared by SHA-256 against their originals. Documentation links and package contents were checked; no SDK binaries, WSL archives, credentials, or report cover-page personal details are included.

## Materials selected for publication

Published inputs are the coursework code, the two supplied configuration files, the platform photograph, and the SDK license text, together with the new monitor, tests, CMake setup, and documentation. The SDK distribution is an external dependency. The WSL disk archive, third-party papers, full report/presentation, and unrelated employment/administrative materials are excluded. The report was used only as background; its personal cover-page details and login instructions are not copied into the repository.
