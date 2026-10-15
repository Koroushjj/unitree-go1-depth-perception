# Third-party attribution

## UnitreeCameraSDK

- Upstream: https://github.com/unitreerobotics/UnitreecameraSDK
- Locally supplied SDK version: `v1.1.0` (`version.txt`). No source commit metadata was included in the supplied directory, so this is a version label rather than a verified commit pin.
- Original example: `examples/example_getDepthFrame.cc`.
- Original author: SunMingzhe; date: 2021.12.07; version: 1.1.0.
- Original notice: Copyright (c) 2020-2021, Hangzhou Yushu Technology Stock CO.LTD. All Rights Reserved.
- License supplied with the SDK: Mozilla Public License 2.0, reproduced in `LICENSE`.

`example_Koroush_Depthframe.cc` is the user's modified version of that example. Its contents have been preserved as supplied, and the original upstream attribution is restored here. `depth_monitor.cpp` is a further portfolio revision and carries the upstream attribution in its header. The two YAML files also derive from the SDK configuration format.

SDK-derived source, including modifications, is provided under MPL-2.0. The new monitor support code and CMake/test code are also provided under MPL-2.0. Vendor headers, compiled SDK libraries, generated SDK documentation, and the legged-motion SDK are not redistributed in this repository.

## Other dependencies and materials

OpenCV and libudev are external dependencies; their source and binaries are not bundled. The photograph `go1-platform.jpeg` was supplied in the project materials as `Position.jpeg`; it is retained without modification and is not licensed by the software LICENSE file. No third-party research papers or private administrative documents are included.
