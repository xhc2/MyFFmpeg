package module.video.jnc.myffmpeg;

/**
 * Created by xhc on 2017/11/23.
 */

public class SDLUtils {


    public static native void initSDLTest();

    static {
        System.loadLibrary("SDL2");
        System.loadLibrary("SDL2main");
    }

}
