#include <iomanip>

#include <sst/sit/sit.hpp>

#include "/home/sabbir/SIT-Examples/monte_carlo_integration/systemc/sc_div.hpp"

int sc_main(int, char* argv[]) {

    sc_report_handler::set_actions(SC_ID_NO_SC_START_ACTIVITY_, SC_DO_NOTHING);
    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_uint<32>> op1;
    sc_signal<sc_uint<32>> op2;
    sc_signal<float> data_out;

    // Connect the DUT
    sc_div DUT("sc_div");
    DUT.op1(op1);
    DUT.op2(op2);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    SITSocketBuffer sit_buf(21, false);
    sit_buf.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    sit_buf.set(std::to_string(getpid()));
    sit_buf.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    std::ostringstream _data_out;
    

    while (true) {

        // RECEIVING
        sit_buf.recv();
        std::string _data_in = sit_buf.get();

        if (_data_in[0] == '0') {
            break;
        }
        op1 = std::stol(_data_in.substr(1, 10));
        op2 = std::stol(_data_in.substr(11, 10));

        // SENDING
        sc_start();

        _data_out << std::fixed << std::setprecision(9) << data_out;

        sit_buf.set(_data_out.str());
        sit_buf.send();

        _data_out.str(std::string());

    }

    return 0;

}
