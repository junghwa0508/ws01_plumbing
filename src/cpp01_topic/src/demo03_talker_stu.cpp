/*  
  需求：以某个固定频率发送文本学生信息，包含学生的姓名、年龄、身高等数据。
*/

// 1.包含头文件；
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/msg/student.hpp"

using namespace std::chrono_literals;
using base_interfaces_demo::msg::Student;
// 3.定义节点类；
class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("student_publisher"), age(0)
    {
      // 3-1.创建发布方；
      publisher_ = this->create_publisher<Student>("topic_stu", 10);
      // 3-2.创建定时器；
      timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::timer_callback, this));
    }

  private:
    void timer_callback()
   {
    // 3-3.组织消息并发布
    auto stu = Student();
    stu.name = "spider-man";
    stu.age = age++;  
    stu.height = 1.75;
    stu.weight = 66.4;
    publisher_->publish(stu);
    RCLCPP_INFO(this->get_logger(), "发布的消息: name=%s,age=%d,height=%2f,weight=%2f", stu.name.c_str(),stu.age,stu.height,stu.weight);
   }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<Student>::SharedPtr publisher_;
    int age;
};

int main(int argc, char * argv[])
{
  // 2.初始化 ROS2 客户端；
  rclcpp::init(argc, argv);
  // 4.调用spin函数，并传入节点对象指针。
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  // 5.释放资源；
  rclcpp::shutdown();
  return 0;
}