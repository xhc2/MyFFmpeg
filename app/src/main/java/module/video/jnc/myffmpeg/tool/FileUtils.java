package module.video.jnc.myffmpeg.tool;

import android.graphics.Bitmap;
import android.text.TextUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileUtils {
    public static final String APP_ROOT = "sdcard/FFmpeg/";
    public static final String APP_VIDEO = "sdcard/FFmpeg/video_src/";

    public static final String APP_WATER_MARK = "sdcard/FFmpeg/water_mark/";
    public static final String APP_FILTER = "sdcard/FFmpeg/filter/";
    public static final String APP_CROP = "sdcard/FFmpeg/crop/";
    public static final String APP_SCALE = "sdcard/FFmpeg/scale/";
    public static final String APP_CLIP = "sdcard/FFmpeg/clip/";
    public static final String APP_GIF = "sdcard/FFmpeg/gif/";
    public static final String APP_REVERSE = "sdcard/FFmpeg/reverse/";
    public static final String APP_DUB = "sdcard/FFmpeg/dub/";
    public static final String APP_MUSIC_VIDEO = "sdcard/FFmpeg/video_music/";
    public static final String APP_SPEED = "sdcard/FFmpeg/speed/";
    public static final String APP_MUSIC_SRC = "sdcard/FFmpeg/music_src";
    public static final String APP_FILE_PARSE = "sdcard/FFmpeg/fileparse";
    public static final String APP_HARD= "sdcard/FFmpeg/hard/";
    private static String[] videoMIME = {"mp4" , "flv" , "rmvb" , "ts"};
//    public static String saveFileToWaterMark(String name, byte[] buffer) {
//        File file = new File(APP_WATER_MARK);
//        if (!file.exists()) {
//            file.mkdirs();
//        }
//        FileOutputStream fos = null;
//        try {
//            fos = new FileOutputStream(APP_WATER_MARK + name);
//            fos.write(buffer);
//            fos.flush();
//
//        } catch (Exception e) {
//            e.printStackTrace();
//            return null;
//        } finally {
//            try {
//                fos.close();
//            } catch (IOException e) {
//                e.printStackTrace();
//            }
//        }
//        return APP_WATER_MARK + name;
//    }

    public static boolean  isVideoFile(String path){
        if(TextUtils.isEmpty(path)){
            return false;
        }
        for(int i = 0 ; i < videoMIME.length ; ++ i){
            if(path.endsWith(videoMIME[i])){
                return true;
            }
        }
        return false;
    }

    public static String saveBitmapToWaterMark(String name, Bitmap bmp) {
        File file = new File(APP_WATER_MARK);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(APP_WATER_MARK + name);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);
            fos.flush();

        } catch (Exception e) {
            e.printStackTrace();
            return null;
        } finally {
            try {
                fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return APP_WATER_MARK + name;
    }

    public static void makeWaterDir() {
        File file = new File(APP_WATER_MARK);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeFilterDir() {
        File file = new File(APP_FILTER);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeCropDir() {
        File file = new File(APP_CROP);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeScaleDir() {
        File file = new File(APP_SCALE);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeClipDir() {
        File file = new File(APP_CLIP);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeGifDir() {
        File file = new File(APP_GIF);
        if (!file.exists()) {
            file.mkdirs();
        }
    }
    public static void makeReverse() {
        File file = new File(APP_REVERSE);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeDubDir() {
        File file = new File(APP_DUB);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeMusicVideo() {
        File file = new File(APP_MUSIC_VIDEO);
        if (!file.exists()) {
            file.mkdirs();
        }
    }
    public static void makeSpeedDir() {
        File file = new File(APP_SPEED);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public static void makeFileParse() {
        File file = new File(APP_FILE_PARSE);
        if (!file.exists()) {
            file.mkdirs();
        }
    }
    public static void makeHardDir() {
        File file = new File(APP_HARD);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

}
