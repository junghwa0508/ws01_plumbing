/*  
  需求：订阅发布方发布的学生消息，并输出到终端
*/

// 1.包含头文件；
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/msg/student.hpp"

using std::placeholders::_1;
using base_interfaces_demo::msg::Student;
// 3.自定义节点类；
class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber(): Node("student_subscriber")
    {
      // 3-1.创建订阅方；
      subscription_ = this->create_subscription<Student>("topic_stu", 10,std::bind(&MinimalSubscriber::topic_callback, this,std::placeholders::_1));
    }
  
  private:
  void topic_callback(const Student &stu)
  {
    // 3-2.回调函数订阅并解析数据；
      RCLCPP_INFO(this->get_logger(),"订阅的消息：name=%s,age=%d,height=%2f,weight=%2f", stu.name.c_str(),stu.age,stu.height,stu.weight);
   
    }
    rclcpp::Subscription<Student>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  // 2.初始化 ROS2 客户端；
  rclcpp::init(argc, argv);
  // 4.调用spin函数，并传入节点对象指针。
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  // 5.释放资源；
  rclcpp::shutdown();
  return 0;
}