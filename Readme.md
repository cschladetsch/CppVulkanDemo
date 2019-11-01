# Vulkan Spike

This is a library that helps with using Vulkan on all supported platforms:
* iOS
* macOS
* Windows 10

Android is under development. In any case, only ~8% of Android devices currently support Vulkan.

A Linux port would be trivial but should be attempted.

## Building

First, install VulkanSDK for your platform.
* https://www.lunarg.com/vulkan-sdk/ 

Then:

`
$ git clone --recursive TODO
$ cd VulkanSpike
$ mkdir build && cmake .. && make
`

This should work for many systems, except...

### Windows

> mkdir build; cd build; cmake ..; start vtb.sln

Then build with Visual Studio.

