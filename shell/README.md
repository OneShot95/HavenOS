# HavenOS shell

Desktop UI prototype, not a flashable OS image. One Qt 6 Widgets process paints a 1280×800 home panel with the raster engine. It does not build a kernel, a disk image, or a tablet rootfs.

The clock and date use the computer's clock. The painted sky follows that clock and is checked about once a minute. Weather, devices, battery, Wi-Fi, and Matter are not connected. The first launch asks for a home name and a room, then skips Wi-Fi, Matter, and AI because they are not available in this desktop prototype. Settings can change the saved name and room. Timer is an offline countdown. Settings also has a Weather look control. Off leaves the weather card as Not connected. A preview only changes the painted sky.

Haven Store is a sample list written into the prototype. It has no prices and cannot take an order. Shopping is a list saved on this computer. Recipes are three notes written into the prototype. None of those pages use the network. Home repeats the timer state and the shopping count, and says both stay on this computer.

The program does not choose a Qt platform plugin. A desktop session uses the normal X11 or Wayland plugin. Set `QT_QPA_PLATFORM` only from the environment (for example `offscreen` in the smoke test below).

## Build

From the repository root:

```
sudo apt-get install -y qt6-base-dev cmake g++
cmake -S shell -B shell/build
cmake --build shell/build
```

## Run

```
./shell/build/haven-shell
```

Open the same window fullscreen:

```
./shell/build/haven-shell --fullscreen
```

## Smoke test

```
QT_QPA_PLATFORM=offscreen HAVEN_SMOKE=1 ./shell/build/haven-shell
```

The process exits 0 after the window is shown. The CTest entry runs that same check:

```
ctest --test-dir shell/build --output-on-failure
```

## Scene preview hooks

These variables choose a painted scene for a screenshot. They are not live weather. When `HAVEN_PREVIEW_HOUR` is set, the clock shows that hour and :00. A normal run keeps the computer's local clock.

```
HAVEN_PREVIEW_HOUR=9 HAVEN_PREVIEW_WEATHER=rain QT_QPA_PLATFORM=offscreen HAVEN_SCREENSHOT=dashboard.png ./shell/build/haven-shell
```

`HAVEN_PREVIEW_HOUR` is 0–23. `HAVEN_PREVIEW_WEATHER` is `clear`, `cloudy`, `rain`, `snow`, `fog`, or `storm`. The weather card then reads `Preview: Rain` (or whichever look was selected). Leave the weather variable unset and the card stays `Not connected`.

## Screenshot

Write a PNG of the home dashboard and exit:

```
QT_QPA_PLATFORM=offscreen HAVEN_SCREENSHOT=dashboard.png ./shell/build/haven-shell
```
