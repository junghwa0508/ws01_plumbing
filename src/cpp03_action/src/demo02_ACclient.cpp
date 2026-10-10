//需求：编写动作客户端，可以发送一个整型数据到服务端，并处理服务端的连续反馈和最终结果。可以解析终端下动态传入的参数（前提）。
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/progress.hpp"

using namespace std::chrono_literals;
using base_interfaces_demo::action::Progress;
using std::placeholders::_1;
using std::placeholders::_2;
class ProgressActionClient : public rclcpp::Node{
public:
  ProgressActionClient() :Node("progress_action_client") {
    RCLCPP_INFO(this->get_logger(),"action客户端创建！");
  /*
  rclcpp_action::Client<ActionT>::SharedPtr
  create_client<ActionT, NodeT>(NodeT node,
  const std::string & name,
  rclcpp::CallbackGroup::SharedPtr group = nullptr,
  const rcl_action_client_options_t & options = rcl_action_client_get_default_options())
  */
    Acclient_ = rclcpp_action::create_client<Progress>(this,"get_sum");
}
//发送请求：1.连接服务器。2.发送目标值。
void goalsender(int num)
{
if (!Acclient_->wait_for_action_server(10s))
{
 RCLCPP_ERROR(this->get_logger(),"服务器连接失败！");
  return;
}
/*
std::shared_future<rclcpp_action::ClientGoalHandle<base_interfaces_demo::action::Progress>::SharedPtr> 
async_send_goal(
const base_interfaces_demo::action::Progress::Goal &goal, 
const rclcpp_action::Client<base_interfaces_demo::action::Progress>::SendGoalOptions &options)
*/
auto goal = Progress::Goal();
goal.num = num;
rclcpp_action::Client<Progress>::SendGoalOptions option;
option.goal_response_callback = std::bind(&ProgressActionClient::goal_response_callback,this,_1);
option.feedback_callback = std::bind(&ProgressActionClient::feedback_callback,this,_1,_2);
option.result_callback = std::bind(&ProgressActionClient::result_callback,this,_1);
auto future =  Acclient_->async_send_goal(goal, option);
}
/*
using GoalHandle = ClientGoalHandle<ActionT>;
using GoalResponseCallback = std::function<void (typename GoalHandle::SharedPtr)>;
*/
void goal_response_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goalhandler){
if (!goalhandler)
{RCLCPP_INFO(this->get_logger(),"目标请求被服务端拒绝！");}
else
{RCLCPP_INFO(this->get_logger(),"目标处理中...");}
}
/*
  using FeedbackCallback =
  std::function<void (
  typename ClientGoalHandle<ActionT>::SharedPtr,
  const std::shared_ptr<const Feedback>)>;
*/
void feedback_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goalhandler,const std::shared_ptr<const Progress::Feedback> feedback){
 (void) goalhandler;
 double progress = feedback->progress;
 int pro = (int) (progress*100);
 RCLCPP_INFO(this->get_logger(),"当前进度：%d%%",pro);      
 }
//using ResultCallback = std::function<void (const WrappedResult & result)>;
void result_callback(const rclcpp_action::ClientGoalHandle<Progress>::WrappedResult &result){
//通过状态码（result.code）判断最终结果状态；
if (result.code == rclcpp_action::ResultCode::SUCCEEDED)
{
RCLCPP_INFO(this->get_logger(),"最终结果：%d",result.result->sum);}
else if (result.code == rclcpp_action::ResultCode::ABORTED)
{
RCLCPP_INFO(this->get_logger(),"被中断！");}
else if (result.code == rclcpp_action::ResultCode::CANCELED)
{
RCLCPP_INFO(this->get_logger(),"被取消！");}
else if (result.code == rclcpp_action::ResultCode::UNKNOWN)
{
RCLCPP_INFO(this->get_logger(),"未知异常！");}
}
private:
rclcpp_action::Client<Progress>::SharedPtr Acclient_;
};
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请提交一个整型数据！");
    return 1;
  }
  
  rclcpp::init(argc,argv);
  auto node =std::make_shared<ProgressActionClient>();
  node->goalsender(atoi(argv[1]));
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}