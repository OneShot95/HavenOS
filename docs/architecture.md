# HavenOS architecture

Date: 8 October 2026. This is the Phase 0 split between the shared product and the Nexus 7 (2012) device definition. It follows `docs/feasibility.md`. It does not add research.

HavenOS 0.1 is a **desktop prototype**, not a flashable image. The shell that exists today does not build a kernel, a disk image, or a tablet rootfs. No flashing has been done.

## Two definitions

**Shared HavenOS** is the product that can move to another tablet later: the shell, the offline surfaces, and the controller boundary. It does not name a bootloader, a BCT, or a device tree.

**The device definition** is everything that is true only of this Nexus 7 family: postmarketOS port `device-nvidia-tegra-armv7`, kernel `linux-postmarketos-grate`, U-Boot as primary bootloader, the `tegra30-asus-nexus7-*` device trees, firmware, and the grouper versus tilapia identity. The published device package is generic Tegra ARMv7. It sets `deviceinfo_dtb="tegra*"` and its initramfs also lists Transformer, Acer A500, and Surface RT modules. A HavenOS device image, when one is built, pins the Nexus 7 device trees. Transformer modules do not stay in the product definition.

A later Tegra 2/3/4 board in that same port can reuse the postmarketOS base. A non-Tegra tablet is a new port on any base the hardware report compared. The shared shell does not remove that porting work.

```
HavenOS (shared)
  Qt 6 Widgets shell          one process, software raster
  offline surfaces            only after the shell runs
  controller boundary         SimController only, until the Matter gates pass

Device definition (Nexus 7 2012, not in the 0.1 prototype)
  postmarketOS / Nura         Alpine, musl, armv7, apk
  device-nvidia-tegra-armv7   pin tegra30-asus-nexus7-* 
  linux-postmarketos-grate    not the Android 3.0 tree
  U-Boot primary              not approved; chainload reported broken
  identity                    grouper or tilapia, PM269 or E1565, storage size
```

## Shell

The shell is **Qt 6 Widgets**. One process. Software raster. No `QOpenGLWidget`, no QML, no shaders, no Qt Quick scene graph.

The program does not choose a Qt platform plugin. A desktop session uses the normal X11 or Wayland plugin. On a future tablet boot the intended setting is `QT_QPA_PLATFORM=linuxfb`, or `QT_QPA_FB_DRM=1` when the driver allows DRM dumb buffers. Those variables stay in the environment. LinuxFB on this tablet has not been tested. The postmarketOS port’s documented usable desktops are 2D X11 (Xfce4 and MATE on the Tegra SoC page). Those sessions show that a graphical login is described as usable. They are not the HavenOS shell. The product boots into the one HavenOS binary. It does not boot Phosh, Plasma Mobile, GNOME, Xfce, or MATE as the product UI.

The window is 1280×800 landscape. The panel’s native scan-out is 800×1280 portrait. Touch targets stay large. On the desktop, a mouse stands in for the touchscreen. Rotation and the touch matrix, if they need a fix, belong in the device profile. A 2021 owner report is the only note the UI research has for that, and it is an anecdote.

Qt comes from the distro package, not a private fork. Stay on Widgets APIs present in Debian’s 6.4/6.8 and Alpine’s 6.11. The current prototype asks for Qt 6.4 Widgets. Alpine’s `qt6-qtbase-x11` package also contains EGLFS and links a GL dispatcher. Installing that package is not a decision to render with the GPU. Select the software platform explicitly on the device.

No published RSS for this shell on a 1 GB Tegra 3 exists. On the first real boot, record idle RSS and `MemAvailable`. If the shell session sits above 150 MB RSS, stop and switch to LVGL on the framebuffer. That 150 MB figure is a budget from the UI report, not a measurement and not a forecast. GTK 3 is a fallback toolkit, not the prototype. Slint, SDL, and Flutter are not the shell. Flutter has no official Linux ARMv7 engine target in the UI report.

QML can be revisited only as an experiment inside the same Qt 6 process, behind the software backend, after a widget shell has a measured RSS on the device. It is not 0.1.

### Desktop prototype that exists now

The program is in `shell/` on the repository `main` branch. The binary name is `haven-shell`. It has not been booted on a Nexus 7. The same process keeps a light software paint for the Nexus 7, or for 1.5 GB of memory or less. A computer with at least 2 GB can use a richer software paint. That choice is labelled in the window. It is not a shader path and not a second toolkit.

The window title is “HavenOS — desktop prototype”. A banner states that it is a desktop prototype, that it is not running on the tablet, and that it is not a flashable OS image. The clock and date are the computer’s clock, labelled as local time from this computer. The first launch asks for a home name and a room, saved on this computer. Weather, devices, and battery are placeholders labelled “Not connected”. A weather look can change the painted sky and is labelled as a preview. The home footer says Wi-Fi and Matter are not connected. Home also repeats the timer state and the shopping count, and says both stay on the computer. Timer, Store, List, and Recipes are offline pages in the same process. Store has no prices and cannot take an order. Apps says nothing is installed. Settings says nothing on the page is live. Power lists Shut down, Restart, and Sleep as disabled buttons and says those actions are not implemented. The prototype cannot launch apps and cannot shut down, restart, or sleep a device.

