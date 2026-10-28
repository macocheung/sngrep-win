import socket, time
msg = ("INVITE sip:bob@example.com SIP/2.0\r\nVia: SIP/2.0/UDP 10.0.0.1:5060;branch=z9hG4bK1\r\n"
       "From: <sip:alice@example.com>;tag=1\r\nTo: <sip:bob@example.com>\r\nCall-ID: sngrep-win-test-1@host\r\n"
       "CSeq: 1 INVITE\r\nContent-Length: 0\r\n\r\n").encode()
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
time.sleep(2)
for _ in range(3):
    s.sendto(msg, ("8.8.8.8", 5060)); time.sleep(0.5)
