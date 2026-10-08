# HavenOS feasibility

Date: 8 October 2026. This note is the Phase 0 decision from three research reports. It does not add research. No tablet was attached. No image was built. No `fastboot` or `adb` command was run. Nothing was flashed.

The Qt shell that exists today is a **desktop prototype**, not a flashable image. It does not build a kernel, a disk image, or a tablet rootfs.

Sources, unchanged:

- `internal/phase0-hardware.md`
- `internal/phase0-matter.md`
- `internal/phase0-ui.md`

Where those reports disagree, the disagreement is stated below. Measurements, driver status, and boot commands that the reports do not contain are not filled in.

## Recommendation

Use **postmarketOS** for a future device image, with kernel package **`linux-postmarketos-grate`**, on the generic port `device-nvidia-tegra-armv7`.

**U-Boot as the primary bootloader is the only documented working boot path, and it is not approved.** Chainloading U-Boot from the stock Android bootloader is reported **Broken** on the grouper page, the tilapia page, and the June 2026 generic-port table. The U-Boot page says flashing U-Boot **erases all eMMC**, and that vendor Android images will not run afterward. Replacing the vendor bootloader is the supported path. It is still destructive. Do not flash without an explicit approval after the unit is identified.

**grouper versus tilapia must be confirmed first.** One U-Boot defconfig covers the family. The BCT filename and the re-crypt device name still have to match the unit. The hardware report calls a wrong BCT the documented way to target the wrong board.

No flashing has been done.

The same project is now called Nura. `postmarketos.org` redirects to `nura.eco`. Package names, image filenames, and the archived wiki still say postmarketOS. This note uses postmarketOS for the install, which is the name on the packages.

Alpine alone, Buildroot, and Yocto do not already ship this board. Buildroot is the hardware report’s fallback only if the product must be a single immutable image with no package manager. It still needs the same U-Boot install and the same kernel. Yocto (OE4T meta-tegra, as read) is a Jetson BSP and does not list grouper, tilapia, or a Tegra 3 tablet. The hardware report’s recommendation is postmarketOS because it is the only option in that comparison with a device package, a firmware package, a kernel package, install docs, and images rebuilt for this SoC family on 7 October 2026.

Prefer the v26.06 packages if the goal is a stable branch: kernel `linux-postmarketos-grate` 7.0.1-r0 as of 31 May 2026. Edge has the same kernel line at 7.0.1-r4 (1 August 2026) and is what the weekly images track. A HavenOS image was not built. A bootable generic image is realistic on paper. It cannot be proven on hardware until the tablet is identified on a machine with USB.

## Confirm the unit before any image

Do not assume a unit is grouper.

| | grouper | tilapia |
| --- | --- | --- |
| Radio | Wi-Fi | Cellular (GSM) |
| Product | Nakasi | Bach |
| Model | ME370T | ME370TG |
| Board | PM269 (TI PMIC, older) or E1565 (Maxim PMIC) | Device tree is E1565 |

Storage is 8, 16, or 32 GB. The panel is the same 1280×800 landscape / 800×1280 portrait IPS panel. The wiki prints 800×1280, which is the portrait orientation.

U-Boot’s current grouper page (`grouper_defconfig`, also the v2026.07 copy) says one defconfig covers grouper revisions and tilapia, and that U-Boot detects the board revision. re-crypt still takes `--dev grouper` or `--dev tilapia`. Preload uses `grouper.bct` or `tilapia.bct`. Older U-Boot docs disagree with that page: v2024.10 used `grouper_common_defconfig` plus a fragment, and a v2026.01 source copy still mentions reloading an Android bootloader image. The latest HTML page fetched for the hardware report does not. Use the latest page. Treat older fragments as historical.

The hardware report records identification from a TWRP adb shell, before the partition layout is replaced, and from the bootloader screen product name. This note does not repeat those commands as a procedure. The first session on a Linux machine with USB is identification and backup, not flashing. That session has not happened.

## What is published

