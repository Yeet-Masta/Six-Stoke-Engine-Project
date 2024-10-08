#include "six-stroke-engine.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <random>
#include <cmath>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

const double PI = 3.14159265358979323846;

// ANSI escape codes for colors and formatting
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

Gearbox::Gearbox() : gear_ratios{ 3.42, 2.14, 1.45, 1.0, 0.83 }, current_gear(1) {}

double Gearbox::get_current_ratio() const {
    return gear_ratios[current_gear - 1];
}

void Gearbox::shift_up() {
    if (current_gear < static_cast<int>(gear_ratios.size())) {
        current_gear++;
    }
}

void Gearbox::shift_down() {
    if (current_gear > 1) {
        current_gear--;
    }
}

int Gearbox::get_current_gear() const {
    return current_gear;
}


void SixStrokeEngine::calculate_displacement() {
    displacement = (PI / 4.0) * pow(bore, 2) * stroke * num_cylinders;
}

void SixStrokeEngine::calculate_rod_stroke_ratio() {
    rod_stroke_ratio = rod_length / stroke;
}

void SixStrokeEngine::calculate_piston_speed() {
    piston_speed = (2 * stroke * rpm) / 60.0;
}

double SixStrokeEngine::calculate_power() const {
    return (mean_effective_pressure * displacement * rpm) / (120 * 1000);
}

double SixStrokeEngine::calculate_torque() const {
    return (power_output * 1000 * 60) / (2 * PI * rpm);
}

double SixStrokeEngine::calculate_thermal_efficiency() const {
    return 1 - 1 / pow(compression_ratio, 1.4 - 1);
}

void SixStrokeEngine::update_performance() {
    calculate_displacement();
    calculate_rod_stroke_ratio();
    calculate_piston_speed();
    power_output = calculate_power();
    torque = calculate_torque();
    thermal_efficiency = calculate_thermal_efficiency();

    for (const auto& [upgrade, is_active] : upgrades) {
        if (is_active && upgrade_effects.contains(upgrade)) {
            upgrade_effects.at(upgrade)();
        }
    }

    // Update fuel consumption based on power output and efficiency
    fuel_consumption = (power_output * 3600) / (43000 * thermal_efficiency); // Assuming gasoline with 43 MJ/kg energy density

    brake_specific_fuel_consumption = (fuel_consumption * 3600) / power_output;
    co2_emissions = brake_specific_fuel_consumption * 3.2; // Approximate CO2 emissions for gasoline

    // Update NOx emissions (simplified model)
    nox_emissions = 0.01 * power_output * (1 + (engine_temperature - 90) / 100);

    if (water_injection_active) {
        thermal_efficiency *= 1.1;
        nox_emissions *= 0.8;
    }

    double temp_difference = std::abs(engine_temperature - optimal_temperature);
    if (temp_difference > 10) {
        thermal_efficiency *= (1 - 0.001 * temp_difference);
    }

    // Ensure volumetric efficiency stays within realistic bounds
    volumetric_efficiency = std::min(std::max(volumetric_efficiency, 0.7), 1.0);
}


void SixStrokeEngine::update_vehicle_speed() {
    double wheel_rpm = rpm / (gearbox.get_current_ratio() * final_drive_ratio);
    vehicle_speed = (wheel_rpm * 2 * PI * wheel_radius) / 60;
}

