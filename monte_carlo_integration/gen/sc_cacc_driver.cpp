#include <iomanip>
#include <sst/sit/bufwidth.hpp>

#include <sst/sit/sit.hpp>

#include "/home/sabbir/SIT-Examples/monte_carlo_integration/systemc/sc_cacc.hpp"

int sc_main(int, char* argv[]) {

    sc_report_handler::set_actions(SC_ID_NO_SC_START_ACTIVITY_, SC_DO_NOTHING);
    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<float> dist;
    sc_signal<sc_uint<32>> inner;
    sc_signal<sc_uint<32>> outer;
    sc_signal<sc_uint<32>> new_inner;
    sc_signal<sc_uint<32>> new_outer;

    // Connect the DUT
    sc_cacc DUT("sc_cacc");
    DUT.dist(dist);
    DUT.inner(inner);
    DUT.outer(outer);
    DUT.new_inner(new_inner);
    DUT.new_outer(new_outer);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    SITSocketBuffer sit_buf(33, false);
    sit_buf.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    sit_buf.set(std::to_string(getpid()));
    sit_buf.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    std::ostringstream _data_out;
    std::string _data_out_str;

    while (true) {

        // RECEIVING
        sit_buf.recv();
        std::string _data_in = sit_buf.get();

        if (_data_in[0] == '0') {
            break;
        }
        dist = std::stof(_data_in.substr(1, 12));
        inner = std::stol(_data_in.substr(13, 10));
        outer = std::stol(_data_in.substr(23, 10));

        // SENDING
        sc_start();

        _data_out_str = align_buffer_width(std::to_string(new_inner.read()), 10);
        _data_out << _data_out_str;
        _data_out_str = align_buffer_width(std::to_string(new_outer.read()), 10);
        _data_out << _data_out_str;

        sit_buf.set(_data_out.str());
        sit_buf.send();

        _data_out.str(std::string());

    }

    return 0;

}
