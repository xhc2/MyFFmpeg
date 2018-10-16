package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.ChoiseVideoAdapter;
import module.video.jnc.myffmpeg.bean.FileBean;
import module.video.jnc.myffmpeg.widget.DividerGridItemDecoration;
import module.video.jnc.myffmpeg.MyBaseAdapter;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.SelectedVideoAdapter;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class ChoiseVideoActivity extends BaseActivity  implements MyBaseAdapter.OnRecyleItemClick<FileBean> {

    private RecyclerView recyclerView ;
    private ChoiseVideoAdapter adapter ;
    private SelectedVideoAdapter selectedVideoAdapter;
    private List<FileBean> listFile = new ArrayList<FileBean>();
    private static final String root = "sdcard/FFmpeg/";
    private String[] videoMIME = {"mp4" , "flv" , "rmvb" , "ts"};
    private RecyclerView rcChoiseView;
    private TitleBar titleBar ;
    private ArrayList<String> listSelected = new ArrayList<>();
    private int choiseNum;
    private String action ;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choise_video);
        Intent intent = getIntent();
        if(intent != null){
            choiseNum = intent.getIntExtra("choise_num" , 1);
            action = intent.getStringExtra("action");
        }

        recyclerView = findViewById(R.id.recycler_view);
        rcChoiseView = findViewById(R.id.rl_choise);
        titleBar = findViewById(R.id.title);
        findFile(root);
        adapter = new ChoiseVideoAdapter(listFile  , this);
        recyclerView.setLayoutManager(new GridLayoutManager(this , 3));
        recyclerView.addItemDecoration(new DividerGridItemDecoration(this));
        recyclerView.setAdapter(adapter);
        adapter.setOnRecyleItemClick(this);

        selectedVideoAdapter = new SelectedVideoAdapter(listSelected , this);
        rcChoiseView.setLayoutManager(new LinearLayoutManager(this , LinearLayoutManager.HORIZONTAL , false));
        rcChoiseView.setAdapter(selectedVideoAdapter);

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                if(listSelected.size() <= 0){
                    showToast("请选择视频！");
                    return ;
                }
                Log.e("xhc" , "action "+action);
                Intent intent = new Intent(action);
                intent.putStringArrayListExtra("videos" , listSelected);
                startActivity(intent);
            }
        });
    }

    private void findFile(String root){
        File file = new File(root);
        if(file.isDirectory()){
            File[] files = file.listFiles();
            if(files == null) return ;
            for(File f : files){
                findFile(f.getAbsolutePath());
            }
        }
        else{
            if(isVideo(file.getAbsolutePath())){
                FileBean fb = new FileBean();
                fb.setPath(file.getAbsolutePath());
                listFile.add(fb);
            }
        }
    }

    private boolean isVideo(String path){
        if(TextUtils.isEmpty(path)){
            return false;
        }
        for(int i = 0 ; i < videoMIME.length ; ++ i){
            if(path.endsWith(videoMIME[i])){
                return true;
            }
        }
        return false;
    }


    @Override
    public void onItemClick(View v, FileBean s, int position) {
        if(listSelected.size() >= choiseNum && !s.isChoise()){
            return ;
        }
        if(s.isChoise()){
            s.setChoise(false);
            listSelected.remove(s.getPath());
        }
        else{
            s.setChoise(true);
            listSelected.add(s.getPath());
        }
        adapter.refreshAllData(listFile);
        selectedVideoAdapter.refreshAllData(listSelected);
    }
}











