package module.video.jnc.myffmpeg;

import android.app.Dialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.VideoView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * 用于播放视频
 */
public class PlayVideoActivity extends AppCompatActivity implements AdapterView.OnItemClickListener{

    private VideoView videoView;

    private Dialog dialog ;
    private String playPath;
    private List<File> listFile = new ArrayList<>();
    private List<String> suffixs = new ArrayList<String>();
    private FileAdater adater ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_video);
        videoView = (VideoView)findViewById(R.id.videoview);
        setOnclickListener();
        addSuffixs();
        playPath = getIntent().getStringExtra("path");
        if(playPath != null){
            videoView.setVideoPath(playPath);
            videoView.start();
        }
        createDialog();
    }

    private void setOnclickListener(){
        findViewById(R.id.show_file).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showDialog();
            }
        });
        findViewById(R.id.play_repeat).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                videoView.start();
            }
        });
    }

    private void addSuffixs(){
        suffixs.add("mp4");
        suffixs.add("flv");
        suffixs.add("aac");
        suffixs.add("ts");

    }

    private void createDialog(){
        if(dialog == null){
            dialog = new Dialog(this);
            dialog.setContentView(R.layout.dialog_listview_layout);
            getFileList();
            adater = new FileAdater();
            ListView listView = dialog.findViewById(R.id.listview);
            listView.setAdapter(adater);
            listView.setOnItemClickListener(this);
        }
    }
    private void showDialog(){
        createDialog();
        if(!dialog.isShowing()){
            dialog.show();
        }
    }

    private void dismissDialog(){
        if(dialog != null && dialog.isShowing()){
            dialog.dismiss();
        }
    }

    private void getFileList(){
        File[] files = Constant.rootFile.listFiles();
        if(files != null){
            for(File f : files){
                if(filterFile(f)){
                    listFile.add(f);
                }
            }
        }
        Log.e("xhc" , " file.list "+listFile.size());
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        File file = listFile.get(i);
        videoView.setVideoPath(file.getAbsolutePath());
        videoView.start();
        dismissDialog();
    }

    private boolean filterFile(File file ){
//        for(String str : suffixs){
//            if(file.getName().endsWith(str)){
//                return true;
//            }
//        }
        return true;
    }

    private class FileAdater extends BaseAdapter{

        @Override
        public int getCount() {
            return listFile.size();
        }

        @Override
        public Object getItem(int i) {
            return listFile.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder holder;
            if(view == null){
                holder = new ViewHolder();
                holder.tv = new TextView(PlayVideoActivity.this);
                holder.tv.setPadding(10 , 10 , 10 , 10);
                ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT , ViewGroup.LayoutParams.WRAP_CONTENT );
                holder.tv.setLayoutParams(params);
                view = holder.tv;
                view.setTag(holder);
            }
            else{
                holder = (ViewHolder)view.getTag();
            }
            holder.tv.setText(listFile.get(i).getName());
            return view;
        }

        class ViewHolder{
            TextView tv;
        }
    }

}
