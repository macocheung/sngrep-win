"""Run a program in a ConPTY, resize it, and print the emulated screen (needs: pip install pyte)."""
import ctypes, sys, threading, time
from ctypes import wintypes as w
import pyte

k32 = ctypes.WinDLL('kernel32', use_last_error=True)
HPCON = ctypes.c_void_p

class COORD(ctypes.Structure):
    _fields_ = [('X', ctypes.c_short), ('Y', ctypes.c_short)]

class SI(ctypes.Structure):
    _fields_ = [('cb', w.DWORD), ('lpReserved', w.LPWSTR), ('lpDesktop', w.LPWSTR), ('lpTitle', w.LPWSTR),
                ('dwX', w.DWORD), ('dwY', w.DWORD), ('dwXSize', w.DWORD), ('dwYSize', w.DWORD),
                ('dwXCountChars', w.DWORD), ('dwYCountChars', w.DWORD), ('dwFillAttribute', w.DWORD),
                ('dwFlags', w.DWORD), ('wShowWindow', w.WORD), ('cbReserved2', w.WORD),
                ('lpReserved2', ctypes.c_void_p), ('hStdInput', w.HANDLE), ('hStdOutput', w.HANDLE),
                ('hStdError', w.HANDLE)]

class SIEX(ctypes.Structure):
    _fields_ = [('StartupInfo', SI), ('lpAttributeList', ctypes.c_void_p)]

class PI(ctypes.Structure):
    _fields_ = [('hProcess', w.HANDLE), ('hThread', w.HANDLE), ('dwProcessId', w.DWORD), ('dwThreadId', w.DWORD)]

def run(cmdline, cols, rows, script):
    in_r, in_w = w.HANDLE(), w.HANDLE()
    out_r, out_w = w.HANDLE(), w.HANDLE()
    k32.CreatePipe(ctypes.byref(in_r), ctypes.byref(in_w), None, 0)
    k32.CreatePipe(ctypes.byref(out_r), ctypes.byref(out_w), None, 0)
    hpc = HPCON()
    hr = k32.CreatePseudoConsole(COORD(cols, rows), in_r, out_w, 0, ctypes.byref(hpc))
    assert hr == 0, hr
    size = ctypes.c_size_t()
    k32.InitializeProcThreadAttributeList(None, 1, 0, ctypes.byref(size))
    buf = ctypes.create_string_buffer(size.value)
    k32.InitializeProcThreadAttributeList(buf, 1, 0, ctypes.byref(size))
    k32.UpdateProcThreadAttribute(buf, 0, 0x00020016, hpc, ctypes.sizeof(HPCON), None, None)
    si = SIEX(); si.StartupInfo.cb = ctypes.sizeof(SIEX); si.lpAttributeList = ctypes.addressof(buf); si.StartupInfo.dwFlags = 0x100
    pi = PI()
    ok = k32.CreateProcessW(None, cmdline, None, None, False, 0x00080000, None, None, ctypes.byref(si), ctypes.byref(pi))
    assert ok, ctypes.get_last_error()

    screen = pyte.Screen(cols, rows)
    stream = pyte.ByteStream(screen)
    stop = False
    def reader():
        data = ctypes.create_string_buffer(65536); n = w.DWORD()
        while not stop:
            if not k32.ReadFile(out_r, data, 65536, ctypes.byref(n), None) or n.value == 0:
                break
            stream.feed(data.raw[:n.value])
    threading.Thread(target=reader, daemon=True).start()

    def send(text):
        n = w.DWORD(); b = text.encode()
        k32.WriteFile(in_w, b, len(b), ctypes.byref(n), None)

    def dump(title):
        print('=' * 20, title, '(%dx%d)' % (screen.columns, screen.lines))
        for line in screen.display:
            print(line.rstrip())

    def resize(c, r):
        print("resize hr =", k32.ResizePseudoConsole(hpc, COORD(c, r)))
        screen.resize(r, c)

    script(send, dump, resize, screen)
    stop = True
    k32.TerminateProcess(pi.hProcess, 0)
    k32.ClosePseudoConsole(hpc)

if __name__ == '__main__':
    exe, pcap = sys.argv[1], sys.argv[2]
    def script(send, dump, resize, screen):
        time.sleep(2.5); dump('call list')
        send('\r'); time.sleep(1.5); dump('call flow 100x30')
        resize(140, 40); time.sleep(2); dump('after resize to 140x40')
        resize(80, 24); time.sleep(2); dump('after resize to 80x24')
    run('"%s" -I "%s"' % (exe, pcap), 100, 30, script)
