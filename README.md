# QuickLib
'QuickLib' is a library primarily containing the current/experimental version of the 'QuickSock' project.

##QuickSock##
'QuickSock' is a C++ library built to make UDP network/socket programming faster and easier, while still providing IP-agnostic features. Support is currently IPV4-only. The IPV6 groundwork has already been done, however, it has not been properly tested. The IPV4 backend supports hostname resolution, but the IPV6 backend is a bit different. The current IPV6 backend does support hostname resolution, however, it does not cache results. This approach is inefficient, and will likely be overhauled later on.

##Supported Platforms##
The platforms supported by this library differ between "sub-projects". In the case of 'QuickSock', earlier versions have been tested on a number of *nix-like systems (Windows, Linux, OS X, etc), and support should generally still be there. The latest versions of 'QuickSock' have only been tested for x64 (64-bit) and x86 (32-bit) versions of Windows. Support for the IPV6 backend varies between systems. Extra work for lower-level implementation differences has been done to ensure implied support for most *nix-like systems. If you have trouble compiling for a specific platform, feel free to make an issue/bug report.

##Other##
Use this library, and any related projects at your own risk. I will only develop these projects when I feel it is necessary.

##TO DO##
* Make a super-class for 'QSocket' called "QStream" (Or similar) in order to create a unified system for packet I/O.
* Get the IPV6 backend working.
* Add hostname caching (Probably using a 'map') for the IPV6 backend.
