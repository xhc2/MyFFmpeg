package module.video.jnc.myffmpeg.activity;

import android.app.Dialog;
import android.content.Intent;
import android.media.MediaPlayer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.Constant;

public class PlayAudioActivity extends AppCompatActivity implements AdapterView.OnItemClickListener{
    private Dialog dialog ;
    private String path ;
    private MediaPlayer mp ;
    private List<File> listFile = new ArrayList<>();
    private List<String> suffixs = new ArrayList<String>();
    private  FileAdater adater ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_audio);

        Intent intent = getIntent();
        if(intent != null){
            path = intent.getStringExtra("path");
        }

        playPath(path);
        addSuffixs();
        findViewById(R.id.bt_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mp != null){
                    mp.start();
                }

            }
        });
        findViewById(R.id.bt_file_list).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showDialog();
            }
        });
    }

    private void addSuffixs(){
        suffixs.add("mp3");
        suffixs.add("flv");
        suffixs.add("aac");
        suffixs.add("pcm");
        suffixs.add("wav");
    }
    private void playPath(String path){

        if(!TextUtils.isEmpty(path)){
            Log.e("xhc" , " path "+path);
            releaseAudio();
            mp = new MediaPlayer();
            try{

                mp.setDataSource(path);
                mp.prepare();
                mp.start();
            }catch(Exception e){
                Toast.makeText(this ,e.getMessage()+" " , Toast.LENGTH_SHORT).show();
                Log.e("xhc",""+ e.getMessage());
            }


        }
    }

    private void releaseAudio(){
        if(mp!= null){
            mp.stop();
            mp.release();
            mp = null;
        }
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
        releaseAudio();
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
        playPath(file.getAbsolutePath());
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

    private class FileAdater extends BaseAdapter {

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
            FileAdater.ViewHolder holder;
            if(view == null){
                holder = new FileAdater.ViewHolder();
                holder.tv = new TextView(PlayAudioActivity.this);
                holder.tv.setPadding(10 , 10 , 10 , 10);
                ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT , ViewGroup.LayoutParams.WRAP_CONTENT );
                holder.tv.setLayoutParams(params);
                view = holder.tv;
                view.setTag(holder);
            }
            else{
                holder = (FileAdater.ViewHolder)view.getTag();
            }
            holder.tv.setText(listFile.get(i).getName());
            return view;
        }

        class ViewHolder{
            TextView tv;
        }
    }

}
