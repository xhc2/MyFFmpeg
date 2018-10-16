package module.video.jnc.myffmpeg.bean;

public class FileBean {
    private String path ;
    private boolean choise;


    public void setPath(String path) {
        this.path = path;
    }

    public void setChoise(boolean choise) {
        this.choise = choise;
    }

    public String getPath() {
        return path;
    }

    public boolean isChoise() {
        return choise;
    }
}