Mirror listing `http://mirror.nura.eco/postmarketos/main/armv7/`, fetched 8 October 2026:

| Package | Filename | File date |
| --- | --- | --- |
| Kernel | `linux-postmarketos-grate-7.0.1-r4.apk` (25.3 MiB) | 1 August 2026 |
| Device | `device-nvidia-tegra-armv7-6-r0.apk` (2.7 KiB) | 2 June 2026 |
| X11 subpackage | `device-nvidia-tegra-armv7-x11-6-r0.apk` | 2 June 2026 |
| Firmware | `firmware-nvidia-tegra-armv7-6-r1.apk` (1.2 MiB) | 3 August 2026 |

Stable branch v26.06 on the same mirror: kernel `linux-postmarketos-grate-7.0.1-r0.apk` (31 May 2026), device `6-r0` (5 June 2026), firmware `6-r0` (31 May 2026).

The unpacked device package is the generic Tegra ARMv7 package, not a grouper-only package. It depends on `alsa-ucm-conf`, `firmware-nvidia-tegra-armv7`, `linux-postmarketos-grate`, and `postmarketos-base`. `deviceinfo_dtb="tegra*"`, `deviceinfo_arch="armv7"`, extlinux config is generated, and there is no `deviceinfo_flash_method` line. The kernel command line file in that package is `console=tty0`, `console=ttyS0,115200n8`, `rw`, `gpt`. The initramfs module list includes `elants-i2c`, `panel-lvds`, `panel-simple`, `smb347-charger`, and `bq27xxx-battery`, and also Transformer, Acer A500, and Surface RT modules, because the package is shared.

Three kernel version claims are in circulation. The mirror filename is the one that matches a file that exists on the day of the report.

| Source | Version |
| --- | --- |
| Archived gitlab.com pmaports recipe | 6.6.22-r5 (repository archived, recipe frozen) |
| Package index pages such as pkgs.postmarketos.org | 6.16.0-r4, build time 14 November 2025. Direct fetch on 8 October 2026 returned HTTP 500 |
| `mirror.nura.eco` edge armv7 | **7.0.1-r4** |

Live APKBUILD text was not readable (bot wall), so the git tag inside 7.0.1 was not opened. The grouper wiki kernel section still says only “Grate kernel, follows linux-next” and does not name 7.0.1.

Nura does not ship plain mainline as this device’s kernel. It ships `linux-postmarketos-grate`. The wiki’s “Mainline: yes” means the port is close to mainline. The board device trees have been in mainline since Linux 5.9 (commit 2720008f4239, 29 June 2020; present in `v5.9`, absent in `v5.8`): grouper E1565, grouper PM269, tilapia E1565, plus `tegra30-asus-nexus7-grouper-common.dtsi`. Grate is where the Tegra 2/3 display and video userspace this port ships actually lives (`xf86-video-opentegra`, `libvdpau-tegra`). Nothing found in the hardware sources says Nouveau drives this tablet. The UI report’s kernel docs agree on the boundary: until Tegra124, `drm/tegra` supports the built-in gr2d and gr3d engines; Nouveau starts at Tegra124 (K1). A mainline kernel can see the Tegra 3 GPU blocks. A working OpenGL ES userspace stack is a different question, and the April 2026 infobox still says 3D is only partial.

