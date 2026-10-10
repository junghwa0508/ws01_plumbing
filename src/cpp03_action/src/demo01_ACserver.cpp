// 编写动作服务器：解析客户端提交的数据，遍历该数据并累加求和，最终结果响应回客户端，且请求响应过程中生成连续反馈。
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/progress.hpp"

using base_interfaces_demo::action::Progress;
using std::placeholders::_1;
using std::placeholders::_2;
class ProgressActionServer : public rclcpp::Node{
public:
  ProgressActionServer() :Node("progress_action_server") 
  {
    RCLCPP_INFO(this->get_logger(),"action服务端创建！");
    /*rclcpp_action::Server<ActionT>::SharedPtr 
    create_server<ActionT, NodeT>(NodeT node, 
    const std::string &name, 
    rclcpp_action::Server<ActionT>::GoalCallback handle_goal, 
    rclcpp_action::Server<ActionT>::CancelCallback handle_cancel, 
    rclcpp_action::Server<ActionT>::AcceptedCallback handle_accepted, const*/
    ACserver_ = rclcpp_action::create_server<Progress>
    (this,"get_sum",
     std::bind(&ProgressActionServer::handle_goal,this,_1,_2),
     std::bind(&ProgressActionServer::handle_cancel,this,_1),
     std::bind(&ProgressActionServer::handle_accepted,this,_1)     
    );
  }
  /*std::function<GoalResponse(const GoalUUID &,std::shared_ptr<const typename ActionT::Goal>)>;
    std::function<CancelResponse(std::shared_ptr<ServerGoalHandle<ActionT>>)>;
    std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;*/
    //处理目标值；
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID & uuid,std::shared_ptr<const Progress::Goal> goal)
    { (void) uuid;
      //业务逻辑：判断提交的数据是否大于1，是则接受，否则拒绝；
      if (goal->num <= 1)
      {
        RCLCPP_INFO(this->get_logger(),"提交的目标值需大于1!");
        return rclcpp_action::GoalResponse::REJECT;
      }
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };
    //处理取消请求；
    rclcpp_action::CancelResponse handle_cancel(std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goalhandler)
    { (void) goalhandler;
      RCLCPP_INFO(this->get_logger(),"收到取消请求！");
      return rclcpp_action::CancelResponse::ACCEPT;
    };
    //生成连续反馈与最终响应；
    void execute(std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goalhandler){
       //1.生成连续反馈并返回给客户端：首先获取目标值，然后遍历该值并累加求和，每循环一次计算进度，作为连续反馈并发布；
      int num = goalhandler->get_goal()->num;
      int sum =0;
      auto feedback = std::make_shared<Progress::Feedback>();
      auto result = std::make_shared<Progress::Result>();
      //设置休眠；
      rclcpp::Rate rate(1.0);
      for (int i = 1; i <= num; i++)
      {
        sum += i;
        double progress = i / (double) num;//计算进度；
        feedback->progress = progress;
        goalhandler->publish_feedback(feedback);
        RCLCPP_INFO(this->get_logger(),"连续反馈中，进度%.2f",progress);
        //判断是否接受到了取消请求，若是则生成最终响应并退出执行函数；
        if(goalhandler->is_canceling())
        {
          result->sum = sum;
          goalhandler->canceled(result);
          RCLCPP_INFO(this->get_logger(),"任务被取消！");
          return;
        }
        rate.sleep();
      }
       //2.生成最终响应结果；
      if (rclcpp::ok())
      {
        result->sum = sum;
        goalhandler->succeed(result);
        RCLCPP_INFO(this->get_logger(),"最终结果：%d",sum);
      }
    }
    void handle_accepted(std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goalhandler)
    { //新建子线程处理耗时的主逻辑实现；
      std::thread(std::bind(&ProgressActionServer::execute,this,goalhandler)).detach(); 
    }
private:
  rclcpp_action::Server<Progress>::SharedPtr  ACserver_;
};
int main(int argc, char *argv[])
{
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<ProgressActionServer>());
  rclcpp::shutdown();
  return 0;
}
