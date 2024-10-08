#ifndef SIX_STROKE_ENGINE_H
#define SIX_STROKE_ENGINE_H

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <queue>
#include <numeric>
#include <chrono>

char get_user_input();

class Gearbox {
private:
    std::vector<double> gear_ratios;
    int current_gear;

public:
    Gearbox();
    double get_current_ratio() const;
    void shift_up();
    void shift_down();
    int get_current_gear() const;
    //void manual_shift_up();
    //void manual_shift_down();
};

class SixStrokeEngine {
private:
    // Engine specifications
    double bore;
    double stroke;
    double compression_ratio;
    int num_cylinders;
    double rpm;
    double max_rpm;
    double idle_rpm;
    double rod_length;
    double deck_height;
    std::string gear_shift_message;
    double gear_shift_message_timer;

    enum class TransmissionMode {
        Automatic,
        Manual
    };
    TransmissionMode transmission_mode;

    // New members for FPS calculation and dynamic simulation
    std::queue<double> frame_times;
    std::chrono::high_resolution_clock::time_point last_frame_time;
    double current_fps;

    // Dynamic simulation variables
    double acceleration;
    double jerk;

    // Performance metrics
    double displacement;
    double power_output;
    double torque;
    double fuel_consumption;
    double thermal_efficiency;
    double volumetric_efficiency;
    double mean_effective_pressure;
    double nox_emissions;
    double co2_emissions;
    double brake_specific_fuel_consumption;
    double rod_stroke_ratio;
    double piston_speed;

    // Upgrade flags and effects
    std::map<std::string, bool> upgrades;
    std::map<std::string, std::function<void()>> upgrade_effects;

    // Six-stroke cycle specific
    bool water_injection_active;
    double water_injection_amount;

    // Thermal management
    double engine_temperature;
    double optimal_temperature;

    // Gearbox and vehicle dynamics
    Gearbox gearbox;
    double vehicle_speed;
    double wheel_radius;
    double final_drive_ratio;
    double vehicle_mass;

    // Helper functions
    void calculate_displacement();
    void calculate_rod_stroke_ratio();
    void calculate_piston_speed();
    double calculate_power() const;
    double calculate_torque() const;
    double calculate_thermal_efficiency() const;
    void update_performance();
    void update_vehicle_speed();

public:
    SixStrokeEngine();
    void apply_upgrade(const std::string& upgrade);
    void toggle_water_injection(bool active);
    void simulate_performance();
    //void simulate_performance() const;
    void accelerate();
    void decelerate();
    void toggle_transmission_mode();
    void manual_upshift();
    void manual_downshift();
    void run_simulation();
    // New methods for dynamic simulation
    void update_dynamics(double dt);
    double calculate_fps();
};

#endif // SIX_STROKE_ENGINE_H