SixStrokeEngine::SixStrokeEngine() :
    bore(0.086),
    stroke(0.086),
    compression_ratio(11.0),
    num_cylinders(3),
    rpm(1000),
    max_rpm(6000),
    idle_rpm(800),
    rod_length(0.143),
    deck_height(0.2),
    power_output(0),
    torque(0),
    fuel_consumption(0),
    thermal_efficiency(0),
    volumetric_efficiency(0.9),
    mean_effective_pressure(1000000),
    nox_emissions(0.5),
    co2_emissions(0),
    brake_specific_fuel_consumption(0),
    water_injection_active(false),
    water_injection_amount(0.005),
    engine_temperature(90),
    optimal_temperature(90),
    vehicle_speed(0),
    transmission_mode(TransmissionMode::Automatic),
    gear_shift_message_timer(0.0),
    wheel_radius(0.3175),
    final_drive_ratio(3.73),
    vehicle_mass(1500),
    current_fps(0),
    acceleration(0),
    jerk(0)
{
    calculate_displacement();
    calculate_rod_stroke_ratio();
    calculate_piston_speed();

    upgrades["direct_injection"] = false;
    upgrades["turbocharger"] = false;
    upgrades["variable_valve_timing"] = false;
    upgrades["exhaust_gas_recirculation"] = false;
    upgrades["waste_heat_recovery"] = false;
    upgrades["smart_cooling"] = false;
    upgrades["advanced_materials"] = false;
    upgrades["enhanced_ecu"] = false;
    upgrades["cylinder_deactivation"] = false;
    upgrades["variable_compression"] = false;
    upgrades["ceramic_coating"] = false;

    upgrade_effects["direct_injection"] = [this]() {
        fuel_consumption *= 0.9;
        thermal_efficiency *= 1.05;
        };
    upgrade_effects["turbocharger"] = [this]() {
        power_output *= 1.2;
        volumetric_efficiency *= 1.15;
        };
    upgrade_effects["variable_valve_timing"] = [this]() {
        volumetric_efficiency *= 1.1;
        fuel_consumption *= 0.95;
        };
    upgrade_effects["exhaust_gas_recirculation"] = [this]() {
        nox_emissions *= 0.7;
        };
    upgrade_effects["waste_heat_recovery"] = [this]() {
        thermal_efficiency *= 1.05;
        };
    upgrade_effects["smart_cooling"] = [this]() {
        thermal_efficiency *= 1.02;
        };
    upgrade_effects["advanced_materials"] = [this]() {
        power_output *= 1.05;
        };
    upgrade_effects["enhanced_ecu"] = [this]() {
        fuel_consumption *= 0.95;
        power_output *= 1.05;
        };
    upgrade_effects["cylinder_deactivation"] = [this]() {
        fuel_consumption *= 0.92;
        };
    upgrade_effects["variable_compression"] = [this]() {
        thermal_efficiency *= 1.08;
        fuel_consumption *= 0.93;
        };
    upgrade_effects["ceramic_coating"] = [this]() {
        thermal_efficiency *= 1.03;
        engine_temperature -= 5;
        };

    update_performance();
    update_vehicle_speed();
}

void SixStrokeEngine::toggle_transmission_mode() {
    transmission_mode = (transmission_mode == TransmissionMode::Automatic) ?
        TransmissionMode::Manual : TransmissionMode::Automatic;
    std::cout << "Transmission mode switched to "
        << (transmission_mode == TransmissionMode::Automatic ? "Automatic" : "Manual")
        << std::endl;
}

void SixStrokeEngine::update_dynamics(double dt) {
    // Update jerk (rate of change of acceleration)
    jerk += (std::rand() % 201 - 100) * dt; // Random jerk between -100 and 100
    jerk = std::max(-500.0, std::min(500.0, jerk)); // Limit jerk

    // Update acceleration
    acceleration += jerk * dt;
    acceleration = std::max(-50.0, std::min(50.0, acceleration)); // Limit acceleration

    // Update RPM based on acceleration
    rpm += acceleration * dt * 10; // Multiply by 10 to make changes more noticeable
    rpm = std::max(idle_rpm, std::min(max_rpm, rpm));

    // Update engine temperature
    double temp_change = (acceleration > 0 ? 0.5 : -0.2) * dt;
    engine_temperature += temp_change;
    engine_temperature = std::max(85.0, std::min(110.0, engine_temperature));

    // Randomly toggle water injection
    if (std::rand() % 1000 < 5) { // 0.5% chance each frame
        toggle_water_injection(!water_injection_active);
    }

    int previous_gear = gearbox.get_current_gear();

    if (transmission_mode == TransmissionMode::Automatic) {
        if (rpm > 4000 && gearbox.get_current_gear() < 5) {
            gearbox.shift_up();
            rpm -= 1500;
        }
        else if (rpm < 2000 && gearbox.get_current_gear() > 1) {
            gearbox.shift_down();
            rpm += 1500;
        }
    }

    // Ensure RPM stays within bounds
    rpm = std::max(idle_rpm, std::min(max_rpm, rpm));

    update_performance();
    update_vehicle_speed();

    // Update gear shift message and timer
    if (gearbox.get_current_gear() != previous_gear) {
        gear_shift_message = "Shifted to gear " + std::to_string(gearbox.get_current_gear());
        gear_shift_message_timer = 3.0; // Display message for 3 seconds
    }
    else if (gear_shift_message_timer > 0) {
        gear_shift_message_timer -= dt;
        if (gear_shift_message_timer <= 0) {
            gear_shift_message.clear();
        }
    }
}

// Add this new method to calculate FPS
double SixStrokeEngine::calculate_fps() {
    auto current_time = std::chrono::high_resolution_clock::now();
    double frame_time = std::chrono::duration<double, std::milli>(current_time - last_frame_time).count();
    last_frame_time = current_time;

    frame_times.push(frame_time);
    if (frame_times.size() > 60) {
        frame_times.pop();
    }

    double total_time = 0.0;
    std::queue<double> temp_queue = frame_times; // Create a temporary queue
    while (!temp_queue.empty()) {
        total_time += temp_queue.front();
        temp_queue.pop();
    }

    return frame_times.size() / (total_time / 1000.0);
}