Do not start from the Android 4.1.2 / Linux 3.0 vendor tree. The grouper wiki says the legacy kernel can reboot under load (pmaports#201) and says to use the mainline/grate kernel instead.

### Category conflict

The port is built. It is not the top postmarketOS support tier. Edge images for `nvidia-tegra-armv7` were published on 7 October 2026. The category label disagrees by page:

| Source | Last edit | grouper | tilapia |
| --- | --- | --- | --- |
| grouper wiki infobox | 20 April 2026 | testing | not a separate row |
| generic port device table | 14 June 2026 | testing | testing |
| tilapia wiki infobox | 29 May 2025 | | community |
| Tegra 2/3/4 SoC generic-port table | 10 December 2025 | community | community |

The two newest pages say **testing**. The tilapia page and the SoC table still say **community**. Testing is the later label. The archived `gitlab.com/postmarketOS/pmaports` tree still has `device-nvidia-tegra-armv7` under `device/community`. That directory is not the 2026 category. Live recipes at gitlab.postmarketos.org returned a bot wall, so the recipe text was not re-read.

The device-package maintainer field is Robert Eckelmann. The April 2026 grouper wiki still lists David Heidelberg.

Live wiki HTML (`wiki.postmarketos.org` and `wiki.nura.eco`) returned an Anubis page on 8 October 2026. Hardware status is from Wayback snapshots plus the current binary packages. The UI report used an older grouper snapshot (oldid 56202, PDF saved 2024-03-25) and says that snapshot is not a claim about later edits.

## Peripherals

“Works” below is the wiki’s word, not a new test. Device-tree facts are from `tegra30-asus-nexus7-grouper-common.dtsi` on torvalds/linux master, 8 October 2026.

Wiki flags that the grouper infobox marks Works: flashing, USB networking, battery, screen, touchscreen, audio, Wi-Fi, Bluetooth, NFC, FDE, USB OTG, accelerometer, magnetometer, ambient light, Hall effect. 3D acceleration Partial. GPS Partial. Camera Untested. Several of those flags conflict with the same page’s body. See the disagreement list.

- **Display.** Wiki: Works. The flag’s definition is “ideally with sleep mode and brightness control”; the page does not add a separate sleep-mode log. DT: LVDS `chunghwa,claa070wp03xg` plus `panel-lvds`, with a comment that some units have a Hydis HV070WX2-1E0 and that the panels are largely compatible. Width 94 mm, height 150 mm, `rotation = <180>`, PWM backlight.
- **Touchscreen.** Wiki: Works. The input device is named `elan-touchscreen`. DT: `elan,ektf3624` at I2C 0x10, swapped and inverted axes. Initramfs lists `elants-i2c`. A 2021 owner report says MATE could rotate the picture and the touch matrix had to be fixed separately. That is an anecdote, not a lab result. Rotation belongs in the device profile.
- **Wi-Fi.** Wiki: Works, and 802.11w must be completely disabled. iwd works better than wpa_supplicant on that page. Firmware is a subpackage; the page says desktop-Linux firmware files are outdated and points at the Android bcm4330 tree. DT: SDIO on `sdmmc3`, comment “Azurewave AW-NH665 BCM4330”, compatible string `brcm,bcm4329-fmac`. Both strings are in the file. One personal note says wireless dropped often on one install. That is one user, not the wiki status. GSMArena lists 802.11 b/g/n. Band limits beyond that listing were not verified.
- **Bluetooth.** Wiki: Works, “tested with BT 4.2 and 5.0 devices.” That sentence does not say BLE, GATT, or advertising. DT: UART-C, comment “Azurewave AW-NH665 BCM4330B1”, compatible `brcm,bcm4330-bt`. Product listings used by the Matter report (GSMArena, PhoneDB) say Bluetooth 3.0. Those sources do not agree on the version label. Both reports agree that a BlueZ BLE central on HavenOS is unproven. A 2014 Android device-tree change is an AOSP userspace switch, not a Linux test. A USB BLE dongle on micro-USB OTG is also unproven here.
- **Audio.** Wiki: Works (playback, microphone, headset, buttons). The device package depends on `alsa-ucm-conf`. DT: `realtek,rt5640`, model “ASUS Google Nexus 7 ALC5642”.
- **Battery and charging.** Wiki: Works. DT: fuel gauge `ti,bq27541`, charger `summit,smb347`, `constant-charge-current-max-microamp = <1800000>`. The Tegra 3 SoC page marks CPU frequency scaling, cpuidle, thermal, and suspend as Works for the SoC. The grouper page has no suspend row. SoC-level “suspend works” is not a Nexus 7 test result.
- **RAM.** DT memory `0x40000000` (1 GiB). Default CMA pool 256 MiB, marked reusable. TrustZone reservation 2 MiB, `no-map`. The UI report’s 2024 snapshot also states quad-core 1.2 GHz Cortex-A9 and ULP GeForce. The 2026 pages used by the hardware report state Tegra 3 T30L and 1 GB and do not restate the clock.
- **GPS.** Infobox Partial. Body: Broadcom BCM4751 “unsupported by the kernel and gpsd,” status N. The body’s “unsupported” is the stronger statement.
- **NFC.** Infobox Works. Body: “wired, not tested yet.”
- **Cameras.** Infobox Untested. Body: front camera N. The DT has an `aptina,mi1040` node. A node is not a working driver.
- **Tilapia modem.** Calls, SMS, and mobile data: Broken. Body: “contribution welcome, no work done yet.”
- **USB OTG.** Wiki: Works. DT `dr_mode = "otg"`.
- **Other sensors marked Works:** accelerometer (`invensense,mpu6050`), magnetometer (`asahi-kasei,ak8974`), light (`dynaimage,al3010`; page says slightly oversensitive), Hall/lid. Proximity is tilapia-only in the body, “disabled, but works.”

A porter’s recorded stock grouper command line in the UI report includes `mem=1022M@2048M`, `vmalloc=512M`, and `tegra_fbmem=8195200@0xabe01000`. That is not the command line in the current device package (`console=tty0`, `console=ttyS0,115200n8`, `rw`, `gpt`). They are different records. This note does not merge them into one boot line.

## Graphics and the shell

A small client on the working display is realistic. A GLES phone shell is not what this port claims.

The reports disagree on the word for 3D, and they describe different layers:

- Grouper infobox (April 2026): 3D **Partial**.
- Grouper hardware table: 3D **N**, and the opening summary lists 3D as not yet supported, with GPS, cameras, and the modem.
- SoC page (December 2025): Tegra 3 GPU **Partial**, video decode/encode **Partial**.
- UI report’s 2024 grouper snapshot: feature table 3D **Partial**, and the status prose lists 3D among “not-yet supported features.”

The December 2025 SoC page says Mesa at that time was GL 1.4, not enough for XWayland (needs GL 2.1 or GLES2); Phosh runs on llvmpipe and is slow; Xfce4 and MATE run on 2D acceleration. That page names `xf86-video-tegra`. The June 2026 device package depends on `xf86-video-opentegra` and `libvdpau-tegra` instead. The package depended on today is opentegra. The GL 1.4 limitation was not re-checked against a 2026 Mesa tree. The X driver package in Alpine edge community armv7, `xf86-video-opentegra` 0.6.0_git20211025-r1, was built 24 November 2024. `libvdpau-tegra` 0_git20210517-r0 was built 7 June 2026. The SoC page says that VDPAU driver does not decode H.264 streams that use weighted prediction or CABAC. The UI report’s `libvdpau-tegra` note is hardware decode only for CAVLC H.264, with the Tegra VDE kernel driver. No source cited in either report measures decode on a grouper.

The grouper page also conflicts with itself on Phosh: some interfaces are unavailable because hardware acceleration is missing (Phosh is the example), and later Phosh is a recommended interface that “needs performance optimization, but works.” The 2024 snapshot has the same pair of sentences. Owner notes, not the feature table: one person using XFCE4 daily; another saying Phosh and XFCE4 run but are not daily-driveable; one Firefox-crash note; one note of software 3D via a grate-driver llvmpipe build on Arch Linux ARM. The grate wiki says stock Mesa llvmpipe “doesn’t work on Tegra20 or if NEON is disabled.” Tegra 3 has NEON. That sentence is not a Tegra 3 llvmpipe failure, and it is not a Tegra 3 llvmpipe success.

Prebuilt edge images rebuilt 7 October 2026 for `nvidia-tegra-armv7`: console, i3wm, xfce4, mate, sxmo-de-dwm. Compressed download sizes (HTTP `Content-Length` only; uncompressed size was not measured): console 173.1 MiB, i3wm 205.1 MiB, sxmo-de-dwm 310.9 MiB, xfce4 393.3 MiB, mate 452.7 MiB. Those images are generic Tegra ARMv7 (`deviceinfo_dtb="tegra*"`). U-Boot on the matching board is what selects the Nexus 7 device tree.

The UI report’s shell choice, which this project follows: **Qt 6 Widgets, one fullscreen process, software raster.** On a future tablet boot, the intended platform is Qt `linuxfb` (DRM dumb buffers when the driver allows). That is a documented Qt path, not a test on this tablet. The port’s usable graphical sessions in the SoC page are 2D X11 desktops (Xfce4, MATE). Those desktops are evidence the display can show a session. They are not the HavenOS product UI. EGLFS is out for the prototype: Qt documents it for boards with a GPU, on OpenGL ES 2.0, and this tablet’s 3D support is partial and not a distro-default GLES stack.

Qt 6 on Linux ARMv7 is packaged and is not a Qt Company desktop reference. Qt 6.12 supported-platforms tables show Linux desktop as `x86_64` and `arm64`. Alpine edge community armv7 has `qt6-qtbase` 6.11.1-r3 (installed size 11.9 MiB, built 2026-09-16) and `qt6-qtbase-x11` 6.11.1-r3 (installed size 13.9 MiB), which ships `libQt6Gui`, `libQt6Widgets`, and `libqlinuxfb.so`. Debian ships `qt6-base-dev` for armhf (bookworm 6.4.2, trixie/sid 6.8.2). The x11 package links `libEGL` and `libGLESv2` because it also contains EGLFS. That dependency is not a requirement to render with the GPU. Qt 5.15 standard support ended after 26 May 2025. Do not start there.

No published RSS for a Qt Widgets process on a Nexus 7 or any 1 GB Tegra 3 was found. Disk sizes above are not RAM. UI-report arithmetic, not a benchmark: one 32-bit 1280×800 buffer is 4,096,000 bytes; two buffers are 7.81 MiB; the stock `tegra_fbmem` reservation of 8,195,200 bytes is about two 32-bit 800×1280 framebuffers (800×1280×4×2 = 8,192,000). On the first boot on a tablet, record idle RSS of the shell and `MemAvailable`. If the shell session sits above **150 MB RSS**, stop and switch the UI to LVGL. 150 MB is a budget chosen so the shell stays a small fraction of 1 GB. It is not a forecast. No fps number was published. A YouTube transcript figure of about 476 MB for Phosh is not this device and is not used.

Web engines stay out of the first image. Qt WebEngine is Chromium (Alpine v3.23 armv7 installed size 201.8 MiB; the edge armv7 URL returned 404 on 8 October 2026). WebKitGTK and WPE memory figures in the UI report are anecdotes from other machines, not grouper measurements. Alpine armv7 WPE found in that report is the v3.21 build from 2024-10-22. No Nexus 7 browser benchmark was found.

## Matter

A real `chip-tool`, python-matter-server, or matter.js controller is not a current HavenOS deliverable. The official controller hosts are 64-bit. A 32-bit build is an unsupported port with no published memory measurement. The only Matter surface on this tablet is a clearly labelled simulation behind a controller interface. The simulation must not write fabric keys or show a device as commissioned onto a real fabric.

Home Assistant is out of scope. The tablet is not assumed to have a Thread radio. HavenOS should not ship an OpenThread border router.

`chip-tool`’s guide (committed 1 September 2026) says the source compiles on Linux (amd64/aarch64) or macOS, and that Raspberry Pi must use a 64-bit OS. The Canonical snap builds amd64 and arm64 only. python-matter-server documents 64-bit only; a maintainer wrote on 18 December 2023 that 32-bit Arm wheels are not supported. matter.js can target Node.js. Node 22.23.3 still publishes `linux-armv7l`. Node 24 does not ship official armv7 binaries. Node 22’s Linux armv7 tier wants kernel >= 4.18 and glibc >= 2.28. The Matter report leaves “whether HavenOS meets that floor” unknown and points at the Linux-image work. The hardware report says the userspace is Alpine with musl. Those two facts do not show a Node 22 armv7 binary running on the recommended image. Nobody measured it.

RSS of `chip-tool`, matter.js, or python-matter-server on ARMv7, Tegra 3, or a 1 GB board is **unknown**. Figures in the hundreds of megabytes in the Matter report are 64-bit Home Assistant anecdotes, not this device. This assessment does not guess that a controller fits beside a UI in 1 GB.

Onboard BLE for Matter is unproven. First commission of a Wi-Fi device that is not yet on the network is the BLE path. A device already on the LAN, including one commissioned first by Apple Home, is an on-network commission. Apple Home will not hand over fabric keys. Sharing is a second commissioning. Soft AP is not a plan: the Google primer says Wi-Fi Soft AP was not implemented in Matter SDK 1.0, and that sentence was not a 2026 device survey.

HavenOS IPv6 behavior is unknown until an image exists. The Matter 1.4.2 core spec says operational communication uses IPv6, and that on a single Wi-Fi/Ethernet link, link-local IPv6 is enough.

The call changes only with a reproducible current armhf controller or matter.js on Node 22 armv7l, a measured RSS and peak on the 1 GB board while the UI is running, a BLE central or a written limit to on-network commissioning, and IPv6 plus mDNS plus a durable fabric store. A 64-bit always-on host that runs the real controller, with the tablet as a client, also changes the call. Failing those gates, the SDK stays deferred.

## Flash and recovery risks

No image was built and no flash command was run. The steps below are the hardware report’s documented risks, not an approval to run them.

U-Boot as primary replaces the partition table. The install writes a whole disk image to the disk U-Boot exports. It is not `fastboot flash` of that image onto the Android `system` partition. A 2024 write-up records that `fastboot flash` of the postmarketOS disk image failed and that writing to the U-Boot USB disk worked. Stock fastboot still exists only until U-Boot replaces it.

The old Android layout is a worse path. pmbootstrap#1422 says the stock system partition cannot hold a rootfs over about 650 MB. The 2024 wiki snapshot gives Plasma Mobile as about 1.1 GB, and that figure is a disk image size, not RAM. U-Boot-as-primary removes that partition limit by erasing the disk. An 8 GB unit still has to be backed up first. A large desktop image is still a bad fit for 1 GiB of RAM even if the eMMC can hold it.

Before anything is written, the hardware report requires:

1. Identify grouper versus tilapia, PM269 versus E1565, and 8/16/32 GB.
2. If Android partitions are still intact, take a recovery backup and copy it off the tablet. The U-Boot page says the install erases the eMMC.
3. Save the per-device SBK and BCT. re-crypt cannot process U-Boot without that SBK. The fusee-tools README says a corrupted BCT can be replaced with a matching file from that tree’s `bct` directory. A missing SBK is not described as replaceable.
4. Keep a known-good vendor bootloader image aside. The grouper wiki says the bootloader inside the default Google factory packages often cannot be flashed, and points at bootloader 4.23, or 4.18 from the KitKat factory set. An old XDA guide says the 4.23 bootloader from nakasi JWR66Y fails a signature check and the 4.23 image from JWR66V does flash. Those reports were not retested on 8 October 2026.

If the vendor bootloader is still there, Power + Volume Down still opens the green Android fastboot screen. Recovery is a factory image for the matching product: nakasi for grouper, nakasig for tilapia. Factory flashes erase user data. They require the vendor fastboot bootloader, so they are not the recovery path after U-Boot has taken the boot partitions. Google’s factory-image page, as a static download on 8 October 2026, contained no `nakasi`, `grouper`, or “Nexus 7” row. The table may be loaded by script. A historically published nakasi JWR66V tarball still answered HTTP 200 (`Content-Length` 319716057, `Last-Modified` 24 July 2013). It was not downloaded. It is a Wi-Fi / nakasi image only.

If U-Boot was installed, or the vendor bootloader is corrupt, stock Android will not boot. Recovery is APX plus fusee-tools, then a preloaded U-Boot or a restored bootloader, and the U-Boot page says this is reversible only with backups. A dead eMMC is not a software recovery. Volume Down at U-Boot, if U-Boot itself still runs, opens USB mass storage so the disk can be rewritten without another bootloader install.

MR 4705 (January 2024) left `pmbootstrap install --split` for chainloading. The 2026 wiki says that secondary path is broken, so split is not a supported fallback.

Biggest risks named by the hardware report: a wrong-variant U-Boot install on a 14-year-old eMMC, because the install erases the disk and stock Android recovery only works before that replacement; 1 GiB RAM with a 256 MiB CMA pool; 3D that is still partial or absent depending on which paragraph you read; aging batteries and dead eMMC called out by owners on the wiki.

Jake’s Linux machine is required for every step that touches the tablet: seeing fastboot or APX, identifying the variant, copying a backup, dumping SBK and BCT, and any later write. A cloud VM can read docs, checksum a prebuilt image, and cross-build U-Boot. Processing U-Boot with re-crypt needs the per-device SBK. Do not ask for that machine until the image to write is chosen and the backup steps are agreed. Neither has happened.

## Where the reports disagree

| Topic | What the sources say |
| --- | --- |
| Support category | Newest pages: testing. Tilapia infobox (May 2025) and the December 2025 SoC table: community. |
| Kernel version | Archived recipe 6.6.22-r5, an index page 6.16.0-r4 (HTTP 500 when fetched), mirror file 7.0.1-r4. Wiki text does not name 7.0.1. |
| 3D | Infobox Partial. Hardware table and summary: not supported (N). SoC page: GPU Partial. 2024 snapshot: Partial and “not-yet supported.” |
| Phosh | Unavailable because acceleration is missing, and also “works” but needs optimization. Owner notes say it is not daily-driveable. |
| X driver name | December 2025 SoC page: `xf86-video-tegra`. June 2026 device package: `xf86-video-opentegra`. |
| GPS | Infobox Partial. Body: unsupported, status N. |
| NFC | Infobox Works. Body: wired, not tested yet. |
| Camera | Infobox Untested. Body: front camera N. |
| Bluetooth version | Wiki: tested with BT 4.2 and 5.0 devices, without saying BLE. Product listings in the Matter report: Bluetooth 3.0. BLE on Linux: unverified in both. |
| U-Boot recipe | Current page: `grouper_defconfig`. v2024.10: common defconfig plus a fragment. v2026.01 source copy still mentions an Android bootloader reload; the latest HTML does not. |
| Maintainer | Device package: Robert Eckelmann. April 2026 wiki: David Heidelberg. |
| Command line | Current device package: `console=tty0` and `console=ttyS0,115200n8`. A porter’s stock line in the UI report: `mem=1022M@2048M`, `vmalloc=512M`, `tegra_fbmem=8195200`. |
| Wiki vintage | Hardware report uses 2025–2026 snapshots. UI report’s device table is the 2024-03-25 snapshot and says so. |
| Node on this userspace | Matter report: Node 22 armv7 tier asks for glibc >= 2.28, and whether HavenOS meets that is unknown. Hardware report: Alpine musl. No measurement closes the gap. |
| Controller memory | Unknown. Other-machine anecdotes are not a 1 GB Tegra 3 budget. |
| Shell RSS | Unknown. 150 MB is a chosen gate, not a measurement. |

## What has not been done

No flashing has been done. The variant on the desk is not confirmed, because no unit was attached. SBK and BCT were not dumped. A HavenOS rootfs was not built. The desktop shell was not booted on a Nexus 7. RSS, fps, BLE, IPv6, GPS, cameras, the modem, and GLES were not measured for this product.
