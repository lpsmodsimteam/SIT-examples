#include <systemc.h>

SC_MODULE(sc_cacc) {
    sc_in<float> dist;
    sc_in<sc_uint<32>> inner;
    sc_in<sc_uint<32>> outer;
    sc_out<sc_uint<32>> new_inner;
    sc_out<sc_uint<32>> new_outer;

    const float radius = 1.0;
    unsigned int temp_inner = inner.read();
    unsigned int temp_outer = outer.read();

    void cond_acc() {
        if (dist < radius) {
            temp_inner += 1;
        } else {
            temp_outer += 1;
        }
        new_inner.write(temp_inner);
        new_outer.write(temp_outer);
    }

    SC_CTOR(sc_cacc) {
        SC_METHOD(cond_acc);
        sensitive << dist << inner << outer;
    }
};
