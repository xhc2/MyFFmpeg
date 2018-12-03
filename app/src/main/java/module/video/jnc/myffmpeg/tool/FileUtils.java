package module.video.jnc.myffmpeg.tool;

import android.graphics.Bitmap;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileUtils {
    public static final String APP_ROOT = "sdcard/FFmpeg/";
    public static final String APP_WATER_MARK = "sdcard/FFmpeg/water_mark/";

    public static String saveFileToWaterMark(String name, byte[] buffer) {
        File file = new File(APP_WATER_MARK);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(APP_WATER_MARK + name);
            fos.write(buffer);
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

    public static String saveBitmapToWaterMark(String name, Bitmap bmp) {
        File file = new File(APP_WATER_MARK);
        if (!file.exists()) {
            file.mkdirs();
        }
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(APP_WATER_MARK + name);
            bmp.compress(Bitmap.CompressFormat.JPEG , 100  ,fos );
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

    public static void makeWaterDir(){
        File file = new File(APP_WATER_MARK);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

}
