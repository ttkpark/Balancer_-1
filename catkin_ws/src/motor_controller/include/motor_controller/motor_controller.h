
// include/motor_controller/motor_controller.h
#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include "motor_controller/mcp4921.h"
#include "motor_controller/i2c_controller.h"
#include "motor_controller/HallSensorEncoder.h"
#include <string>
#include <gpiod.h>

#define WHEEL_RADIUS 0.142
#define WHEEL_DISTANCE 0.375

class MotorController { public:
    // 생성자 및 소멸자
    MotorController(ros::NodeHandle& nh, ros::NodeHandle& pnh);
    ~MotorController();
    
    // 속도 명령 콜백
    void speed0Callback(const std_msgs::Float32::ConstPtr& msg);
    void speed1Callback(const std_msgs::Float32::ConstPtr& msg);
    
    // 타이머 콜백 - 주기적 실행
    void timerCallback(const ros::TimerEvent& event);
    
    // 홀센서 속도 감지 - 주기적 실행
    void checkHallSensor(const ros::TimerEvent& event);
    
    // 버튼 입력 처리
    void checkButtons(); private:
    // 모터 제어 메서드
    void setMotor0Direction(bool forward);
    void setMotor0Speed(float speed_percent);
    void setMotor1Direction(bool forward);
    void setMotor1Speed(float speed_percent);
    void updateActualSpeed();
    
    // GPIO 초기화 및 정리
    bool setOutputLine(int pin,struct gpiod_line** pline);
    bool setInputLine(int pin,struct gpiod_line** pline);
    void setupGPIO();
    void cleanupGPIO();
    
    // ROS 관련 멤버
    ros::NodeHandle& nh_;
    ros::NodeHandle& pnh_;
    ros::Subscriber speed0_sub_;
    ros::Subscriber speed1_sub_;
    ros::Publisher actual_speed0_pub_;
    ros::Publisher actual_speed1_pub_;
    ros::Timer timer0_;
    ros::Timer timer1_;
    
    // 하드웨어 제어 멤버
    MCP4921* dac0_;
    MCP4921* dac1_;
    I2CController i2c;
    BitBangSPI spi;
    HallSensorEncoder encoder;

    // gpiod 관련 멤버
    struct gpiod_chip* chip_;
    struct gpiod_line* reverse0_line_;
    struct gpiod_line* reverse1_line_;
    struct gpiod_line* inc_button_line_;
    struct gpiod_line* dec_button_line_;
    
    // GPIO 핀 번호
    int mosi_pin_;
    int miso_pin_;
    int clk_pin_;
    int ss0_pin_;
    int ss1_pin_;
    int reverse0_pin_;
    int reverse1_pin_;
    //int hall_sensor_pin_;
    int inc_button_pin_;
    int dec_button_pin_;

    int latch_pin_;
    struct gpiod_chip *chip;
    struct gpiod_line* latch_line;
    
    // 상태 및 설정 멤버
    float target_speed_0;
    float actual_speed_0;

    float target_speed_1;
    float actual_speed_1;

    uint8_t hall_state_0;
    uint8_t hall_state_1;

    float max_speed_;
    bool prev_hall_state_;
    int hall_count_;
    ros::Time last_hall_time_;
    
    // 디바운싱을 위한 이전 버튼 상태
    bool prev_inc_button_state_;
    bool prev_dec_button_state_;
    ros::Time last_button_time_;
    
    // 설정 매개변수
    std::string spi_device_;
    int spi_speed_;
    std::string gpio_chip_name_; // 추가: gpiod 칩 이름
};
#endif // MOTOR_CONTROLLER_H
