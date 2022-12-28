from sit import SITPyRTL, SITSystemC, SITVerilog

if __name__ == "__main__":

    mt19937 = SITVerilog(
        lib="monte_carlo",
        module_dir="verilog",
        module_name="mt19937",
    )

    mt19937.set_ports(
        (
            ("clock", "clk", "1"),
            ("input", "rst", "1"),
            ("input", "seed_val", "32"),
            ("input", "seed_start", "1"),
            ("input", "ready", "1"),
            ("output", "r_num", "32"),
            ("output", "valid", "1"),
            ("output", "busy", "1"),
        )
    )
    mt19937.generate_black_boxes()

    sc_div = SITSystemC(
        lib="monte_carlo",
        module_dir="systemc",
        module_name="sc_div",
    )
    sc_div.set_ports(
        (
            ("input", "op1", "<sc_uint<32>>"),
            ("input", "op2", "<sc_uint<32>>"),
            ("output", "data_out", "<float>", 12),
        )
    )
    sc_div.fixed_width_float_output(9)
    sc_div.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
    sc_div.generate_black_boxes()

    sc_sumsq = SITSystemC(
        lib="monte_carlo",
        module_dir="systemc",
        module_name="sc_sumsq",
    )
    sc_sumsq.set_ports(
        (
            ("input", "op1", "<float>", 8),
            ("input", "op2", "<float>", 8),
            ("output", "data_out", "<float>", 12),
        )
    )
    sc_sumsq.fixed_width_float_output(9)
    sc_sumsq.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
    sc_sumsq.generate_black_boxes()

    sc_cacc = SITSystemC(
        lib="monte_carlo",
        module_dir="systemc",
        module_name="sc_cacc",
    )
    sc_cacc.set_ports(
        (
            ("input", "dist", "<float>", 12),
            ("input", "inner", "<sc_uint<32>>"),
            ("input", "outer", "<sc_uint<32>>"),
            ("output", "new_inner", "<sc_uint<32>>"),
            ("output", "new_outer", "<sc_uint<32>>"),
        )
    )
    sc_cacc.fixed_width_float_output(9)
    sc_cacc.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
    sc_cacc.generate_black_boxes()
