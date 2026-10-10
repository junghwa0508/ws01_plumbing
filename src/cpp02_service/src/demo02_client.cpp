#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/addints.hpp"

using base_interfaces_demo::srv::Addints;
using namespace std::chrono_literals;
class AddintsClient : public rclcpp::Node{
public:
  AddintsClient() :Node("addints_client") 
  { //测试；
    RCLCPP_INFO(this->get_logger(),"客户端节点创建成功！");
 
    // 创建客户端；
    /* 
      服务接口类型  服务名称；
      返回值类型：rclcpp::Client<服务接口类型>::SharedPtr  ；
    */
    client_ = this->create_client<Addints>("addints");
  }
  //连接服务器（客户端未连接到服务器时，不能发送请求）；
  bool connect_server()
  { 
    //指定时间内连接服务器，连接成功返回true，否则返回false；
    while(!client_->wait_for_service(1s)) //循环以1s为超时时间连接服务器，直至连接成功，然后退出循环；
    {
      /*
        对ctrl+c 作出特殊处理
        1.如何判断按下ctrl+c？
        2.如何处理？ 
      */
      if (!rclcpp::ok())
      {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"程序被用户中断!");
        return false;
      }
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务器连接中……");
    }
    return true;
  }
  //编写发送请求函数；参数是两个整型数据，返回值是提交请求后服务端的返回结果。
   rclcpp::Client<Addints>::FutureAndRequestId  send_request(int num1,int num2){
    //组织请求数据；

    //发送；
    auto request =std::make_shared<Addints::Request>();
    request->num1 = num1; request->num2 = num2; 
    return client_->async_send_request(request);
  }
private:
  rclcpp::Client<Addints>::SharedPtr  client_;
};
int main(int argc, char * argv[])
{   if(argc !=3)
  {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请输入两个整型数据！");
    return 1;
  }
  rclcpp::init(argc, argv);
  //创建对象指针并调用连接服务器功能（客户端不需要挂起，无spin）；
  auto client = std::make_shared<AddintsClient>();
  bool flag = client->connect_server();
  //根据连接结果做出响应；
  if (!flag)
  {  
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务器连接失败，程序退出！");
    return 0;
  }
  //调用请求提交函数，接受并相应处理结果；
  auto future =client->send_request(atoi(argv[1]),atoi(argv[2]));
  //处理响应；
  if(rclcpp::spin_until_future_complete(client,future) == rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(client->get_logger(),"响应成功！sum=%d",future.get()->sum);
  }
  else
  {
    RCLCPP_INFO(client->get_logger(),"响应失败！");
  }
  rclcpp::shutdown();
  return 0;
}
//RCLCPP_INFO(rclcpp::get_logger("name")创建logger对象不依赖于context；