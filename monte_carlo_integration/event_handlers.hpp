#ifndef EVENT_HANDLERS_HPP
#define EVENT_HANDLERS_HPP

#include "monte_carlo.hpp"

void monte_carlo::x_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        // std::cout << "x_rand32 " << m_cycle << '\n';
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_x_link->send(temp_se, MAX_32);
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        // std::cout << "y_rand32 " << m_cycle << '\n';
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_y_link->send(temp_se, MAX_32);
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        x_val_norm = std::stof(se->getString());
        std::cout << "div_x " << x_val_norm << '\n';
        x_val_rdy = true;
    }

    delete se;
}

void monte_carlo::div_y(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        y_val_norm = std::stof(se->getString());
        std::cout << "div_y " << y_val_norm << '\n';
        y_val_rdy = true;
    }

    delete se;
}

void monte_carlo::sum_sq(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        // std::cout << "sum_sq " << m_cycle << '\n';
        std::string dist_str = se->getString();
        SigWidth::align_signal_width(12, dist_str);
        std::string inner_str = std::to_string(inner);
        SigWidth::align_signal_width(10, inner_str);
        std::string outer_str = std::to_string(outer);
        SigWidth::align_signal_width(10, outer_str);
        cacc_link->send(dist_str, inner_str, outer_str);
    }

    delete se;
}

void monte_carlo::cacc(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && clock_high) {
        // std::cout << "cacc " << m_cycle << '\n';
        std::string temp_se = se->getString();
        inner = std::stol(temp_se.substr(0, 10));
        outer = std::stol(temp_se.substr(10, 10));

        std::string denom = std::to_string(inner + outer);
        SigWidth::align_signal_width(10, denom);
        div_areas_link->send(temp_se.substr(0, 10), denom);
    }

    delete se;
}

void monte_carlo::div_areas(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        // std::cout << "div_areas " << m_cycle << '\n';
        // std::cout << "divided? " << se->getString() << '\n';
        estimate = 4 * std::stof(se->getString());
    }

    delete se;
}

#endif  // EVENT_HANDLERS_HPP
