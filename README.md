# sngrep for Windows (native port)

Native Windows build of [irontec/sngrep](https://github.com/irontec/sngrep) (v1.9.0) as a **single
exe** (`dist/sngrep.exe`, no DLLs or data files). Built with MSYS2 UCRT64, ncurses 6.6 and Npcap.

Prebuilt binary: [Releases](https://github.com/macocheung/sngrep-win/releases) (x64 zip and exe, SHA-256 in the release notes).

## Use

Install [Npcap](https://npcap.com), then run `sngrep.exe` from Windows Terminal / PowerShell:

```
sngrep.exe -I capture.pcap            open a pcap (also .pcap.gz)
sngrep.exe                            live capture on the default-route interface
sngrep.exe -d \Device\NPF_{GUID}      live capture on a specific interface
sngrep.exe -O out.pcap.gz             save the capture (gzip if the name ends in .gz)
sngrep.exe -k server.key              decrypt TLS (OpenSSL)
sngrep.exe -L udp:0.0.0.0:9060        receive HEP/EEP    -H udp:host:9060   send HEP/EEP
```

Interface names: build `tools\listdevs.c` (`gcc listdevs.c -o listdevs.exe -L. -lwpcap`) or use Wireshark.
Live capture may need an elevated shell depending on Npcap's "Restrict to Administrators" setting.

## Build

In an MSYS2 UCRT64 shell: `bash build.sh` (output: `dist/sngrep.exe`).
Packages: gcc cmake ninja ncurses libpcap pcre2 libsystre openssl zlib pkgconf tools (gendef/dlltool).

## What changed vs upstream (`sngrep-windows.patch`)

- `win32_compat.h`: replaces `netinet/*`, `arpa/inet.h`, `strsep`, `strcasestr`; force-included on Windows.
- `win32_terminfo.h`, `win32_shim.c`: embedded terminfo and a static-link shim, for the single exe.
- `main.c`: adds the Npcap directory to the DLL search path, extracts the embedded terminfo.
- `capture.c`: default device via default route (Npcap has no `any`); no SIGUSR1; `-I -` and gzip
  pcap read/write go through temporary files (no `fopencookie` on Windows).
- `capture_eep.c`: HEP/EEP on Winsock (`WSAStartup`, `closesocket`, `WSAPoll`).
- `ui_manager.c`: waits for console input before `wgetch` (ncurses cannot time out on the Windows console).
- `util.c`, `option.c`: no SIGQUIT/SIGHUP; `HOME` falls back to `USERPROFILE`.
- `CMakeLists.txt`: links Npcap's `wpcap.dll` (delay-loaded) and everything else statically.

## Tested

pcap/pcap.gz read and write, stdin, live capture, TUI list, RTP flow detection (`-m`),
HEP over UDP and TCP (send and listen on loopback), invalid TLS key error.

## Not tested / limitations

- TUI key navigation (Enter, F2 save, F3 search, F7 filter), mouse, non-ASCII text.
- TLS decryption with real traffic, live RTP statistics.
- SIGUSR1 dump rotation does not exist on Windows. Upstream tests use `fork()` and do not build here.

## Upstream and license

This is a Windows port of [irontec/sngrep](https://github.com/irontec/sngrep) (based on commit
`8811be1`, "ui: add media inspector dashboard"). All credit for sngrep goes to Irontec S.L. and
its contributors. sngrep is licensed under GPLv3 (see `src/COPYING`, with the OpenSSL exception in
`src/LICENSE.OpenSSL`); the changes here are under the same license.
