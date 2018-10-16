package module.video.jnc.myffmpeg;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;

import java.util.ArrayList;
import java.util.List;

public abstract class MyBaseAdapter<T, V extends RecyclerView.ViewHolder> extends RecyclerView.Adapter<V> {

    protected List<T> list = new ArrayList<T>();
    protected Context context;
    protected OnRecyleItemClick lis ;
    protected DisplayImageOptions options;
    protected ImageLoader imageLoader = null;

    public interface OnRecyleItemClick<T> {
        void onItemClick(View v , T t , int position);
    }

    public MyBaseAdapter(List<T> list, Context context) {
        this.context = context;
        if (list != null) {
            this.list.addAll(list);
        }
    }

    @Override
    public int getItemCount() {
        return list.size();
    }

    public void setOnRecyleItemClick(OnRecyleItemClick lis  ){
        this.lis = lis;
    }

    public void refreshAllData(List<T> list) {
        this.list.clear();
        if (list != null) {
            this.list.addAll(list);
        }
        this.notifyDataSetChanged();
    }

    @Override
    public abstract V onCreateViewHolder(ViewGroup container, int type);

    @Override
    public abstract void onBindViewHolder(V holder, int position);



    protected synchronized void displayImageView(ImageView img, String url) {
        if (options == null) {
            //使用的时候才加载
            imageLoader = ImageLoader.getInstance();
            options = new DisplayImageOptions.Builder()
                    .showImageOnLoading(R.mipmap.ic_launcher) // resource or drawable
                    .showImageForEmptyUri(R.mipmap.ic_launcher) // resource or drawable
                    .showImageOnFail(R.mipmap.ic_launcher) // resource or drawable
                    .cacheInMemory(true) // default
                    .cacheOnDisk(true) // default
                    .imageScaleType(ImageScaleType.IN_SAMPLE_POWER_OF_2) // default
                    .bitmapConfig(Bitmap.Config.ARGB_4444) // default
                    .build();
        }
        imageLoader.displayImage(url, img, options);
    }

}