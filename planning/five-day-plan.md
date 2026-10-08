# Five-day plan

Start date: Thursday 8 October 2026. End date: Monday 12 October 2026.

This plan covers the desktop prototype and the notes that lock its scope. It does not promise a flashed tablet. No flashing has been done. grouper versus tilapia is not confirmed. U-Boot is not approved, because chainloading the stock bootloader is reported broken and the install erases eMMC.

The shell is a **desktop prototype**, not a flashable image.

## HavenOS 0.1

The minimum viable release is the honest desktop shell plus the Phase 0 notes.

Included:

- `docs/feasibility.md`, `docs/architecture.md`, and this plan. Architecture is locked once these three files exist.
- The Qt 6 Widgets process already built locally: branch `shell/qt6-prototype`, commit `b5aae3efe7446f7f9dc9901ad8415e9a1dcc70f2`, sources in `internal/shell-prototype/`, screenshot `media/shell-dashboard.png`.
- A 1280×800 window that says it is a desktop prototype, that it is not running on the tablet, and that it is not a flashable OS image.
- Clock from the computer, labelled as such. Weather, devices, battery, Wi-Fi, and Matter stay “Not connected” until a later surface is explicitly labelled. Power actions stay disabled. Apps stay empty until an offline page is actually in the process.
- Haven Store, and offline shopping and recipes, only as extra pages inside that same process, and only after the shell still runs. They are offline. They do not call a network. The Phase 0 reports do not specify a catalog, prices, or accounts, so those pages do not invent live stock.

Not included, and not scheduled this week:

- A flashed tablet, a rootfs, a kernel, or a write to eMMC.
- U-Boot, SBK, BCT, fastboot, or APX.
- Jake’s Linux machine. The desktop shell continues without it.
- A Matter SDK, chip-tool, matter.js, python-matter-server, or a real commission.
- AI.
- A web engine (WPE, WebKitGTK, Qt WebEngine).
- Qt Quick, Phosh, or any other session as the product UI.
- A GitHub publish. Push to https://github.com/OneShot95/HavenOS is blocked until `cursor[bot]` can write there. Local work does not wait on that.

0.1 can ship as the honest shell even if the store and recipe pages slip. Those pages do not land by weakening the labels. Missing them is not a reason to flash.

## Whose machine

| Work | Where |
| --- | --- |
| These notes, the Qt prototype, offline pages, screenshots | Cloud Linux, or whatever machine already builds Qt 6 Widgets. Not Jake’s. |
| Seeing fastboot or APX, identifying grouper versus tilapia, copying a backup, dumping SBK and BCT | Jake’s Linux machine, later. |
| Flashing | Jake’s Linux machine, only after the variant is confirmed, a backup exists, the image to write is chosen, and there is an explicit approval. |

The hardware report says not to ask for that machine until the image to write is chosen and the backup steps are agreed. This week does not choose an image and does not agree a flash. Do not ask Jake to switch machines for HavenOS 0.1.

## Day order

### Day 1 — Thursday 8 October 2026 — lock the architecture

Write the three Phase 0 notes from the hardware, Matter, and UI reports. Do not open a new research pass. Where the reports disagree, the notes say so. Mark the shell as a desktop prototype.

Done when `docs/feasibility.md`, `docs/architecture.md`, and `docs/five-day-plan.md` exist and are non-empty. That lock is this day. No code has to change for the lock to count.

### Day 2 — Friday 9 October 2026 — keep the prototype honest

Build and run `haven-shell` from `internal/shell-prototype/`. Confirm the banner, the 1280×800 size, the “Not connected” slots, the disabled power buttons, and the empty Apps page. Refresh `media/shell-dashboard.png` only if the window still shows those labels.

Do not add a platform plugin inside the program. Do not add QML, OpenGL, a web view, or a Matter library. Do not retitle the window as an installed OS.

If the build fails, this day is the fix. Store and recipe work does not start on a shell that does not run.

### Day 3 — Saturday 10 October 2026 — same shell, still honest

Use the running prototype. Check Home, Apps, Settings, and Power. Anything that implies a live tablet, a live radio, a commissioned fabric, or a shutdown path gets labelled or removed.

Leave Wi-Fi, battery, weather, and Matter on “Not connected”. The architecture allows a `SimController` later. These five days do not implement it and do not add the SDK behind it.

Still no device image. Still no call for Jake’s machine.

### Day 4 — Sunday 11 October 2026 — Haven Store, only if the shell runs

If `haven-shell` still starts and the honesty checks from days 2 and 3 hold, add one offline Haven Store page in the same process.

The page says it is offline. It does not request the network, sign in, or show a price as live. The Phase 0 reports do not define a catalog. Use a static list inside the prototype, or an empty labelled state. Do not present either as a shop that can take an order.

If the shell does not run, this day stays on the shell. The store waits.

### Day 5 — Monday 12 October 2026 — offline shopping and recipes, only if the shell runs

If the shell still runs after day 4, add offline shopping and recipes in the same process. Same rule: labelled offline, no network, no AI meal planner, no account, no claim that a store received an order.

End the day on the desktop prototype. Record what runs. Do not describe it as installed on a Nexus 7. Do not open a flash procedure.

If day 4 did not land, day 5 finishes the store page or stays on shell honesty. It does not skip ahead to Matter or to flashing.

## Stop conditions

Stop and stay on the shell if a change would make the prototype look installed, connected, or flashable.

Do not start Matter work because a page has an empty devices slot. The Matter report’s gates (a current 32-bit or Node 22 controller, a measured RSS on the 1 GB board, BLE or a written on-network limit, IPv6 and a durable store) are unmet. RSS on this class of board is unknown. Do not fill that gap with a guess.

Do not switch toolkits this week. LVGL is the fallback only after a tablet boot records shell RSS above the 150 MB budget in the UI report. That boot is outside these five days. The 150 MB figure is a budget, not a measurement.

Do not publish a download. GitHub is still empty of this work until `cursor[bot]` can push. The local commit is the 0.1 artifact.

## After Monday

The next hardware step, whenever it is approved, is still identification and backup on Jake’s machine. It is not a flash. postmarketOS, `linux-postmarketos-grate`, and U-Boot-as-primary remain the recommendation in `docs/feasibility.md`, and U-Boot stays behind an explicit approval after grouper versus tilapia is known.
