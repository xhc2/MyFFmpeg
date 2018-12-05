package module.video.jnc.myffmpeg.bean;

public class FilterBean {
    private int logoId;
    private String filterDes;
    private String name ;
    private boolean choiseFlag ;



    public FilterBean(int logoId, String filterDes , String name ) {
        this.logoId = logoId;
        this.filterDes = filterDes;
        this.name = name;
    }

    public void setChoiseFlag(boolean choiseFlag) {
        this.choiseFlag = choiseFlag;
    }

    public boolean isChoiseFlag() {
        return choiseFlag;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setLogoId(int logoId) {
        this.logoId = logoId;
    }

    public void setFilterDes(String filterDes) {
        this.filterDes = filterDes;
    }

    public int getLogoId() {
        return logoId;
    }

    public String getFilterDes() {
        return filterDes;
    }
}
