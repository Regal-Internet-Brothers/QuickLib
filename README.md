# QuickLib
'QuickLib' is a library primarily containing the current/experimental version of the 'QuickSock' project.

##QuickSock##
'QuickSock' is a C++ library built to make UDP network/socket programming faster and easier, while still providing IP-agnostic features. Support is currently IPV4-only. The IPV6 groundwork has already been done, however, it has not been properly tested. The IPV4 backend supports hostname resolution, but the IPV6 backend is a bit different. The current IPV6 backend does support hostname resolution, however, it does not cache results. This approach is inefficient, and will likely be overhauled later on.

##Getting Started##
To install QuickLib, you will need to build the source code with your compiler of choice. This varies from platform to platform. For Windows, you should just be able to build the project using MSBuild, or Visual Studio 2013 (Community) itself. Before doing this, however, you'll need to move the header files (Located in each project's **"src/Local"** folder) to a new folder called **"QuickLib"**; more specifically, the headers will need to be at **"INCLUDE_FOLDER_HERE/QuickLib/PROJECT_NAME_HERE"**.

That's basically it, now you can compile the projects. You could also compile the projects using library-specific local-compilation options, if available. QuickSock has the 'QSOCK_LOCAL_BUILD' preprocessor variable for this purpose. That variable will allow you to compile using the 'Local' versions of the headers.

From there, on Windows, your library files should be located at **"LIB_FOLDER_HERE/QuickLib/ARCHITECTURE_HERE"**; the files should be ***"PROJECT_NAME_HERE.lib"***. **The "ARCHITECTURE_HERE"** folder currently has the following pre-defined names as possibilities: "x86", "x64", and "ARM". If your architecture was not listed, then it has not been explicitly described, and is therefore up to the user.

In the event you are compiling full DLLs, and not static-only 'lib' files, please follow the same rules, managing the DLL files as you see fit.

Compiling these libraries on other platforms has not yet been documented. The 'QuickSock' project shouldn't need to link against very much, however.

##Supported Platforms##
The platforms supported by this library differ between "sub-projects". In the case of 'QuickSock', earlier versions have been tested on a number of *nix-like systems (Windows, Linux, OS X, etc), and support should generally still be there. The latest versions of 'QuickSock' have only been tested for x64 (64-bit) and x86 (32-bit) versions of Windows. Support for the IPV6 backend varies between systems. Extra work for lower-level implementation differences has been done to ensure implied support for most *nix-like systems. If you have trouble compiling for a specific platform, feel free to make an issue/bug report.

##TO DO##
* Make a super-class for 'QSocket' called "QStream" (Or similar) in order to create a unified system for packet I/O.
* Get the IPV6 backend working.
* Add hostname caching (Probably using a 'map') for the IPV6 backend.

##Other##
Use this library, and any related projects at your own risk. I will only develop these projects when I feel it is necessary.
