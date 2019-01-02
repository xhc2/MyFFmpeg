package module.video.jnc.myffmpeg.bean;

public class FileBean {
    private String path ;
    private boolean choise;
    private Integer choiseRid;
    private boolean musicPlay ; //true 播放 ， false 停止


    public void setChoiseRid(Integer choiseRid) {
        this.choiseRid = choiseRid;
    }

    public Integer getChoiseRid() {
        return choiseRid;
    }

    public void setMusicPlay(boolean musicPlay) {
        this.musicPlay = musicPlay;
    }

    public boolean isMusicPlay() {
        return musicPlay;
    }

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
