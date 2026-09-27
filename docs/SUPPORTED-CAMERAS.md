# Supported cameras

The program does **not** identify a camera by USB ID. You pick a model from the camera list; the program then looks for a
DirectShow device whose *friendly name* exactly equals one of the names listed for that model, and applies that model's
protocol ("pool"). Many cameras register with Windows as the generic name **"USB Camera"**, so choosing the right list
entry matters — a wrong choice simply reports that no device was found.

Device names come from `src/RMH_SupportedIRCameras_Resources.h`.

| List entry | Windows device name(s) matched | Protocol pool |
|---|---|---|
| InfiRay T2L / T2L V2 | `T2L-A4L`, `T2L-A6L`, `T2L-A8L`, `T2L`, … | 1 / 3 |
| InfiRay T2-Search / V2 | `T2-Search`, `T2`, `T2S`, … | 1 / 3 |
| InfiRay T2S+ / V2 | `T2S+`, `T2Sp`, `T2SPro`, … | 1 / 3 |
| InfiRay T2Pro and T2SPro / V2 | `T2Pro`, `T2+`, `T2p`, `T2P`, `T2SPro`, … | 1 / 3 |
| InfiRay T3-Search | `T3-Search`, `T3` | 1 |
| InfiRay T3S | `Xtherm-T3S`, `T3S-A68`, `T3S-A13`, `T3S`, … | 1 |
| InfiRay T3Pro | `T3Pro-A13`, `T3Pro-A68`, `T3Pro`, … | 1 |
| InfiRay P2 | `USB Camera`, `Camera` | 2 |
| **InfiRay Or Thermal Master P2Pro** | `USB Camera`, `Camera` | 2 |
| InfiRay DV-DL13 | `VirtualBox Webcam - DV-DL13`, `DV-DL13` | 1 |
| InfiRay S0 Series | `S0-90W`, `S0-40`, `S0-68`, `S0-90` | 1 |
| InfiRay Tiny1-C | `USB Camera`, `Tiny1C` | 2 |
| Thermal Master P2 | `Camera` | 4 |
| HTI HT-301 | `T3`, `T3-317-13`, `T3-317-68`, `HT-301` | 1 |
| UNI-T UTi260M | `USB Camera` | 2 |
| TOPDON TC001 / TS001 | `USB Camera`, `TC001` | 2 |
| TOPDON TC002 / TC003 | `USB Camera`, `TC002`, `TC003` | 2 |
| Victor 328B | `USB Camera` | 2 |
| LODESTAR L2 | `USB Camera` | 2 |
| Snapshot Analysis / Recording Analysis | (no camera — works on saved data) | — |

## Verification status

| Camera | Status |
|---|---|
| Thermal Master P2 Pro (USB `0BDA:5830`, shows as "USB Camera") | **works** — select *InfiRay Or Thermal Master P2Pro* |
| all others | untested in this repository |

## If your camera is not found

1. Check the exact list entry (table above). The Thermal Master **P2** entry is a different model from the **P2 Pro**.
2. Confirm Windows sees it: Device Manager → *Cameras*. Note the name shown.
3. Close other programs that may hold the camera; check *Settings → Privacy → Camera → desktop apps* is on.
4. If the Windows name is not in the table, the model needs a device-name entry — open an issue with the name from step 2.