That labelling is the architecture. Later pages may not replace it with implied live hardware.

## Matter boundary

The Matter controller is deferred. There is no Matter SDK in the image and none in the five-day plan. A labelled simulation is the only backend allowed on the Nexus 7.

The UI calls a small boundary. The Matter report defines it as:

- `listNodes()`, `read()`, `subscribe()`, `invoke()`
- `commission()` and `openCommissioningWindow()` as separate actions
- storage status: absent, simulated, or a durable path
- a transport note on each node: Wi-Fi, Ethernet, or Thread-via-external-border-router

Two backends share that boundary:

- **`SimController`**, the only backend linked for this tablet. The UI says simulation in the place the user looks, not only in a log line. It must not write fabric keys or show a device as commissioned onto a real fabric.
- **`SdkController`**, later: chip-tool interactive, matter.js, or a 64-bit controller on another machine. Same interface. Not compiled in until the gates in `docs/feasibility.md` pass.

The current prototype does not implement `SimController`. It says Matter is not connected. That is the honest 0.1 state. A simulation that appears later has to be labelled as a simulation. It does not replace “Not connected” with a picture of a real fabric.

Device records, when a simulation shows any, use the certifiable device-type IDs in the Matter report (lights `0x0100`–`0x010D` as listed there, plugs, switches, thermostat, sensors, door lock, window covering). A simulation can show lights, plugs, and sensors first. It does not pretend a lock command reached a real lock. Switches in Matter are often stateless events. The controller does not implement these types as device firmware.

No Thread stack and no border-router API on the tablet. A later status flag that a border router was seen on the LAN is not a local radio. HavenOS does not ship an OpenThread border router.

Commissioning and day-to-day control stay separate calls. HavenOS needs a controller only if it toggles devices after setup, and a commissioner only when it adds a device to its own fabric. Apple Home import does not exist. A second admin is a second commissioning. Fabric storage, if a real controller ever exists, is a durable directory, not the default temp directory (`chip_tool_kvs`). Disk size of a small fabric store is unknown. 0.1 has no fabric store.

## Device image, when one is built

postmarketOS is the device image. Userspace is Alpine (`apk`, musl, armv7). The kernel package is `linux-postmarketos-grate`, not plain mainline and not Android 3.0. Mainline has carried the Nexus 7 device trees since Linux 5.9. Grate is still the tree this port ships for Tegra display userspace.

Boot is U-Boot as the primary bootloader, extlinux on the eMMC, and a whole-disk image written to the disk U-Boot exports. There is no `deviceinfo_flash_method` on the current device package. Chainloading the stock bootloader is reported broken and is not a fallback. U-Boot is **not approved**. The install erases eMMC. grouper versus tilapia, and the PMIC revision, are confirmed before any BCT or re-crypt device name is chosen. Stable v26.06 is the preference for a stable branch (kernel 7.0.1-r0 as of 31 May 2026). Edge 7.0.1-r4 is what the 7 October 2026 images track. Neither image has been written to a device.

The product image, when it exists, is a console plus this one graphical client. It does not adopt Phosh. Prebuilt generic images (console, i3wm, xfce4, mate, sxmo) are reference downloads for the port, not HavenOS releases.

Wi-Fi, if the wiki status holds on the unit, still requires 802.11w completely disabled. The wiki says iwd works better than wpa_supplicant. That has not been retested here.

Web engines stay out of the image: no WPE, no WebKitGTK, no Qt WebEngine. Video is not part of the shell. If it is added later, the UI report says to try GStreamer or mpv in a separate process, software decode first, and to treat the Tegra VDE as an experiment. Codec limits in the reports (CAVLC H.264; no weighted prediction or CABAC in the VDPAU note) are not a promise that decode works on grouper. Nothing cited measures that.

## Deliberately not in 0.1

- A flashable image, a rootfs, a kernel build, or any write to eMMC. No flashing has been done.
- U-Boot install, SBK or BCT handling, factory restore, or APX recovery. Those wait for an identified unit, backups, and an explicit approval.
- Asking for Jake’s Linux machine. The desktop shell does not need it. That machine is for USB identification, backup, and a later flash.
- Phosh, Plasma Mobile, GNOME, Xfce, or MATE as the session the product boots.
- Qt Quick, QML, EGLFS, a GPU compositor requirement, and a private Qt fork.
- A web engine in-process or as a separate process.
- The Matter SDK, chip-tool, python-matter-server, matter.js, BlueZ commissioning, and a fabric store.
- A Thread radio or an OpenThread border router.
- Home Assistant.
- AI, live weather, live shopping, accounts, or any network call presented as a connected home.
- Cameras, GPS, the tilapia modem, BLE, GLES, and a smooth Wayland phone UI. The reports do not mark these as known working.
- Video playback and hardware decode.
- A claim that LinuxFB, DRM dumb buffers, or a 150 MB RSS figure have been measured on this tablet.
