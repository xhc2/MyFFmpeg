package module.video.jnc.myffmpeg.bean;

public class ScaleBean {
    private String name ;
    private int width ;
    private int height ;
    private boolean choiseFlag ;

    public ScaleBean(String name, int width, int height) {
        this.name = name;
        this.width = width;
        this.height = height;
    }

    public boolean isChoiseFlag() {
        return choiseFlag;
    }

    public void setChoiseFlag(boolean choiseFlag) {
        this.choiseFlag = choiseFlag;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public String getName() {
        return name;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }
}
