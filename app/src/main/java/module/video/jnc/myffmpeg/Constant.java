package module.video.jnc.myffmpeg;

import android.os.Environment;

import java.io.File;

/**
 * Created by xhc on 2017/11/21.
 */

public class Constant {
    public static File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
}