void SixStrokeEngine::apply_upgrade(const std::string& upgrade) {
    if (upgrades.count(upgrade) > 0) {
        upgrades[upgrade] = true;
        std::cout << upgrade << " applied\n";
        update_performance();
    }
    else {
        std::cout << "Unknown upgrade: " << upgrade << std::endl;
    }
}

void SixStrokeEngine::toggle_water_injection(bool activate) {
    water_injection_active = activate;
    std::cout << "Water injection " << (water_injection_active ? "activated" : "deactivated") << std::endl;
    update_performance();
}

void SixStrokeEngine::simulate_performance() {
    static bool first_run = true;

    if (first_run) {
        std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left
        std::cout << BOLD << BLUE << "Advanced Six-Stroke Engine Simulation\n" << RESET;
        std::cout << WHITE << std::string(50, '=') << RESET << "\n\n";
        first_run = false;
    }

    auto print_label = [](const std::string& label, int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H" << BOLD << CYAN << std::setw(22) << std::left << label << RESET;
        };

    auto print_value = [](const std::string& value, const std::string& color, int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H" << color << std::setw(15) << std::right << value << RESET;
        };

    if (!gear_shift_message.empty() && gear_shift_message_timer > 0) {
        print_label("Gear Shift:", 17, 2);
        print_value(gear_shift_message, WHITE, 17, 25);
    }
    else {
        // Clear the gear shift message line
        std::cout << "\033[17;2H" << std::string(60, ' ');
    }

    // Engine Performance
    print_label("Displacement:", 3, 2);
    print_value(std::to_string(static_cast<int>(displacement * 1000000)) + " cc", YELLOW, 3, 25);

    print_label("Power Output:", 4, 2);
    print_value(std::to_string(static_cast<int>(power_output)) + " kW", GREEN, 4, 25);

    print_label("Torque:", 5, 2);
    print_value(std::to_string(static_cast<int>(torque)) + " Nm", MAGENTA, 5, 25);

    print_label("Thermal Efficiency:", 6, 2);
    print_value(std::to_string(static_cast<int>(thermal_efficiency * 100)) + "%", BLUE, 6, 25);

    // Engine State
    print_label("RPM:", 8, 2);
    print_value(std::to_string(static_cast<int>(rpm)), RED, 8, 25);

    print_label("Engine Temperature:", 9, 2);
    std::string temp_color = engine_temperature > 100 ? RED : (engine_temperature < 80 ? BLUE : GREEN);
    print_value(std::to_string(static_cast<int>(engine_temperature)) + " °C", temp_color, 9, 25);

    print_label("Water Injection:", 10, 2);
    print_value(water_injection_active ? "Active" : "Inactive", water_injection_active ? GREEN : YELLOW, 10, 25);

    // Vehicle Dynamics
    print_label("Vehicle Speed:", 12, 2);
    print_value(std::to_string(static_cast<int>(vehicle_speed * 3.6)) + " km/h", YELLOW, 12, 25);

    print_label("Transmission Mode:", 15, 2);
    print_value(transmission_mode == TransmissionMode::Automatic ? "Automatic" : "Manual", transmission_mode == TransmissionMode::Automatic ? GREEN : YELLOW, 15, 25);

    print_label("Current Gear:", 13, 2);
    print_value(std::to_string(gearbox.get_current_gear()), MAGENTA, 13, 25);

    print_label("Acceleration:", 14, 2);
    print_value(std::to_string(acceleration).substr(0, 6) + " m/s²", BLUE, 14, 25);

    // Emissions and Efficiency
    print_label("NOx Emissions:", 3, 42);
    print_value(std::to_string(nox_emissions).substr(0, 5) + " g/kWh", RED, 3, 65);

    print_label("CO2 Emissions:", 4, 42);
    print_value(std::to_string(static_cast<int>(co2_emissions)) + " g/km", YELLOW, 4, 65);

    print_label("BSFC:", 5, 42);
    print_value(std::to_string(brake_specific_fuel_consumption).substr(0, 6) + " g/kWh", MAGENTA, 5, 65);

    print_label("Volumetric Efficiency:", 6, 42);
    print_value(std::to_string(static_cast<int>(volumetric_efficiency * 100)) + "%", GREEN, 6, 65);

    // Simulation Stats
    print_label("FPS:", 8, 42);
    print_value(std::to_string(static_cast<int>(current_fps)), CYAN, 8, 65);

    print_label("Jerk:", 9, 42);
    print_value(std::to_string(jerk).substr(0, 6) + " m/s³", BLUE, 9, 65);

    // Controls reminder
    std::cout << "\033[16;2H" << WHITE << BOLD << "Controls: " << RESET
        << "a: Accelerate | d: Decelerate | e: Upshift | q: Downshift | Ctrl+C: Exit";

    std::cout << "\033[18;1H"; // Move cursor to a safe position at the bottom
    std::cout.flush();
}

