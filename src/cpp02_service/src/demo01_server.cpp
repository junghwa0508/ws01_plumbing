#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/addints.hpp"
using base_interfaces_demo::srv::Addints;
using  std::placeholders::_1;
using  std::placeholders::_2;

class AddintsServer : public rclcpp::Node{
public:
  AddintsServer() : Node("addints_server") {
    // 测试；
    RCLCPP_INFO(this->get_logger(),"服务端节点创建成功！");

    // 创建服务端；
    /* 服务接口类型  服务名  回调函数  绑定成员函数需传入this指针  _1请求  _2响应  ；
       返回值类型：rclcpp::Service<服务接口类型>::SharedPtr  ；
    */
    server_ = this->create_service<Addints>("addints",std::bind(&AddintsServer::add,this,_1,_2));
}
    
private :
    // 回调函数解析请求并发送响应；
void add(const Addints::Request::SharedPtr req, const Addints::Response::SharedPtr res)
    { res->sum = req->num1+req->num2;
      RCLCPP_INFO(this->get_logger(),"%d+%d=%d",req->num1,req->num2,res->sum);
    }
   rclcpp::Service<Addints>::SharedPtr server_;
};
int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AddintsServer>());
  rclcpp::shutdown();
  return 0;
}

    //可执行文件没生成，排查 CMakeLists.txt 配置错误;
    //ros2 service call <服务名> <服务类型> <请求参数>