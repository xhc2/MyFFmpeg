package module.video.jnc.myffmpeg.activity;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.widget.FrameLayout;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.tool.FileUtils;

/**
 * 用来处理单路的滤镜Activity
 */
public class SingleFilterActivity extends VideoEditParentActivity {


    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(0));
        }
    }

    private DealFilterThread dealThread;

    protected void stopDealFilter() {
        FFmpegUtils.videoFilterDestroy();
        if (dealThread != null) {
            try {
                dealThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            dealThread = null;
        }
    }

    protected void startDealVideo(String inputPath, String outputPath, String filterDes, int[] params) {
        stopDealFilter();
        startProgressThread();
        dealThread = new DealFilterThread(inputPath , outputPath , filterDes , params);
        dealThread.start();
    }

    @Override
    protected int getProgress(){
        return FFmpegUtils.getVideoFilterProgress();
    }

    @Override
    protected int destroyFFmpeg(){
        FFmpegUtils.videoFilterDestroy();
        return 1;
    }



    private class DealFilterThread extends Thread {
        String videoPath;
        String outputPath;
        String filterDes;
        int[] params;

        public DealFilterThread(String inputPath, String outputPath, String filterDes, int[] params) {
            this.videoPath = inputPath;
            this.outputPath = outputPath;
            this.filterDes = filterDes;
            this.params = params;
        }

        @Override
        public void run() {
            super.run();
            dealFlag = true;

            FFmpegUtils.initVideoFilter(videoPath, outputPath, filterDes, params);
            FFmpegUtils.videoFilterStart();
            dealFlag = false;
        }
    }


    //查看水印进度相关
//    private ProgressThread progressThread;
//
//    private void startProgressThread() {
//        stopProgressThread();
//        progressThread = new ProgressThread();
//        progressThread.progressFlag = true;
//        progressThread.start();
//    }
//
//    private void stopProgressThread() {
//        if (progressThread != null) {
//            progressThread.progressFlag = false;
//            try {
//                progressThread.join();
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//            progressThread = null;
//        }
//    }
//
//    class ProgressThread extends Thread {
//        boolean progressFlag = false;
//
//        @Override
//        public void run() {
//            super.run();
//            while (progressFlag) {
//                progress = FFmpegUtils.getVideoFilterProgress();
//                myHandler.sendEmptyMessage(PROGRESS);
//                try {
//                    sleep(1000);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
//            }
//        }
//    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.removeNotify(this);
        stopDealFilter();
    }

//    @Override
//    public void nativeNotify(String str) {
//        if ( FFmpegUtils.isShowToastMsg(str) ) {
//            Message msg = myHandler.obtainMessage();
//            msg.what = SHOW_NATIVE_MSG ;
//            msg.obj = str ;
//            myHandler.sendMessage(msg);
//        }
//    }

}