void SixStrokeEngine::accelerate() {
    rpm += 100;
    if (rpm > max_rpm) {
        rpm = max_rpm;
    }

    // More realistic temperature increase
    double temp_increase = 0.5 * (1 - (engine_temperature - 90) / 100);
    engine_temperature += std::max(0.0, temp_increase);

    if (engine_temperature > 110) {
        engine_temperature = 110; // Cap maximum temperature
    }

    update_performance();
    update_vehicle_speed();

    if (rpm > 4000 && gearbox.get_current_gear() < 5) {
        gearbox.shift_up();
        rpm -= 1500;
    }
}

void SixStrokeEngine::decelerate() {
    rpm -= 100;
    if (rpm < idle_rpm) {
        rpm = idle_rpm;
    }

    // More realistic temperature decrease
    double temp_decrease = 0.2 * ((engine_temperature - 90) / 100);
    engine_temperature -= std::max(0.0, temp_decrease);

    if (engine_temperature < 85) {
        engine_temperature = 85; // Cap minimum temperature
    }

    update_performance();
    update_vehicle_speed();

    if (rpm < 2000 && gearbox.get_current_gear() > 1) {
        gearbox.shift_down();
        rpm += 1500;
    }
}

/*void Gearbox::manual_shift_up() {
    if (current_gear < static_cast<int>(gear_ratios.size())) {
        current_gear++;
        std::cout << "Manually shifted up to gear " << current_gear << std::endl;
    }
    else {
        std::cout << "Already in highest gear" << std::endl;
    }
    }

void Gearbox::manual_shift_down() {
    if (current_gear > 1) {
        current_gear--;
        std::cout << "Manually shifted down to gear " << current_gear << std::endl;
    }
    else {
        std::cout << "Already in lowest gear" << std::endl;
    }
}*/

void SixStrokeEngine::manual_upshift() {
    if (transmission_mode == TransmissionMode::Manual) {
        int previous_gear = gearbox.get_current_gear();
        gearbox.shift_up();
        if (gearbox.get_current_gear() != previous_gear) {
            rpm -= 1500; // Simulate rpm drop after upshift
            rpm = std::max(rpm, idle_rpm);
            gear_shift_message = "Manually shifted up to gear " + std::to_string(gearbox.get_current_gear());
            gear_shift_message_timer = 3.0; // Display message for 3 seconds
        }
        else {
            gear_shift_message = "Already in highest gear";
            gear_shift_message_timer = 3.0; // Display message for 3 seconds
        }
    }
}

void SixStrokeEngine::manual_downshift() {
    if (transmission_mode == TransmissionMode::Manual) {
        int previous_gear = gearbox.get_current_gear();
        gearbox.shift_down();
        if (gearbox.get_current_gear() != previous_gear) {
            rpm += 1500; // Simulate rpm increase after downshift
            rpm = std::min(rpm, max_rpm);
            gear_shift_message = "Manually shifted down to gear " + std::to_string(gearbox.get_current_gear());
            gear_shift_message_timer = 3.0; // Display message for 3 seconds
        }
        else {
            gear_shift_message = "Already in lowest gear";
            gear_shift_message_timer = 3.0; // Display message for 3 seconds
        }
    }
}


void clear_console() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void move_cursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

char get_user_input() {
#ifdef _WIN32
    if (_kbhit()) {
        return _getch();
    }
#else
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int result = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

    if (result > 0) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
            return c;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
#endif
    return 0;
}

void SixStrokeEngine::run_simulation() {
    std::cout << "Running real-time simulation at 60 FPS. Controls:\n";
    std::cout << "a: Increase acceleration | d: Decrease acceleration\n";
    std::cout << "e: Manual upshift | q: Manual downshift\n";
    std::cout << "m: Toggle transmission mode\n";
    std::cout << "Press Ctrl+C to stop.\n";

    const double target_frame_time = 1.0 / 60.0; // 60 FPS
    auto start_time = std::chrono::high_resolution_clock::now();
    last_frame_time = start_time;

    while (true) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        char input = get_user_input();
        switch (input) {
        case 'a':
            acceleration += 10;
            break;
        case 'd':
            acceleration -= 10;
            break;
        case 'e':
            manual_upshift();
            break;
        case 'q':
            manual_downshift();
            break;
        case 'm':
            toggle_transmission_mode();
            break;
        }

        double elapsed = std::chrono::duration<double>(frame_start - start_time).count();
        update_dynamics(elapsed);
        simulate_performance();
        current_fps = calculate_fps();

        auto frame_end = std::chrono::high_resolution_clock::now();
        double frame_duration = std::chrono::duration<double>(frame_end - frame_start).count();

        if (frame_duration < target_frame_time) {
            std::this_thread::sleep_for(std::chrono::duration<double>(target_frame_time - frame_duration));
        }

        start_time = frame_start;
    }
}