package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.media.MediaPlayer;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.ChoiseMusicAdapter;
import module.video.jnc.myffmpeg.adapter.ChoiseVideoAdapter;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.SelectedMusicAdapter;
import module.video.jnc.myffmpeg.adapter.SelectedVideoAdapter;
import module.video.jnc.myffmpeg.bean.FileBean;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.DividerGridItemDecoration;
import module.video.jnc.myffmpeg.widget.TitleBar;


/**
 * 选择音乐界面
 */
public class ChoiseMusicActivity extends BaseActivity implements MyBaseAdapter.OnRecyleItemClick<FileBean>, ChoiseMusicAdapter.MusicPlayClickInter {

    private RecyclerView recyclerView;
    private ChoiseMusicAdapter adapter;
    private SelectedMusicAdapter selectedMusicAdapter;
    private List<FileBean> listFile = new ArrayList<FileBean>();
    private String[] musicMIME = {"mp3", "aac"};
    private RecyclerView rcChoiseView;
    private TitleBar titleBar;
    private ArrayList<String> listSelected = new ArrayList<>();
    private int choiseMaxNum;
    private int choiseMin;
    private String actionNext;
    private MediaPlayer mediaPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choise_music);
        Intent intent = getIntent();
        if (intent != null) {
            choiseMaxNum = intent.getIntExtra("choise_max_num_music", 1);
            choiseMin = intent.getIntExtra("choise_min_music", 1);
            actionNext = intent.getStringExtra("action_next");
        }

        recyclerView = findViewById(R.id.recycler_view);
        rcChoiseView = findViewById(R.id.rl_choise);
        titleBar = findViewById(R.id.title);
        findFile(FileUtils.APP_MUSIC);
        adapter = new ChoiseMusicAdapter(listFile, this);
        recyclerView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false));
        recyclerView.addItemDecoration(new DividerItemDecoration(this, DividerItemDecoration.VERTICAL));
        recyclerView.setAdapter(adapter);
        adapter.setOnRecyleItemClick(this);
        adapter.setMusicPlayClickInter(this);
        selectedMusicAdapter = new SelectedMusicAdapter(listSelected, this);
        rcChoiseView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false));
        rcChoiseView.setAdapter(selectedMusicAdapter);

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                if (listSelected.size() <= 0) {
                    showToast("请选择音乐！");
                    return;
                }
                if (listSelected.size() < choiseMin) {
                    showToast("至少选择 " + choiseMin + " 个音乐！");
                    return;
                }
                Intent intent = new Intent();
                intent.setAction(actionNext);
                intent.putExtra("choise_max_video", getIntent().getIntExtra("choise_max_video", 1));
                intent.putExtra("action", getIntent().getStringExtra("action"));
                intent.putExtra("music_path", listSelected.get(0));
                startActivity(intent);
                finish();
            }
        });
    }


    private void findFile(String root) {
        File file = new File(root);

        File[] files = file.listFiles();
        if (files == null) return;
        for (File f : files) {
            if (isPic(f.getAbsolutePath())) {
                FileBean fb = new FileBean();
                fb.setPath(f.getAbsolutePath());
                listFile.add(fb);
            }
        }
    }

    private boolean isPic(String path) {
        if (TextUtils.isEmpty(path)) {
            return false;
        }
        for (int i = 0; i < musicMIME.length; ++i) {
            if (path.endsWith(musicMIME[i])) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void onItemClick(View v, FileBean s, int position) {
        if (listSelected.size() >= choiseMaxNum && !s.isChoise()) {
            showToast("只能选取 " + choiseMaxNum + " 首");
            return;
        }
        if (s.isChoise()) {
            s.setChoise(false);
            listSelected.remove(s.getPath());
        } else {
            s.setChoise(true);
            listSelected.add(s.getPath());
        }
        adapter.refreshAllData(listFile);
        selectedMusicAdapter.refreshAllData(listSelected);
    }

    private void startMediaPlayer(String path) {
        if (mediaPlayer == null) {
            mediaPlayer = new MediaPlayer();
            try {
                mediaPlayer.setDataSource(this, Uri.parse(path));
                mediaPlayer.prepare(); //同步准备发方法
                mediaPlayer.start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void stopMediaPlayer(){
        if (mediaPlayer != null) {
            mediaPlayer.stop();
            mediaPlayer.release();
            mediaPlayer = null;
        }
    }

    @Override
    public void musicClick(FileBean fb, boolean oper) {
        Log.e("xhc", " music play " + fb.getPath() + " status " + oper);
        for (FileBean f : listFile) {
            f.setMusicPlay(false);
        }
        fb.setMusicPlay(oper);
        adapter.refreshAllData(listFile);
        if (oper) {
            //播放音乐
            stopMediaPlayer();
            startMediaPlayer(fb.getPath());
        }
        else{
            stopMediaPlayer();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopMediaPlayer();
    }
}
