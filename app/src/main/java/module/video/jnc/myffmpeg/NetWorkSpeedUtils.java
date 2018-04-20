//package module.video.jnc.myffmpeg;
//
//import android.content.Context;
//import android.net.TrafficStats;
//import android.os.Handler;
//import android.util.Log;
//
//import java.util.Timer;
//import java.util.TimerTask;
//
///**
// * Created by xhc on 2017/12/20.
// */
//
//public class NetWorkSpeedUtils {
//
//
//    private Context context;
//
//    private long lastTotalRxBytes = 0;
//    private long lastTimeStamp = 0;
//
//    public NetWorkSpeedUtils(Context context){
//        this.context = context;
//    }
//
//    TimerTask task = new TimerTask() {
//        @Override
//        public void run() {
//            showNetSpeed();
//        }
//    };
//
//    public void startShowNetSpeed(){
//        lastTotalRxBytes = getTotalRxBytes();
//        lastTimeStamp = System.currentTimeMillis();
//        new Timer().schedule(task, 1000, 1000); // 1s后启动任务，每2s执行一次
//
//    }
//
//    private long getTotalRxBytes() {
//        return TrafficStats.getUidRxBytes(context.getApplicationInfo().uid) == TrafficStats.UNSUPPORTED ? 0 :(TrafficStats.getTotalRxBytes()/1024);//转为KB
//    }
//
//    private void showNetSpeed() {
//        long nowTotalRxBytes = getTotalRxBytes();
//        long nowTimeStamp = System.currentTimeMillis();
//        long speed = ((nowTotalRxBytes - lastTotalRxBytes) * 1000 / (nowTimeStamp - lastTimeStamp));//毫秒转换
//        long speed2 = ((nowTotalRxBytes - lastTotalRxBytes) * 1000 % (nowTimeStamp - lastTimeStamp));//毫秒转换
//
//        lastTimeStamp = nowTimeStamp;
//        lastTotalRxBytes = nowTotalRxBytes;
//
//        Log.e("xhc" , String.valueOf(speed) + "." + String.valueOf(speed2) + " kb/s");
//
//    }
//
//}
