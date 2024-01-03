#!/usr/bin/env python3

#################################################
# Agent of Loongson Control Panel
# Author: Wells Wang
# Version: 0.1beta
# Released under MIT License.
#################################################


import serial, threading, psutil, os, datetime, time;
from settings import *

ser = None
hostname = 'hostname'
kernal = ''
ip = ''
transfering = False


def init():
    global ser
    try:
        ser = serial.Serial(DEVICEPORT, BAUDRATE, timeout = TIMEOUT)
    except:
        print("Init serial port {} faild!".format(DEVICEPORT))

def startTransfer():
    global transfering
    while transfering:
        time.sleep(0.01)
    transfering = True

def endTransfer():
    global transfering
    transfering = False
  

def updateData():
    global ser, hostname, kernal, ip
    threading.Timer(5.0, updateData).start()

    startTransfer()

    if (os.uname().nodename != hostname):
        hostname = os.uname().nodename
        ser.write('!HN!{}\n'.format(hostname).encode('utf8'))
    if (os.uname().release != kernal):
        kernal = os.uname().release
        ser.write('!KR!{}\n'.format(kernal[:kernal.index('-')][:6]).encode('utf8'))

    try:
        cip = os.popen("ip a show dev {}".format(NETDEV)).read()
        cip = cip[cip.index('inet ') + 5 : cip.index('inet ') + 20]
        cip = cip[:cip.index('/')]
        if (ip != cip):
            ip = cip
            ser.write('!IP!{}\n'.format(ip).encode('utf8'))
    except:
        print ("[ERROR]: Network Device {} IP address get faild!".format(NETDEV))
    cpu_percent = psutil.cpu_percent()
    mem_percent = psutil.virtual_memory().percent
    ser.write('!CG!{}\n'.format(chr(int(cpu_percent/10))).encode('utf8'))
    ser.write('!CU!{}\n'.format(chr(int(round(cpu_percent)))).encode('utf8'))
    ser.write('!MG!{}\n'.format(chr(int(mem_percent/10))).encode('utf8'))
    ser.write('!MU!{}\n'.format(chr(int(round(mem_percent)))).encode('utf8'))

    ser.write('!TI!{}\n'.format(datetime.datetime.now().strftime("%H:%M")).encode('utf8'))

    uptime = (time.time() - psutil.boot_time()) /3600
    ser.write('!UT!{}d{}h\n'.format(int(uptime / 24), int(uptime % 24)).encode('utf8'))

    endTransfer()
    
    print("[{}] Data updated.".format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))


def sendLog(page = 0):
    global ser
    startTransfer()
    try:
        size = os.path.getsize(LOGFILE)
        if page < 0: page = 0
        if (TPP * page > size): page = int(size / TPP)
        elif (TPP * page  == size): page = page - 1
        
        f = open(LOGFILE, "r")
        f.seek(TPP * page)
        msg = f.read(TPP).replace("\n", " ")
        ser.write('!LP!{}\n'.format(chr(page)).encode('utf8'))
        ser.write('!LG!{}\n'.format(msg).encode('utf8'))
        print("Message sent: {}\n".format(msg))
    except:
        print("Log file operation failed!")
    endTransfer()

def serialInputTrigger():
    threading.Timer(0.5, serialInputTrigger).start()
    while ser.in_waiting:
        response = ser.readline()
        print("[RESPONSE]: {}\n".format(response))
        if b'#GL#' in response:
            sendLog(response[response.index(b'#GL#')+4])


if __name__=='__main__':
    print("Start to initialize serial port '{}' ...".format(DEVICEPORT))
    init()
    if ser:
        print('Successfull!')
        updateData()
        serialInputTrigger()
        #while 1:
        #    if ser.in_waiting:
        #        print("[RESPONSE]: {}\n".format(ser.readline()))

    


