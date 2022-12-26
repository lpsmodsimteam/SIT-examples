import os
import socket

import cocotb


@cocotb.test()
def mt19937_test(dut):

    # connect the cocotb simulation to SST through socket
    _sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    _sock.connect(cocotb.plusargs["ipc_proc"])

    _sock.sendall(str(os.getpid()).encode())

    while True:
        signal = str(_sock.recv(15).decode("utf-8"))
        alive = int(signal[0])
        signal = signal[1:]
        if not alive:
            break
        dut.rst.value = int(signal[0:1])
        dut.seed_val.value = int(signal[1:11])
        dut.seed_start.value = int(signal[11:12])
        dut.ready.value = int(signal[12:13])
        dut.clk.value = int(signal[13:14]) % 2
        yield cocotb.triggers.Timer(1, units='ns')

        _outputs = (
            str(dut.r_num.value).encode() 
            + str(dut.valid.value).encode() 
            + str(dut.busy.value).encode()
        )
        _sock.sendall(_outputs)

    _sock.close()
