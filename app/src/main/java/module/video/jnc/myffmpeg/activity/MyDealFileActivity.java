package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.ArrayMap;
import android.util.Log;
import android.view.View;
import android.widget.ExpandableListView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyExpandAdapter;
import module.video.jnc.myffmpeg.tool.FileUtils;

public class MyDealFileActivity extends BaseActivity implements ExpandableListView.OnChildClickListener {

    private Map<String, File[]> mapFile = new ArrayMap<>();
    private List<String> listName = new ArrayList<>();
    private List<File[]> listFile = new ArrayList<>();
    private ExpandableListView expandableListView;
    private MyExpandAdapter expandAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_deal_file);
        expandableListView = findViewById(R.id.expand_listview);
        init();
    }

    private void init() {
        getFileList();
        expandAdapter = new MyExpandAdapter(listName, listFile, this);
        expandableListView.setGroupIndicator(null);
        expandableListView.setAdapter(expandAdapter);
        expandableListView.setOnChildClickListener(this);
    }

    //把处理的文件遍历出来。
    private void getFileList() {
        File file = new File(FileUtils.APP_ROOT);
        if (!file.exists()) {
            return;
        }
        File[] files = file.listFiles();
        if (files == null) {
            return;
        }
        for (File f : files) {
            if (f == null || f.listFiles() == null || f.listFiles().length <= 0 || !f.isDirectory()) {
                continue;
            }
            List<File> listF = new ArrayList<>();
            for (File fv : f.listFiles()) {
                if (FileUtils.isVideoFile(fv.getAbsolutePath())) {
                    listF.add(fv);
                }
            }
            if (listF.size() <= 0) {
                //没有视频文件
                continue;
            }
            File[] fResult = new File[listF.size()];
            listF.toArray(fResult);
            if (FileUtils.APP_CLIP.contains(f.getName())) {
                mapFile.put("剪辑", fResult);
            } else if (FileUtils.APP_WATER_MARK.contains(f.getName())) {
                mapFile.put("图片水印", fResult);
            } else if (FileUtils.APP_FILTER.contains(f.getName())) {
                mapFile.put("滤镜", fResult);
            } else if (FileUtils.APP_CROP.contains(f.getName())) {
                mapFile.put("裁剪", fResult);
            } else if (FileUtils.APP_SCALE.contains(f.getName())) {
                mapFile.put("分辨率", fResult);
            } else if (FileUtils.APP_GIF.contains(f.getName())) {
                mapFile.put("gif", fResult);
            } else if (FileUtils.APP_REVERSE.contains(f.getName())) {
                mapFile.put("倒放", fResult);
            } else if (FileUtils.APP_DUB.contains(f.getName())) {
                mapFile.put("配音", fResult);
            } else if (FileUtils.APP_MUSIC_VIDEO.contains(f.getName())) {
                mapFile.put("视频音乐", fResult);
            } else if (FileUtils.APP_SPEED.contains(f.getName())) {
                mapFile.put("播放速度", fResult);
            }
        }
        for (String str : mapFile.keySet()) {
            listName.add(str);
            listFile.add(mapFile.get(str));
        }
    }


    @Override
    public boolean onChildClick(ExpandableListView parent, View v, int groupPosition, int childPosition, long id) {
        Intent intent = new Intent(MyDealFileActivity.this, Mp4PlayerActivity.class);
        intent.putExtra("path", listFile.get(groupPosition)[childPosition].getAbsolutePath());
        startActivity(intent);
        Log.e("xhc", " child click " + groupPosition + " child " + childPosition);
        return true;
    }
}
