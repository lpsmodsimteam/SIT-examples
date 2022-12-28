import json
from os import environ
from pathlib import Path
import tempfile

import sst

BASE_PATH = Path.cwd()
CLOCK = "1Hz"
LINK_DELAY = "1ps"

monte_carlo_main = sst.Component("main", "monte_carlo.monte_carlo")
monte_carlo_main.addParams(
    {"CLOCK": CLOCK, "SEED1": "0000000000", "SEED2": "0000000000", "ITER": 1000}
)


def connect_comps(comp, comp_name, main_comp_name):
    sst.Link(main_comp_name + "_din").connect(
        (comp, comp_name + "_din", LINK_DELAY),
        (monte_carlo_main, main_comp_name + "_din", LINK_DELAY),
    )
    sst.Link(main_comp_name + "_dout").connect(
        (comp, comp_name + "_dout", LINK_DELAY),
        (monte_carlo_main, main_comp_name + "_dout", LINK_DELAY),
    )


# SystemC components
###############################################################################
with tempfile.NamedTemporaryFile() as tfp:

    div_x_comp = sst.Component("systemc.div_x", "monte_carlo.sc_div")
    div_x_comp.addParams(
        {
            "clock": CLOCK,
            "proc": BASE_PATH / "sc_div.o",
            "ipc_port": tfp.name,
        }
    )
    connect_comps(div_x_comp, "sc_div", "div_x")

with tempfile.NamedTemporaryFile() as tfp:

    div_y_comp = sst.Component("systemc.div_y", "monte_carlo.sc_div")
    div_y_comp.addParams(
        {
            "clock": CLOCK,
            "proc": BASE_PATH / "sc_div.o",
            "ipc_port": tfp.name,
        }
    )
    connect_comps(div_y_comp, "sc_div", "div_y")

with tempfile.NamedTemporaryFile() as tfp:

    div_areas_comp = sst.Component("systemc.div_areas", "monte_carlo.sc_div")
    div_areas_comp.addParams(
        {
            "clock": CLOCK,
            "proc": BASE_PATH / "sc_div.o",
            "ipc_port": tfp.name,
        }
    )
    connect_comps(div_areas_comp, "sc_div", "div_areas")

with tempfile.NamedTemporaryFile() as tfp:

    sum_sq = sst.Component("systemc.sumsq", "monte_carlo.sc_sumsq")
    sum_sq.addParams(
        {
            "clock": CLOCK,
            "proc": BASE_PATH / "sc_sumsq.o",
            "ipc_port": tfp.name,
        }
    )
    connect_comps(sum_sq, "sc_sumsq", "sum_sq")

with tempfile.NamedTemporaryFile() as tfp:

    cacc = sst.Component("systemc.cacc", "monte_carlo.sc_cacc")
    cacc.addParams(
        {
            "clock": CLOCK,
            "proc": BASE_PATH / "sc_cacc.o",
            "ipc_port": tfp.name,
        }
    )
    connect_comps(cacc, "sc_cacc", "cacc")

###############################################################################
############################### VERILOG ####################################
###############################################################################
with tempfile.NamedTemporaryFile() as tfp:

    proc = ""
    with open("config-cmd.json") as cmd_file:
        data = json.load(cmd_file)
    for k, v in data.items():
        if k != "FILE":
            environ[k] = v
        else:
            proc = v

    x_rand32 = sst.Component("verilog.rand32_x", "monte_carlo.mt19937")
    x_rand32.addParams(
        {
            "clock": CLOCK,
            "proc": proc,
            "ipc_port": tfp.name,
        }
    )
    connect_comps(x_rand32, "mt19937", "x_rand32")

with tempfile.NamedTemporaryFile() as tfp:

    proc = ""
    with open("config-cmd.json") as cmd_file:
        data = json.load(cmd_file)
    for k, v in data.items():
        if k != "FILE":
            environ[k] = v
        else:
            proc = v

    y_rand32 = sst.Component("verilog.rand32_y", "monte_carlo.mt19937")
    y_rand32.addParams(
        {
            "clock": CLOCK,
            "proc": proc,
            "ipc_port": tfp.name,
        }
    )
    connect_comps(y_rand32, "mt19937", "y_rand32")
