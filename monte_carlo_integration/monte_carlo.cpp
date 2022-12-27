#include "monte_carlo.hpp"

monte_carlo::monte_carlo(SST::ComponentId_t id, SST::Params& params)
    : SST::Component(id),
      // collect all the parameters from the project driver
      seed(params.find<uint16_t>("SEED", 0)),
      output(params.find<bool>("OUTPUT", true)) {
    x_val_rdy = false;
    y_val_rdy = false;

    x_rand32_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    x_rand32_link->din_link = configureLink("x_rand32_din");
    x_rand32_link->dout_link = configureLink(
        "x_rand32_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::x_rand32));

    y_rand32_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    y_rand32_link->din_link = configureLink("y_rand32_din");
    y_rand32_link->dout_link = configureLink(
        "y_rand32_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::y_rand32));

    div_x_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    div_x_link->din_link = configureLink("div_x_din");
    div_x_link->dout_link = configureLink(
        "div_x_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_x));

    div_y_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    div_y_link->din_link = configureLink("div_y_din");
    div_y_link->dout_link = configureLink(
        "div_y_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_y));

    sum_sq_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    sum_sq_link->din_link = configureLink("sum_sq_din");
    sum_sq_link->dout_link = configureLink(
        "sum_sq_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::sum_sq));

    m_output.init("\033[93mmonte_carlo-" + getName() + "\033[0m -> ", 1, 0,
                  SST::Output::STDOUT);

    m_output.setVerboseLevel(output);

    // Just register a plain clock for this simple example
    registerClock(
        "1Hz", new SST::Clock::Handler<monte_carlo>(this, &monte_carlo::tick));

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void monte_carlo::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

void monte_carlo::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
}

bool monte_carlo::tick(SST::Cycle_t current_cycle) {

    m_keep_send = current_cycle < 40000;
    m_keep_recv = current_cycle < 40000 - 1;
    mt19937_rdy = current_cycle > 39940;

    std::string _seed = "0000000000";
    std::string _seed2 = "0000000101";

    std::string _rst = "0";
    std::string _seed_start = "0";
    std::string _rdy = "0";

    if (current_cycle == 1) {
        _rst = "1";
    }

    if (current_cycle == 3) {
        _seed_start = "1";
    }

    if (current_cycle > 4) {
        _rdy = "1";
    }

    x_rand32_link->send(_rst, _seed, _seed_start, _rdy,
                        std::to_string(current_cycle % 2));

    y_rand32_link->send(_rst, _seed2, _seed_start, _rdy,
                        std::to_string(current_cycle % 2));

    if (x_val_rdy && y_val_rdy) {
        sum_sq_link->send(std::to_string(x_val_norm),
                          std::to_string(y_val_norm));
        x_val_rdy = false;
        y_val_rdy = false;
    }

    if (current_cycle > 39940) {

        m_output.verbose(CALL_INFO, 1, 0, "cycle: %ld x: %f\n", current_cycle,
                         x_val_norm);
        m_output.verbose(CALL_INFO, 1, 0, "y: %f\n", y_val_norm);
    }

    if (current_cycle > 40000) {
        primaryComponentOKToEndSim();
    }

    return current_cycle > 40000;
}

void monte_carlo::x_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_x_link->send(temp_se, "4294967295");
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_y_link->send(temp_se, "4294967295");
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se && se->getString().length()) {
        x_val_norm = std::stof(se->getString());
        x_val_rdy = true;
    }

    delete se;
}

void monte_carlo::div_y(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se && se->getString().length()) {
        y_val_norm = std::stof(se->getString());
        y_val_rdy = true;
    }

    delete se;
}

void monte_carlo::sum_sq(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se && se->getString().length()) {
        std::cout << "BURGGGF " << se->getString() << '\n';
    }

    delete se;
}
