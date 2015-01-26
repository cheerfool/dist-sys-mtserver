import socket
import sys
import struct
import time

str1="uptime"
str2="loadl"
str3="oa"
str4="3353 uptime"
str5="exit"

def new_sock():
  global port, addr
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  sock.settimeout(None)
  sock.connect((addr, port))
  return sock

addr = sys.argv[1]
port = int(sys.argv[2])

sock = new_sock()
sock.sendall(str1)
try:
  r = sock.recv(4)
  v = struct.unpack("!i", r)[0]
except socket.timeout:
  print "Received timeout"

print str1+" returned " + str(v)

sock.sendall(str2)
try:
  r = sock.recv(4)
  v = struct.unpack("!i", r)[0]
except socket.timeout:
  print "Received timeout"

print str2+" returned " + str(v)

sock.sendall(str3)
try:
  r = sock.recv(4)
  v = struct.unpack("!i", r)[0]
except socket.timeout:
  print "Received timeout"

print str3+" returned " + str(v)

sock.sendall(str4)
try:
  r = sock.recv(4)
  v = struct.unpack("!i", r)[0]
except socket.timeout:
  print "Received timeout"

print str4+" returned " + str(v)

sock.sendall(str5)
try:
  r = sock.recv(4)
  v = struct.unpack("!i", r)[0]
except socket.timeout:
  print "Received timeout"

print str5 + " returned " + str(v)
