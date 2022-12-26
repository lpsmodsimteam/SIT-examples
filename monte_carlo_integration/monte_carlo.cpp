#include "monte_carlo.hpp"

monte_carlo::monte_carlo(SST::ComponentId_t id, SST::Params& params)
    : SST::Component(id),

      // collect all the parameters from the project driver
      seed(params.find<uint16_t>("SEED", 0)),
      output(params.find<bool>("OUTPUT", true)),

      x_rand32_din_link(configureLink("x_rand32_din")),
      x_rand32_dout_link(configureLink(
          "x_rand32_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::x_rand32))),

      y_rand32_din_link(configureLink("y_rand32_din")),
      y_rand32_dout_link(configureLink(
          "y_rand32_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::y_rand32))),

      div_x_din_link(configureLink("div_x_din")),
      div_x_dout_link(configureLink(
          "div_x_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_x))),

      div_y_din_link(configureLink("div_y_din")),
      div_y_dout_link(configureLink(
          "div_y_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_y))),

      sum_sq_din_link(configureLink("sum_sq_din")),
      sum_sq_dout_link(configureLink(
          "sum_sq_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::sum_sq))) {

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

    m_keep_send = current_cycle < 39950;
    m_keep_recv = current_cycle < 39950 - 1;

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

    x_rand32_din_link->send(new SST::Interfaces::StringEvent(
        std::to_string(m_keep_send) + std::to_string(m_keep_recv) + _rst +
        _seed + _seed_start + _rdy + std::to_string(current_cycle % 2)));
    y_rand32_din_link->send(new SST::Interfaces::StringEvent(
        std::to_string(m_keep_send) + std::to_string(m_keep_recv) + _rst +
        _seed2 + _seed_start + _rdy + std::to_string(current_cycle % 2)));

    if (current_cycle > 39940) {

        m_output.verbose(CALL_INFO, 1, 0, "cycle: %d x: %f\n", current_cycle,
                         x);
        m_output.verbose(CALL_INFO, 1, 0, "y: %f\n", y);
    }

    return current_cycle > 40000;
}

void monte_carlo::x_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se) {
        std::string temp = se->getString();
        // x = std::stof(temp.substr(0, temp.length() - 2));
        div_x_din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(m_keep_send) + std::to_string(m_keep_recv) +
            temp.substr(0, temp.length() - 2)));
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se) {
        std::string temp = se->getString();
        // y = std::stof(temp.substr(0, temp.length() - 2));
        div_y_din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(m_keep_send) + std::to_string(m_keep_recv) +
            temp.substr(0, temp.length() - 2)));
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se) {
        x = std::stof(se->getString()) / 4294967295;
    }

    delete se;
}

void monte_carlo::div_y(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se) {
        y = std::stof(se->getString()) / 4294967295;
    }

    delete se;
}

void monte_carlo::sum_sq(SST::Event* ev) {}
