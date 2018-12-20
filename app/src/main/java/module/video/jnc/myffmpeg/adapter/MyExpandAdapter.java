package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.text.Layout;
import android.util.ArrayMap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;

import java.io.File;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import module.video.jnc.myffmpeg.R;

public class MyExpandAdapter extends BaseExpandableListAdapter {
    private ImageLoader imageLoader = null;
    private DisplayImageOptions options;
    private List<String> listName = new ArrayList<>();
    private List<File[]> listFile = new ArrayList<>();
    private Context context;
    private GroupViewHolder gvh;
    private ChildViewHolder cvh;

    public MyExpandAdapter( List<String> listName , List<File[]> listFile, Context context) {
        this.context = context;
        this.listName.addAll(listName);
        this.listFile.addAll(listFile);
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

    @Override
    public int getGroupCount() {
        return listName.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        return listFile.get(groupPosition).length;
    }

    @Override
    public Object getGroup(int groupPosition) {
        return listName.get(groupPosition);
    }

    @Override
    public Object getChild(int groupPosition, int childPosition) {
        return listFile.get(groupPosition)[childPosition];
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.file_group_item_layout, parent, false);
            gvh = new GroupViewHolder();
            gvh.tvName = convertView.findViewById(R.id.tv_name);
            gvh.tvNum = convertView.findViewById(R.id.tv_num);
            gvh.arrow = convertView.findViewById(R.id.img_arrow);
            convertView.setTag(gvh);
        } else {
            gvh = (GroupViewHolder) convertView.getTag();
        }
        gvh.tvName.setText(listName.get(groupPosition));
        gvh.tvNum.setText("(" + listFile.get(groupPosition).length + ")");
        if(isExpanded){
            gvh.arrow.setImageResource(R.mipmap.arrow_down);
        }
        else{
            gvh.arrow.setImageResource(R.mipmap.right_arrow);
        }

        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.file_child_item_layout, parent, false);
            cvh = new ChildViewHolder();
            cvh.imgHead = convertView.findViewById(R.id.img_logo);
            cvh.tvName = convertView.findViewById(R.id.tv_name);
            convertView.setTag(cvh);
        } else {
            cvh = (ChildViewHolder) convertView.getTag();
        }
        imageLoader.displayImage("file://" + listFile.get(groupPosition)[childPosition].getAbsolutePath(), cvh.imgHead, options);
        cvh.tvName.setText(listFile.get(groupPosition)[childPosition].getName());
        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }


    class GroupViewHolder {
        TextView tvName;
        TextView tvNum;
        ImageView arrow ;
    }

    class ChildViewHolder {
        ImageView imgHead;
        TextView tvName;
    }
}
