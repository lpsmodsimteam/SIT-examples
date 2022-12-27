#include <systemc.h>

SC_MODULE(sc_cacc) {

    sc_in<float> dist;
    sc_in<sc_uint<32>> inner;
    sc_in<sc_uint<32>> outer;
    sc_out<sc_uint<32>> new_inner;
    sc_out<sc_uint<32>> new_outer;

    const float radius = 1.0;
    int temp_outer = 0;
    int temp_inner = 0;

    void cond_acc() {
        if (dist.read() < radius) {
            temp_inner = inner.read() + 1;
        } else {
            temp_outer = outer.read() + 1;
        }
        new_inner.write(temp_inner);
        new_outer.write(temp_outer);
    }

    SC_CTOR(sc_cacc)
        : dist("dist"),
          inner("inner"),
          outer("outer"),
          new_inner("new_inner"),
          new_outer("new_outer") {
        SC_METHOD(cond_acc);
        sensitive << dist << inner << outer;
    }
};
