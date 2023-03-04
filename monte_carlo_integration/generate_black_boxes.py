from sit import SSTInteroperability

if __name__ == "__main__":

    mt19937_config = {
        "hdl": "verilog",
        "config": {
            "module_name": "mt19937",
            "lib": "monte_carlo",
            "module_dir": "verilog",
        },
        "ports": {
            "clock": [
                {"name": "clk", "type": "int", "len": 1},
            ],
            "input": [
                {"name": "rst", "type": "int", "len": 1},
                {"name": "seed_val", "type": "int", "len": 32},
                {"name": "seed_start", "type": "int", "len": 1},
                {"name": "ready", "type": "int", "len": 1},
            ],
            "output": [
                {"name": "r_num", "type": "int", "len": 32},
                {"name": "valid", "type": "int", "len": 1},
                {"name": "busy", "type": "int", "len": 1},
            ],
        },
    }
    mt19937 = SSTInteroperability(mt19937_config)
    mt19937.ingest()
    mt19937.generate_black_boxes()

    sc_div_config = {
        "hdl": "systemc",
        "config": {
            "lib": "monte_carlo",
            "module_dir": "systemc",
            "module_name": "sc_div",
        },
        "extra": {
            "fixed_width_float_output": 9,
            "disable_runtime_warnings": ["SC_ID_NO_SC_START_ACTIVITY_"],
        },
        "ports": {
            "input": [
                {"name": "op1", "type": "sc_uint<32>", "len": 32},
                {"name": "op2", "type": "sc_uint<32>", "len": 32},
            ],
            "output": [{"name": "data_out", "type": "float", "len": 12}],
        },
    }
    sc_div = SSTInteroperability(sc_div_config)
    sc_div.ingest()
    sc_div.generate_black_boxes()

    sc_sumsq_config = {
        "hdl": "systemc",
        "config": {
            "lib": "monte_carlo",
            "module_dir": "systemc",
            "module_name": "sc_sumsq",
        },
        "extra": {
            "fixed_width_float_output": 9,
            "disable_runtime_warnings": ["SC_ID_NO_SC_START_ACTIVITY_"],
        },
        "ports": {
            "input": [
                {"name": "op1", "type": "float", "len": 8},
                {"name": "op2", "type": "float", "len": 8},
            ],
            "output": [{"name": "data_out", "type": "float", "len": 12}],
        },
    }
    sc_sumsq = SSTInteroperability(sc_sumsq_config)
    sc_sumsq.ingest()
    sc_sumsq.generate_black_boxes()

    sc_cacc_config = {
        "hdl": "systemc",
        "config": {
            "lib": "monte_carlo",
            "module_dir": "systemc",
            "module_name": "sc_cacc",
        },
        "extra": {
            "fixed_width_float_output": 9,
            "disable_runtime_warnings": ["SC_ID_NO_SC_START_ACTIVITY_"],
        },
        "ports": {
            "input": [
                {"name": "dist", "type": "float", "len": 12},
                {"name": "inner", "type": "sc_uint<32>", "len": 32},
                {"name": "outer", "type": "sc_uint<32>", "len": 32},
            ],
            "output": [
                {"name": "new_inner", "type": "sc_uint<32>", "len": 32},
                {"name": "new_outer", "type": "sc_uint<32>", "len": 32},
            ],
        },
    }
    sc_cacc = SSTInteroperability(sc_cacc_config)
    sc_cacc.ingest()
    sc_cacc.generate_black_boxes()
