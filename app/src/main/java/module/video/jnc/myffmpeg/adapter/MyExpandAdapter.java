package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.text.Layout;
import android.util.ArrayMap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;


import com.bumptech.glide.Glide;

import java.io.File;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import module.video.jnc.myffmpeg.R;

public class MyExpandAdapter extends BaseExpandableListAdapter {
//    private ImageLoader imageLoader = null;
//    private DisplayImageOptions options;
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
//        imageLoader = ImageLoader.getInstance();
//        options = new DisplayImageOptions.Builder()
//                .showImageOnLoading(R.mipmap.video_default)
//                .showImageForEmptyUri(R.mipmap.video_default)
//                .showImageOnFail(R.mipmap.video_default)
//                .cacheInMemory(true)
//                .cacheOnDisk(true)
//                .imageScaleType(ImageScaleType.IN_SAMPLE_POWER_OF_2)
//                .bitmapConfig(Bitmap.Config.ARGB_4444)
//                .build();

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
        Glide.with(context).load(Uri.fromFile(new File(listFile.get(groupPosition)[childPosition].getAbsolutePath()))).into(cvh.imgHead);
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
